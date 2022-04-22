#include <Oled.h>
#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

void Oled::init(std::map<String,int> ok)
{
    Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

  // initialize OLED display with address 0x3C for 128x64
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 initialization failed"));
        while (true);
    }

    oled.clearDisplay(); // clear display

    oled.setTextSize(1);          // text size
    oled.setTextColor(WHITE);     // text color
    oled.setCursor(0, 10);        // position to display
    oled.println("Baja Electronics!"); // text to display
    for(auto i = ok.cbegin(); i != ok.cend(); ++i){
      oled.println(i->first + " " + i->second + "\n");
    }
    oled.display();               // show on OLED
};
void Oled::moveUp(){

};
void Oled::moveDown(){

};
void Oled::increase(){

};
void Oled::decrease(){

};
void Oled::off(){

};
std::map<String,int> Oled::createMap(){
    std::map<String,int> constants;
    constants.insert(std::pair<String,int>("Name",24));
};



