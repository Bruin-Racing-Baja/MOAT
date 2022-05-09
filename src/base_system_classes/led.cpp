#include "Constant.h"
#include "Led.h"


#if MODELNUMBER == 21
void setup_led(){
    pinMode(led_1_pin, OUTPUT);
    pinMode(led_2_pin, OUTPUT);
    pinMode(led_3_pin, OUTPUT);
    pinMode(led_4_pin, OUTPUT);
}

void activate_one_led(int LED_num){
    clear_all_leds();
    activate_led(LED_num);
}

void set_led(int LED_num, bool state){
    switch (LED_num) {
        case 1:
            digitalWrite(led_1_pin, !state);
            break;
        case 2:
            digitalWrite(led_2_pin, !state);
            break;
        case 3:
            digitalWrite(led_3_pin, !state);
            break;
        case 4:
            digitalWrite(led_4_pin, !state);
            break;
    }
}

void clear_all_leds(){
  digitalWrite(led_1_pin, HIGH);
  digitalWrite(led_2_pin, HIGH);
  digitalWrite(led_3_pin, HIGH);
  digitalWrite(led_4_pin, HIGH);
}

void blink_in_series(){
    int timeout = 500;
    activate_one_led(1);
    delay(timeout);
    activate_one_led(2);
    delay(timeout);
    activate_one_led(3);
    delay(timeout);
    activate_one_led(4);
    delay(timeout);
    clear_all_leds();
}
#else
void setup_led(){
}

void activate_one_led(int LED_num){
}

void clear_all_leds(){
}

void blink_in_series(){
}
#endif