/* 
MOAT - Model 2O Awesome Teensy
mk. V.0
General code to oversee all functions of the Teensy




*/

#include <Arduino.h>

//Libraries
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//SETTINGS
#define req_serial 0 //Halts program until serial connection established
#define req_display 1 //Halts program if display allocation fails

//Pin numbers (first 3 for radio)
#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3
#define OLED_RESET -1 // Reset pin same as arduino (display)

//ANTENNAE CONSTANTS AND DRIVER
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

//DISPLAY CONSTANTS AND INSTANCE
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
/*---------------------------[Serial Init]---------------------------*/
  Serial.begin(9600);

  if (REQ_SERIAL) {
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Serial connected and confirmed");
  }
  else {
    Serial.println("Serial connected but not confirmed");
  }

/*---------------------------[Display Init]---------------------------*/
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      if (req_display) {
        Serial.println(F("SSD1306 allocation failed, halting as it is required"));
        for(;;);
      }
      else {
        Serial.println(F("SSD1306 allocation failed, continuing"));
      }
    }

  Serial.println("Established connection with display");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("BruinRacing: Baja");
  display.setTextSize(1);
  display.println("turboencabulator mk.V");
  display.println("keep it secret..");
  display.println("..keep it safe");
  display.display();
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
}