#ifndef OLED_H
#define OLED_H
#include <iostream>
#include <map>
#include <Arduino.h>
#include <Constant.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

class Oled
{
public:
  void init(std::map<String, int>constants);
  void moveUp();
  void moveDown();
  void increase();
  void decrease();
  void off();
  std::map<String,int> createMap();

private:
  
};

#endif  //! OLED_H