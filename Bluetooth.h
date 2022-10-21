#include <BluetoothSerial.h>

//ESP32 Bluetooth checks
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

//Bluetooth control
int controller = 0;
String cmd = "";
void btcontroller()
{
  //Read data from bluetooth
  while (SerialBT.available()) {
    cmd += (char)SerialBT.read();
  }

  //Bluetooth controller commands
  if (cmd != "") {
    Serial.print("Command received : ");
    Serial.println(cmd);
    if (cmd == "sit") {
      controller = 1;
    } else if (cmd == "stand") {
      controller = 2;
    } else if (cmd == "f") {
      controller = 3;
    }
    cmd = ""; //reset cmd
  }

  if (controller == 1) {
    transition(presentposition, servoBASE);
    presentposition[0] = servoBASE[0];
    presentposition[1] = servoBASE[1];

  } else if (controller == 2) {
    transition(presentposition, servoSTAND);
    presentposition[0] = servoSTAND[0];
    presentposition[1] = servoSTAND[1];

  } else if (controller == 3) {
    if (!check) {
      transition(presentposition, servoSTAND);
      check = true;
    }
    walktransition();

    presentposition[0] = servoSTAND[0];
    presentposition[1] = servoSTAND[1];
  }

  if (controller != 3) {
    check = false;
  }
}