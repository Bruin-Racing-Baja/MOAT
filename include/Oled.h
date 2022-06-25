#ifndef oled_h
#define oled_h

#include <iostream>
#include <Arduino.h>
#include <Constant.h>
#include <EduIntro.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>
#include <iostream>
#include <utility>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

class Oled
{
public:
  Oled(Constant constant);
  void init();
  void write();
  void moveUpOrDown();
  void change();
  void power();
  void clear();

private:
  std::vector<std::pair<String,int>> constants={
   {"Testing1",24},
   {"Testing2",25},
   {"Testing3",26},
   {"Name4",27},
   {"Name5",28},
   {"Name6",29},
   {"Name7",30},
   {"Name8",31},
   {"Name9",32}
  };
  Button up;
  Button down;
  Button center;
  Button right;
  Button left;
  Constant constant;
};

#endif  //! OLED_H