//Libraries
#include <Arduino.h>
#include "Wire.h"       // For I2C/TWI devices to communicate with the Arduino i.e. the temp and humid. sensor and sunlight sensor
#include "DHT.h"        // For temperature and humidity sensor
#include "Si115x.h"     // For sunlight sensor

//Definitions for constant values
#define DHTTYPE DHT20   // DHT 20 is the current iteration of the temp & hum. sensor
DHT dht(DHTTYPE);       // DHT object for calling temp and humidity library methods
Si115X si1151;          // Sunlight sensor object for calling library methods

void setup() {
  Serial.begin(9600);    // Serial communication speed
  Wire.begin();
  dht.begin();  
  Serial.println("\n################################################"); 
}

void loop() {
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

  Serial.println("\n################################################"); // hashes to separate each loop for readability
  delay(10000); //  10 second delay between loops
}