//Libraries
#include <Arduino.h>
#include "Wire.h"       // For I2C/TWI devices to communicate
#include "DHT.h"        // For temperature and humidity sensor

//Definitions for constant values
#define DHTTYPE DHT20   // DHT 20 is the current iteration of the temp & hum. sensor
DHT dht(DHTTYPE);       //   Define pin

void setup() {
  Serial.begin(9600);    // Serial communication speed
  Wire.begin();
  dht.begin();  
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

  Serial.println("\n################################################"); // For readability
  delay(10000);
}