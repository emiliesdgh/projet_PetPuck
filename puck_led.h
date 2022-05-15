#ifndef PUCK_LED_H
#define PUCK_LED_H

//defines
#define LED_RGB_INTENSITY 100  //max intensity for the RGB LEDs

#include <spi_comm.h>			//to be able to use the RGB LEDs
#include <chprintf.h>
#include <main.h>
#include <leds.h>
#include "ch.h"
#include "hal.h"
#include <usbcfg.h>

//functions
void LedClear(void);
void GoodMorning_LED(void);
void GoodNight_LED(void);
void PanicMode_LED(void);
void LedSet_ALL(unsigned int led_number, unsigned int value);
void LedSet_intensity(unsigned int led_number, unsigned int value, int intensity);

#endif
