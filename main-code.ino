//Libraries
#include <Wire.h>
#include <math.h>
#include "kinematics.h"
#include "Bluetooth.h"
#include "ultrasonic.h"
#include "LCD.h"
#include "MPU6050.h"
#include "OLED.h"

void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  //Initialize Bluetooth Serial Port
  SerialBT.begin("SPOTPARAHOT");

  //PWM Initialization
  kinematicsinit();
}

void loop() {

/*
   //manual value input (Used for testing purposes only)
    while (Serial.available() == 0) {}
        int writeY = Serial.parseInt();
  
      while (Serial.available() == 0) {}
        int writeX = Serial.parseInt();
  
        servopos(writeY, writeX, 2);
  
        Serial.print(writeY);
        Serial.print(" ");
        Serial.println(writeX);
*/

  btcontroller();
}
