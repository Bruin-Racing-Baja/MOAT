#include "Constant.h"
#include "Led.h"


#if MODELNUMBER == 21
void setup_led(){
    pinMode(led_1_pin, OUTPUT);
    pinMode(led_2_pin, OUTPUT);
    pinMode(led_3_pin, OUTPUT);
    pinMode(led_4_pin, OUTPUT);
}

void clear_one_led(int LED_num){
    switch (LED_num) {
        case 1:
            digitalWrite(led_1_pin, HIGH);
            break;
        case 2:
            digitalWrite(led_2_pin, HIGH);
            break;
        case 3:
            digitalWrite(led_3_pin, HIGH);
            break;
        case 4:
            digitalWrite(led_4_pin, HIGH);
            break;
    }
}

void activate_one_led(int LED_num){
    clear_all_leds();
    set_led(LED_num, 1);
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
    set_led(1, 1);
    delay(timeout);
    set_led(2, 1);
    delay(timeout);
    set_led(3, 1);
    delay(timeout);
    set_led(4, 1);
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