#ifndef led_h
#define led_h
    
void setup_led();

void clear_all_leds();

void blink_in_series();

void set_led(int LED_num, bool state);

#endif