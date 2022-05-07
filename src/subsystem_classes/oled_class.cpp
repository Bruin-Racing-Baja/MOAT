#include <Oled.h>
#include <EduIntro.h>
#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
Button up(5);
Button down(9);
Button cen(7);
Button plus(8);
Button minu(6);

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);
std::map<String,int> constants;
std::map<String,int>::iterator it=constants.begin();
byte place=1;
bool onstate;

void Oled::init()
{
  // initialize OLED display with address 0x3C for 128x64
    oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    //oled.setTextSize(1);
    //oled.setTextColor(WHITE);  
    oled.clearDisplay();
    oled.println("TESTING INIT");
    oled.display();
    onstate=true;

    //oled.clearDisplay();
};
void Oled::write(std::map<String,int> ok){
    int cursor=10;
    oled.setCursor(cursor, 0);
    oled.println("Baja Electronics!");  
    cursor=cursor+10;       
    for(auto i = ok.cbegin(); i != ok.cend(); ++i){
      oled.setCursor(cursor,0);
      oled.println(i->first + " " + i->second + "\n");
      oled.setCursor(60,cursor);
      cursor=cursor+10;
    }

    oled.setCursor(2,(place*10)+10);
    oled.println(">");

    oled.display();  
}

void Oled::moveUpOrDown(){
  if(up.pressed()){
    if(place<=1){
      place=constants.size()-1;
    }
    else{
      place--;
    }
  }
  if(down.pressed()){
    if(place>=constants.size()-1){
      place=1;
    }
    else{
      place++;
    }
  }
};

void Oled::change(std::map<String,int> ok){
  if(plus.pressed()){
    switch(place){
      case 1:
        it=ok.find(1);
        if(it!=ok.end()){
          it->second+=1;
        }
        break;
      case 2:
        it=ok.find(2);
        if(it!=ok.end()){
          it->second+=1;
        }
        break;
      default:
        oled.print("FAILURE");
    }
  }
  if(minu.pressed()){
    switch(place){
      case 1:
        it=ok.find(1);
        if(it!=ok.end()){
          it->second-=1;
        }
        break;
      case 2:
        it=ok.find(2);
        if(it!=ok.end()){
          it->second-=1;
        }
        break;
      default:
        oled.print("FAILURE");
    }
  }
  
};
void Oled::power(){
  if(cen.pressed()){
    if(onstate==true){
      oled.dim(true);
    }
    else{
      oled.dim(false);
      oled.clearDisplay();
      oled.display();
    }
  }
  
};

void Oled::clear(){
  oled.clearDisplay();
};

std::map<String,int> Oled::createMap(){
    int a=1;
    int b=2;
    int c=3;
    constants.insert(it,std::pair<String,int>("Testing1",a));
    constants.insert(it,std::pair<String,int>("Testing2",b));
    constants.insert(it,std::pair<String,int>("Testing3",c));
    constants.insert(it,std::pair<String,int>("Name4",27));
    constants.insert(it,std::pair<String,int>("Name5",28));
    constants.insert(it,std::pair<String,int>("Name6",29));
    constants.insert(it,std::pair<String,int>("Name7",30));
    constants.insert(it,std::pair<String,int>("Name8",31));
    constants.insert(it,std::pair<String,int>("Name9",32));
    return constants;

};









