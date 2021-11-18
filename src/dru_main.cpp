#include "Arduino.h"

// includes
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <Encoder.h>

#include <actuator.h>
// Printing with stream operator helper functions
// template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
// template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }


////////////////////////////////
// Set up serial pins to the ODrive
////////////////////////////////

// Below are some sample configurations.
// You can comment out the default Teensy one and uncomment the one you wish to use.
// You can of course use something different if you like
// Don't forget to also connect ODrive GND to Arduino GND.

// Teensy 3 and 4 (all versions) - Serial1
// pin 0: RX - connect to ODrive TX
// pin 1: TX - connect to ODrive RX
// See https://www.pjrc.com/teensy/td_uart.html for other options on Teensy

//HardwareSerial& odrive_serial = Serial1;

#define input_pot 14

#define enc_A 20
#define enc_B 21
#define enc_PPR 2048
#define enc_index 22

#define hall_inbound 10
#define hall_outbound 11

// ODrive object
//ODriveArduino odrive(Serial1);
//serial_(Serial1) {}

Actuator actuator(Serial1, enc_A, enc_B, 0, 0, hall_inbound, hall_outbound);
//Encoder encoder(enc_A, enc_B);

void setup() {
  actuator.initialize();

  // // ODrive uses 115200 baud
  // odrive_serial.begin(115200);

  // // Serial to PC
  // Serial.begin(115200);
  // while (!Serial) ; // wait for Arduino Serial Monitor to open

  // Serial.println("ODriveArduino");
  // Serial.println("Setting parameters...");

  // // In this example we set the same parameters to both motors.
  // // You can of course set them different if you want.
  // // See the documentation or play around in odrivetool to see the available parameters

  // Serial.println("Ready!");
  // Serial.println("Send the character '0' or '1' to calibrate respective motor (you must do this before you can command movement)");
  // Serial.println("Send the character 's' to exectue test move");
  // Serial.println("Send the character 'b' to read bus voltage");
  // Serial.println("Send the character 'p' to read motor positions in a 10s loop");
  // Serial.println("Send the character 'q' for potentiometer velocity control");
  // odrive.run_state(0, 1, true);
}

void loop() {
  //actuator.control_function();

  // Serial.print(digitalReadFast(hall_inbound));
  // Serial.print(", ");
  // Serial.print(digitalReadFast(hall_outbound));
  // Serial.print(", ");
  // Serial.println(actuator.get_encoder_pos());
  
  // if (Serial.available()) {
  //   char c = Serial.read();

  //   // Run calibration sequence
  //   if (c == '0' || c == '1') {
  //     int motornum = c-'0';
  //     int requested_state;

  //     //odrive.EncoderConfig(c);
  //     requested_state = 3;
  //     Serial << "Axis" << c << ": Requesting state " << requested_state << '\n';
  //     if(!odrive.run_state(motornum, requested_state, true)) return;
  //     Serial<<odrive.DumpErrors()<<"\n";
  //   }

  //   // Follow Potentiometer for Velocity
  //   if (c == 'q') {
  //     float goalVel = 0;
  //     odrive.run_state(0, 8, false, 10);
  //     for(float i = 0; i <100; i++) {
  //       goalVel = map(analogRead(input_pot), 0, 1023, 0, 20);
  //       odrive.SetVelocity(0, goalVel);
  //       delay(100);
  //     }
  //     odrive.SetVelocity(0, 0);
  //     odrive.run_state(0, 1, false, 10);
  //   }

  //   // Sinusoidal test move
  //   if (c == 's') {
  //       Serial.println("Executing test spin");
  //       odrive.run_state(0, 8, false, 10);

  //       for(float i = 5; i<20; i += 1) {
  //           Serial<<"Velocity: "<<i<<"\n";
  //           odrive.SetVelocity(0, i);
  //           delay(1000);
  //       }
  //       odrive.SetVelocity(0, 0);
  //       odrive.run_state(0, 1, false, 10);

  //       Serial<<odrive.DumpErrors()<<"\n";
  //   }

  //   // Read bus voltage
  //   if (c == 'b') {
  //     odrive_serial << "r vbus_voltage\n";
  //     Serial << "Vbus voltage: " << odrive.readFloat() << '\n';
  //   }

  //   // print motor positions in a 10s loop
  //   if (c == 'p') {
  //     static const unsigned long duration = 10000;
  //     unsigned long start = millis();
  //     while(millis() - start < duration) {
  //       for (int motor = 0; motor < 2; ++motor) {
  //         Serial << odrive.GetVelocity(motor) << '\t';
  //       }
  //       Serial << '\n';
  //     }
  //   }
  // }
}