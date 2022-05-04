#ifndef PUCK_LED_H
#define PUCK_LED_H

#include "main.h"
#include <leds.h>

/*
 * RGB intensity (percentage).
 */
#define LED_RGB_INTENSITY 100

void initial_rgb_led(rgb_led_name_t led_number, uint8_t red_val);

/* functions */
void LedClear(void);

void GoodMorning(void);
void GoodNight(void);
void Led_panic_mode(void);
//void Led_dance_mode(void);

void LedSet_ALL(unsigned int led_number, unsigned int value);
void LedSet_intensity(unsigned int led_number, unsigned int value, int intensity);

#endif
