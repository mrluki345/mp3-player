#include <Arduino.h>

float BatteryLevelFinder(int PIN_BATTERY) {
  // 1. Read the raw voltage number (0 to 1023)
  int rawBattery = analogRead(PIN_BATTERY);

  // 2. Map the 4.2V (Full) and 3.0V (Empty) readings to a percentage
  // 4.2V cut in half is 2.1V, which the Teensy reads as ~651
  // 3.0V cut in half is 1.5V, which the Teensy reads as ~465
  int batteryPercent = map(rawBattery, 465, 651, 0, 100);

  // 3. Constrain it so we don't accidentally print "105%" or "-2%"
  batteryPercent = constrain(batteryPercent, 0, 100); 

  return batteryPercent;
}