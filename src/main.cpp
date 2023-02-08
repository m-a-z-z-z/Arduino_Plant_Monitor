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
String path = "/Plant_Name";            // Name of "table" containing data. Temporary for testing.

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
  Serial.println(WiFi.status());
  // Soil moisture code
  int soilMoistureValue = analogRead(A3);
  Serial.print("Soil moisture: ");
  Serial.print(soilMoistureValue);
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

  // Sunlight code
  Serial.print("IR: ");
  Serial.print(si1151.ReadHalfWord());
  Serial.print("\t\tVisible: ");
  Serial.print(si1151.ReadHalfWord_VISIBLE());
  Serial.print("\t\tUV: ");
  Serial.print(si1151.ReadHalfWord_UV());
  Serial.println();

  // Firebase code
  // Send data to Firebase with specific path
  if (Firebase.setInt(firebaseData, path + "/Soil_Moisture", soilMoistureValue)) {
    Serial.println(firebaseData.dataPath() + " = " + soilMoistureValue);
  } 
   if (Firebase.setFloat(firebaseData, path + "/Temp_Humid/Humidity", temp_hum_val[0])) {
    Serial.println(firebaseData.dataPath() + " = " + temp_hum_val[0]);
  } 
  if (Firebase.setFloat(firebaseData, path + "/Temp_Humid/Temperature", temp_hum_val[1])) {
    Serial.println(firebaseData.dataPath() + " = " + temp_hum_val[1]);
  }
  if (Firebase.setInt(firebaseData, path + "/Sunlight/IR", si1151.ReadHalfWord())) {
    Serial.println(firebaseData.dataPath() + " = " + si1151.ReadHalfWord());
  }
  if (Firebase.setInt(firebaseData, path + "/Sunlight/Visible", si1151.ReadHalfWord_VISIBLE())) {
    Serial.println(firebaseData.dataPath() + " = " + si1151.ReadHalfWord_VISIBLE());
  }
  if (Firebase.setFloat(firebaseData, path + "/Sunlight/UV", si1151.ReadHalfWord_UV())) {
    Serial.println(firebaseData.dataPath() + " = " + si1151.ReadHalfWord_UV());
  }

  Serial.println("\n################################################"); // hashes to separate each loop for readability
  delay(10000); //  10 second delay between loops
}

