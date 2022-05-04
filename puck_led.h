#ifndef PUCK_LED_H
#define PUCK_LED_H

#include "main.h"
#include <leds.h>

//max intensity for the RGB LEDs
#define LED_RGB_INTENSITY 100


/* functions */
void LedClear(void);

void GoodMorning(void);
void GoodNight(void);
void Led_panic_mode(void);
//void Led_dance_mode(void);

void LedSet_ALL(unsigned int led_number, unsigned int value);
void LedSet_intensity(unsigned int led_number, unsigned int value, int intensity);

#endif
