//Libraries
#include <Arduino.h>
#include "Wire.h"                       // For I2C/TWI devices to communicate with the Arduino i.e. the temp and humid. sensor and sunlight sensor
#include "DHT.h"                        // For temperature and humidity sensor
#include "Si115X.h"                     // For sunlight sensor
#include "Firebase_Arduino_WiFiNINA.h"  // For Firebase, also includes WiFiNINA library
#include "secrets.h"

//Definitions for constant values
#define DHTTYPE DHT20                   // DHT 20 is the current iteration of the temp & hum. sensor
DHT dht(DHTTYPE);                       // DHT object for calling temp and humidity library methods
Si115X si1151;                          // Sunlight sensor object for calling library methods
FirebaseData firebaseData;              // Firebase object for calling library methods
String path = "Users/" + userName + "/Plants/" + plantID;            // Name of "table" containing data. Temporary for testing.
String jsonStr;                         // String for storing JSON data

// Sensor vartiables
const int dryValue = 720;               // Value of soil moisture sensor when not in any water or soil
const int waterValue = 500;             // Value of soil moisture sensor when in water
int soilMoistureValue, soilMoisturePercentage, irValue, visValue;
float temp_hum_val[2] = {0}, uvValue, humidityValue, temperatureValue;


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

void setup() {
  Serial.begin(9600);     // Serial communication speed
  Wire.begin();
  dht.begin();
  wifi_connect();
  firebase_connect();

  if(!si1151.Begin()) {
    Serial.println("Si115X sensor not found");
    while(1);
  } else {
    Serial.println("Si115X sensor found");
  }


  Serial.println("\n################################################"); 
}

void loop() {
  // Sensor readings
  //Soil moisture
  int soilMoistureValue = analogRead(A3);
  soilMoisturePercentage = constrain(map(soilMoistureValue, dryValue, waterValue, 0, 100), 0, 100); // Map soil moisture value to percentage (100% moist - 0% moist)
  // sunlight
  irValue = si1151.ReadHalfWord(); 
  visValue = si1151.ReadHalfWord_VISIBLE(); 
  uvValue = si1151.ReadHalfWord_UV();
  // temp and humidity
  if (!dht.readTempAndHumidity(temp_hum_val)) {
    humidityValue = temp_hum_val[0]; 
    temperatureValue = temp_hum_val[1];
  } else {
    humidityValue = 0;
    temperatureValue = 0;
    Serial.println("Failed to get temperature and humidity value.");
  }

  // For debugging
  Serial.print("Soil moisture: ");
  Serial.print(soilMoistureValue);
  Serial.print("\t\tSoil moisture percentage: ");
  Serial.print(soilMoisturePercentage);
  Serial.print("%");
  Serial.println();
  Serial.print("IR: ");
  Serial.print(irValue);
  Serial.print("\t\tVisible: ");
  Serial.print(visValue);
  Serial.print("\t\tUV: ");
  Serial.print(uvValue);
  Serial.println();
  Serial.print("Humidity: ");
  Serial.print(humidityValue);
  Serial.print("%\t");
  Serial.print("Temperature: ");
  Serial.print(temperatureValue);
  Serial.print("°C");
  Serial.print("\n\n");

  // Firebase code
  // Send data to Firebase with specific path
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

  // Push data in json strin using pushJSON to a history path
  jsonStr = 
    "{\"soilMoisture\":" + String(soilMoisturePercentage) 
    + ",\"irLight\":" + String(irValue)
    + ",\"visLight\":" + String(visValue)
    + ",\"uvLight\":" + String(visValue)
    + ",\"humidity\":" + String(humidityValue) 
    + ",\"temperature\":" + String(temperatureValue) + "}";
  if (Firebase.pushJSON(firebaseData, path + "/history", jsonStr)) {
    Serial.println(firebaseData.dataPath() + " = " + firebaseData.pushName());
  }
  else {
    Serial.println("Error: " + firebaseData.errorReason());
  }

  Serial.println("################################################"); // hashes to separate each loop for readability
  delay(60000); //  minute delay between loops
}