#include <EduIntro.h>
#include <Constant.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 
#include <Oled.h>
#include <vector>
#include <iostream>
#include <utility>
using namespace std;

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, 4);

int place=0;
int first_visible=0;
bool onstate;

Oled::Oled(Constant constant_in)
 : up(constant_in.up_button_pin), down(constant_in.down_button_pin), center(constant_in.center_button_pin), right(constant_in.right_button_pin), left(constant_in.left_button_pin)
{
  Constant constant = constant_in;
}

void Oled::init()
{
    
    oled.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
    oled.display(); 
    oled.clearDisplay();
    onstate=true;
};

void Oled::write(){
    int cursor=10;
    //cursor placement
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(cursor, 0);

    oled.println("Baja Electronics!");  
    //title
    cursor=cursor+10;
    if(first_visible+3 != constants.size() && (place-first_visible)>3){
      first_visible++;
      
    }
    else if(first_visible !=0 && (place-first_visible)<3){
      first_visible--;
      
    }
    
    
    for(int i=first_visible ; i < constants.size(); i++){
      oled.setCursor(10,cursor);
      oled.println(constants[i].first + " " + constants[i].second + "\n");
      cursor=cursor+10;
    }
    if(first_visible!=0)
      oled.setCursor(2,50);
    else
      oled.setCursor(2, (place * 10) + 20);
    oled.println(">");
    oled.display(); 
}

void Oled::moveUpOrDown(){
  if(up.pressed()){
    if(place<=0){
      place=(constants.size()-1);
    }
    else{
      place--;
    }
  }
  if(down.pressed()){
    if(place>=(constants.size()-1)){
      place=0;
    }
    else{
      place++;
    }
  }
};

void Oled::change(){
  if(right.pressed()){
    switch(place){
      case 0:
        constants[0].second+=1;
        break;
      case 1:
        constants[1].second+=1;
        break;
      case 2:
        constants[2].second+=1;
        break;
      case 3:
        constants[3].second+=1;
        break;
      case 4:
        constants[4].second+=1;
        break;
      case 5:
        constants[5].second+=1;
        break;
      case 6:
        constants[6].second+=1;
        break;
      case 7:
        constants[7].second+=1;
        break;
      case 8:
        constants[8].second+=1;
        break;
      default:
        oled.print("FAILURE");
    }
  }
  if(left.pressed()){
    switch(place){
      case 0:
        constants[0].second-=1;
        break;
      case 1:
        constants[1].second-=1;
        break;
      case 2:
        constants[2].second-=1;
        break;
      case 3:
        constants[3].second-=1;
        break;
      case 4:
        constants[4].second-=1;
        break;
      case 5:
        constants[5].second-=1;
        break;
      case 6:
        constants[6].second-=1;
        break;
      case 7:
        constants[7].second-=1;
        break;
      case 8:
        constants[8].second-=1;
        break;
      default:
        oled.print("FAILURE");
    }
  }
  
};
void Oled::power(){
  if(center.pressed()){
    Serial.print("center");
    if(onstate==true){
      oled.ssd1306_command(SSD1306_DISPLAYOFF);
      onstate=false;
      delay(5000);
    }
    else{
      oled.ssd1306_command(SSD1306_DISPLAYON);
      onstate=true;
      oled.clearDisplay();
      oled.display();
    }
  }

};

void Oled::clear(){
  oled.clearDisplay();
};










