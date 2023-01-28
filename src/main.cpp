#include <Arduino.h>

void setup() {
  Serial.begin(9600);    // Serial communication speed
  
}

void loop() {
  // Soil moisture code
  int soilMoistureValue = analogRead(A3);
  Serial.print("Soil moisture: ");
  Serial.print(soilMoistureValue);
  Serial.println();

  delay(10000);
}