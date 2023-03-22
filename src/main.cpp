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
String path = "Users/" + userName + "/Plants/" + plantName;            // Name of "table" containing data. Temporary for testing.
String jsonStr;                         // String for storing JSON data

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

  Serial.println("\n################################################"); 
}

void loop() {
  // Soil moisture code
  int soilMoistureValue = analogRead(A3);
  Serial.print("Soil moisture: ");
  Serial.print(soilMoistureValue);
  Serial.println();

  // Sunlight code
  Serial.print("IR: ");
  Serial.print(si1151.ReadHalfWord());
  Serial.print("\t\tVisible: ");
  Serial.print(si1151.ReadHalfWord_VISIBLE());
  Serial.print("\t\tUV: ");
  Serial.print(si1151.ReadHalfWord_UV());
  Serial.println();

  // Temperature and humidity code
  float temp_hum_val[2] = {0};
  if (!dht.readTempAndHumidity(temp_hum_val)) {
    Serial.print("Humidity: ");
    Serial.print(temp_hum_val[0]);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temp_hum_val[1]);
    Serial.print(" *C");
  } else {
    Serial.println("Failed to get temperature and humidity value.");
  }
  Serial.println();

  // Firebase code
  // Send data to Firebase with specific path
  if (Firebase.setInt(firebaseData, path + "/soil_Moisture", soilMoistureValue)) {
    Serial.println(firebaseData.dataPath() + " = " + soilMoistureValue);
  } 
   if (Firebase.setFloat(firebaseData, path + "/humidity", temp_hum_val[0])) {
    Serial.println(firebaseData.dataPath() + " = " + temp_hum_val[0]);
  } 
  if (Firebase.setFloat(firebaseData, path + "/temperature", temp_hum_val[1])) {
    Serial.println(firebaseData.dataPath() + " = " + temp_hum_val[1]);
  }
  if (Firebase.setInt(firebaseData, path + "/irLight", si1151.ReadHalfWord())) {
    Serial.println(firebaseData.dataPath() + " = " + si1151.ReadHalfWord());
  }
  if (Firebase.setInt(firebaseData, path + "/visLight", si1151.ReadHalfWord_VISIBLE())) {
    Serial.println(firebaseData.dataPath() + " = " + si1151.ReadHalfWord_VISIBLE());
  }
  if (Firebase.setFloat(firebaseData, path + "/uvLight", si1151.ReadHalfWord_UV())) {
    Serial.println(firebaseData.dataPath() + " = " + si1151.ReadHalfWord_UV());
  }

  // Push data in json strin using pushJSON to a history path
  // light sensor is currently broken and pushJSON can't work with null values so it is omitted
  String jsonStr = "{\"soil_Moisture\":" + String(soilMoistureValue) + ",\"humidity\":" + String(temp_hum_val[0]) + ",\"temperature\":" + String(temp_hum_val[1]) + "}";
  if (Firebase.pushJSON(firebaseData, path + "/history", jsonStr)) {
    Serial.println(firebaseData.dataPath() + " = " + firebaseData.pushName());
  }
  else {
    Serial.println("Error: " + firebaseData.errorReason());
  }

  Serial.println("\n################################################"); // hashes to separate each loop for readability
  delay(60000); //  minute delay between loops
}