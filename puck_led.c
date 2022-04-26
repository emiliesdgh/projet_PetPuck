//#include "main.h"
#include <puck_led.h>
#include <leds.h>
//#include "gpio.h"
//
void LedClear(void){

	palClearPad(GPIOD, GPIOD_LED1);
	palClearPad(GPIOD, GPIOD_LED3);
	palClearPad(GPIOD, GPIOD_LED5);
    palClearPad(GPIOD, GPIOD_LED7);
    palClearPad(GPIOD, GPIOD_LED_FRONT);
    palClearPad(GPIOB, GPIOB_LED_BODY);

    palClearPad(GPIOD, LED2);
}

void LedSet(void){

	palSetPad(GPIOD, GPIOD_LED1);
	palSetPad(GPIOD, GPIOD_LED3);
	palSetPad(GPIOD, GPIOD_LED5);
    palSetPad(GPIOD, GPIOD_LED7);
    palSetPad(GPIOD, GPIOD_LED_FRONT);
    palSetPad(GPIOB, GPIOB_LED_BODY);

    palSetPad(GPIOD, LED2);
}

void GoodMorning(void){

	palTogglePad(GPIOD, GPIOD_LED1);
	palTogglePad(GPIOD, GPIOD_LED3);
	palTogglePad(GPIOD, GPIOD_LED5);
    palTogglePad(GPIOD, GPIOD_LED7);
    palTogglePad(GPIOD, GPIOD_LED_FRONT);
    palTogglePad(GPIOB, GPIOB_LED_BODY);

    palTogglePad(GPIOD, LED2);

}
//void SetLed(unsigned int led_number, unsigned int value)
//// led_number between 0 and 7, value 0 (off) or 1 (on)
//// if led_number other than 0-7, all leds are set to value
//{
//	switch(led_number)
//	{
//		case 0:
//			{
//			if(value>1)
//				gpio_toggle(LED1);
//			if(value==0)
//				gpio_clear(LED1);
//			else
//				gpio_set(LED1);
//			break;
//			}
//		case 1:
//			{
//			if(value>1)
//				gpio_toggle(LED3);
//			if(value==0)
//				gpio_clear(LED3);
//			else
//				gpio_set(LED3);
//			break;
//			}
//		case 2:
//			{
//			if(value>1)
//				gpio_toggle(LED5);
//			if(value==0)
//				gpio_clear(LED5);
//			else
//				gpio_set(LED5);
//			break;
//			}
//		case 3:
//			{
//			if(value>1)
//				gpio_toggle(LED7);
//			if(value==0)
//				gpio_clear(LED7);
//			else
//				gpio_set(LED7);
//			break;
//			}
//
//		default:
//			if(value==0)
//				LedClear();
//			else
//				LedSet();
//	}
//}
//
//
//void SetBodyLed(unsigned int value)
//{
//	if(value>1)
//		gpio_toggle(BODY_LED);
//	if(value==0)
//		gpio_clear(BODY_LED);
//	else
//		gpio_set(BODY_LED);
//}
//
//void SetFrontLed(unsigned int value)
//{
//	if(value>1)
//		gpio_toggle(FRONT_LED);
//	if(value==0)
//		gpio_clear(FRONT_LED);
//	else
//		gpio_set(FRONT_LED);
//}
//
//void LedClear(void)
//{
//	gpio_clear(LED1);
//	gpio_clear(LED3);
//	gpio_clear(LED5);
//	gpio_clear(LED7);
//}
//
//void LedSet(void)
//{
//	gpio_set(LED1);
//	gpio_set(LED3);
//	gpio_set(LED5);
//	gpio_set(LED7);
//}
