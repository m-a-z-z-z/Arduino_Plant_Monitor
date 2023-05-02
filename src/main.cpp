//Libraries
#include <Arduino.h>
#include "Wire.h"                       // For I2C/TWI devices to communicate with the Arduino i.e. the temp and humid. sensor and sunlight sensor
#include "DHT.h"                        // For temperature and humidity sensor
#include "Si115X.h"                     // For sunlight sensor
#include "Firebase_Arduino_WiFiNINA.h"  // For Firebase, also includes WiFiNINA library
#include "Ticker.h"                     // For scheduling tasks
#include "secrets.h"                    // For WiFi and Firebase credentials

#define DHTTYPE DHT20                   // DHT 20 is the current iteration of the temp & hum. sensor
DHT dht(DHTTYPE);                       // DHT object for calling temp and humidity library methods
Si115X si1151;                          // Sunlight sensor object for calling library methods
FirebaseData firebaseData;              // Firebase object for calling library methods

// Sensor variables
const int dryValue = 720;               // Value of soil moisture sensor when not in any water or soil
const int waterValue = 500;             // Value of soil moisture sensor when in water
int soilMoistureValue, soilMoisturePercentage, irValue, visValue;
float temp_hum_val[2] = {0}, uvValue, humidityValue, temperatureValue;


// declare functions
void wifi_connect();
void firebase_connect();
void getSoilMoisture();
void getSunlight();
void getTempAndHumidity();
void sendToFirebase();
void createSnapshot();
void checkPaths();

// Ticker objects for scheduling tasks
Ticker
timer_getSoilMoisture(getSoilMoisture, 5000, 0),
timer_getLight(getSunlight, 5000, 0),
timer_getTempHum(getTempAndHumidity, 5000, 0),
timer_sendToFb(sendToFirebase, 5000, 0),
timer_createSnapshot(createSnapshot, 300000, 0);  // snapshots are created every 5 minutes for history in database

void setup() {
  Serial.begin(9600);     // Serial communication speed
  Wire.begin();
  dht.begin();
  wifi_connect();
  firebase_connect();
  checkPaths();

  if (!si1151.Begin())
    Serial.println("Si1151 is not ready!");
  else
    Serial.println("Si1151 is ready!");
  
  // start ticker objects
  timer_getSoilMoisture.start();
  timer_getLight.start();
  timer_getTempHum.start();
  timer_sendToFb.start();
  timer_createSnapshot.start();
  Serial.println("\n################################################"); 
}

void loop() {
  timer_getSoilMoisture.update();
  timer_getLight.update();
  timer_getTempHum.update();
  timer_sendToFb.update();
  timer_createSnapshot.update();
}

void wifi_connect() {
  // WiFi setup
  Serial.println("Connecting to WiFi...");
  int status = WL_IDLE_STATUS;
  while(status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println(WiFi.status());      // idle = 0, no SSID available = 1, scan completed = 2, connected = 3, connection failed = 4, disconnected = 5, attempting to connect = 6
    delay(300);
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void firebase_connect() {
  // Firebase setup
  Serial.println("\nConnecting to Firebase...");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);
}

void getSoilMoisture() {
  int readings = 0;
  for (int i=0; i<20; i++) {
    readings += analogRead(A3);
    soilMoistureValue = readings / 20;
  }
  soilMoisturePercentage = constrain(map(soilMoistureValue, dryValue, waterValue, 0, 100), 0, 100); // Map soil moisture value to percentage (100% moist - 0% moist)
  Serial.print("Soil moisture: ");
  Serial.print(soilMoistureValue);
  Serial.print("\t\tSoil moisture percentage: ");
  Serial.print(soilMoisturePercentage);
  Serial.print("%");
  Serial.println();
}

void getSunlight() {
  // sunlight
  irValue = si1151.ReadHalfWord(); 
  visValue = si1151.ReadHalfWord_VISIBLE(); 
  uvValue = si1151.ReadHalfWord_UV();

  Serial.print("IR: ");
  Serial.print(irValue);
  Serial.print("\t\tVisible: ");
  Serial.print(visValue);
  Serial.print("\t\tUV: ");
  Serial.print(uvValue);
  Serial.println();
}

void getTempAndHumidity() {
  if (!dht.readTempAndHumidity(temp_hum_val)) {
    humidityValue = temp_hum_val[0]; 
    temperatureValue = temp_hum_val[1];
  } else {
    humidityValue = 0;
    temperatureValue = 0;
    Serial.println("Failed to get temperature and humidity value.");
  }
  
  Serial.print("Humidity: ");
  Serial.print(humidityValue);
  Serial.print("%\t");
  Serial.print("Temperature: ");
  Serial.print(temperatureValue);
  Serial.print("°C");
  Serial.print("\n\n");
}

void checkPaths() {
  // This method is purely for if a user enters the plantID wrong and creates a new plant they didn't add through the app accidentally.
  // If a new plant is added through the Arduino it will be missing the max and min values for various attributes casuing a crash in the app.
  // This method checks if the max and min values exist and if not adds them to the database to prevent a crash in the app.
  // The user can then delete the accidental plant from the app.
  String path = "Users/" + userName + "/Plants/" + plantID;

  Serial.println("Checking paths...");

  if (Firebase.get(firebaseData, path + "/maxLight")) {
    Serial.println("maxLight not found");
    Firebase.setInt(firebaseData, path + "/maxLight", 10000);
  }
  if (Firebase.get(firebaseData, path + "/minLight")) {
    Serial.println("minLight not found");
    Firebase.setInt(firebaseData, path + "/minLight", 0);
  }
  if (Firebase.get(firebaseData, path + "/maxTemp")) {
    Serial.println("maxTemp not found");
    Firebase.setInt(firebaseData, path + "/maxTemp", 60);
  }
  if (Firebase.get(firebaseData, path + "/minTemp")) {
    Serial.println("minTemp not found");
    Firebase.setInt(firebaseData, path + "/minTemp", -10);
  }
  if (Firebase.get(firebaseData, path + "/maxSoilMoisture")) {
    Serial.println("maxMoisture not found");
    Firebase.setInt(firebaseData, path + "/maxSoilMoisture", 100);
  }
  if (Firebase.get(firebaseData, path + "/minSoilMoisture")) {
    Serial.println("minSoilMoisture not found");
    Firebase.setInt(firebaseData, path + "/minSoilMoisture", 0);
  }
  if (Firebase.get(firebaseData, path + "/photoUrl")) {
    Serial.println("photoUrl not found");
    Firebase.setString(firebaseData, path + "/photoUrl", "plant_default.png");
  }
  if (Firebase.get(firebaseData, path + "/plantID")) {
    Serial.println("plantID not found");
    Firebase.setString(firebaseData, path + "/plantID", plantID);
  }
  if (Firebase.get(firebaseData, path + "/plantName")) {
    Serial.println("plantName not found");
    Firebase.setString(firebaseData, path + "/plantName", "Plant");
  }
  if (Firebase.get(firebaseData, path + "/plantType")) {
    Serial.println("plantType not found");
    Firebase.setString(firebaseData, path + "/plantType", "Plant");
  }

}

void sendToFirebase() {
  String path = "Users/" + userName + "/Plants/" + plantID;

  // Send data to Firebase under the specified path
  if (Firebase.setInt(firebaseData, path + "/soilMoisture", soilMoisturePercentage)) {
    Serial.println(firebaseData.dataPath() + " = " + soilMoisturePercentage);
  } 
  if (Firebase.setFloat(firebaseData, path + "/humidity", humidityValue)) {
    Serial.println(firebaseData.dataPath() + " = " + humidityValue);
  } 
  if (Firebase.setFloat(firebaseData, path + "/temperature", temperatureValue)) {
    Serial.println(firebaseData.dataPath() + " = " + temperatureValue);
  }
  if (Firebase.setInt(firebaseData, path + "/irLight", irValue)) {
    Serial.println(firebaseData.dataPath() + " = " + irValue);
  }
  if (Firebase.setInt(firebaseData, path + "/visLight", visValue)) {
    Serial.println(firebaseData.dataPath() + " = " + visValue);
  }
  if (Firebase.setFloat(firebaseData, path + "/uvLight", uvValue)) {
    Serial.println(firebaseData.dataPath() + " = " + uvValue);
  }

  Serial.println("################################################");
}

void createSnapshot() {
  String path = "Users/" + userName + "/Plants/" + plantID + "/history";

  String jsonStr = 
    "{\"soilMoisture\":" + String(soilMoisturePercentage) 
    + ",\"irLight\":" + String(irValue)
    + ",\"visLight\":" + String(visValue)
    + ",\"uvLight\":" + String(uvValue)
    + ",\"humidity\":" + String(humidityValue) 
    + ",\"temperature\":" + String(temperatureValue) + "}";
  if (Firebase.pushJSON(firebaseData, path, jsonStr)) {
    Serial.println(firebaseData.dataPath() + " = " + firebaseData.pushName());
  }
  else {
    Serial.println("Error: " + firebaseData.errorReason());
  }
  Serial.println("Snapshot added to database.");
  Serial.println("################################################");
}
