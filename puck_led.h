#ifndef PUCK_LED_H
#define PUCK_LED_H

#include "main.h"

/* functions */
//void SetLed(unsigned int led_number, unsigned int value); // set led_number (0-7) to value (0=off 1=on higher=inverse)
void LedClear(void);
void LedSet(void);
void GoodMorning(void);

//void SetBodyLed(unsigned int value); // value (0=off 1=on higher=inverse)
//void SetFrontLed(unsigned int value); //value (0=off 1=on higher=inverse)

#endif
