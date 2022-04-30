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
//void SetLed(unsigned int led_number, unsigned int value); // set led_number (0-7) to value (0=off 1=on higher=inverse)
void LedClear(void);
void LedSet(void);
void GoodMorning(void);

//void GoodMorning_start(void);
//void GoodNight_start(void);

void LedBlink(void);
void LedSet_ALL(unsigned int led_number, unsigned int value);
void CircleLed(void);

#endif
