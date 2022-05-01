#include "ch.h"
#include "hal.h"
#include "main.h"
#include <puck_led.h>
#include <leds.h>
#include <spi_comm.h>

void LedClear(void){

	clear_leds();

	palClearPad(GPIOD, GPIOD_LED_FRONT);
	palClearPad(GPIOB, GPIOB_LED_BODY);
}

//function to be called when thread GoodMorning detects that it's morning
void GoodMorning(void){

	LedClear();

	for(int i=0; i<8; i++){

		LedSet_ALL(i,1);

		chThdSleepMilliseconds(100);
	}

	for(int i=0; i<8; i++){

		LedSet_ALL(i,0);

		chThdSleepMilliseconds(100);
	}
	for(int i=0; i<8; i++){

		LedSet_ALL(i,1);
	}
	palTogglePad(GPIOB, GPIOB_LED_BODY);
	chThdSleepMilliseconds(1000);
	for(int i=0; i<8; i++){

			LedSet_ALL(i,0);
	}
	palTogglePad(GPIOB, GPIOB_LED_BODY);
	chThdSleepMilliseconds(1000);
}

//function to be called when thread GoodNight detects that it's night
void GoodNight(void){

	LedClear();

	int led_intensity = LED_RGB_INTENSITY;
	int led_value =  1;

	do{

		for(int i=0; i<8; i++){

			LedSet_intensity(i, led_value, led_intensity);
		}

		led_intensity = led_intensity - 5;

		if(led_intensity == 5){

			led_value = 0;
		}
    	chThdSleepMilliseconds(500);

	}while(led_intensity !=  0);

}

void Led_panic_mode(void){

	for(int i=0; i<4; i++){
		set_led(i, 1);
	}
	chThdSleepMilliseconds(100);
	for(int i=0; i<4; i++){
		set_led(i, 0);
	}
	chThdSleepMilliseconds(100);

	palTogglePad(GPIOB, GPIOB_LED_BODY);
	chThdSleepMilliseconds(100);

	palTogglePad(GPIOB, GPIOB_LED_BODY);
	chThdSleepMilliseconds(100);

	for(int i=0; i<4; i++){
		set_led(i, 1);
	}
	chThdSleepMilliseconds(100);
	for(int i=0; i<4; i++){
		set_led(i, 0);
	}

	palTogglePad(GPIOD, GPIOD_LED_FRONT);
	chThdSleepMilliseconds(100);

	palTogglePad(GPIOD, GPIOD_LED_FRONT);
	chThdSleepMilliseconds(100);

	for(int j=0; j<3; j++){

		for(int i=0; i<4; i++){
			toggle_rgb_led(i, j, LED_RGB_INTENSITY);
		}
		chThdSleepMilliseconds(100);

		for(int i=0; i<4; i++){
			toggle_rgb_led(i, j, LED_RGB_INTENSITY);
		}
		chThdSleepMilliseconds(100);
		for(int i=0; i<4; i++){
			set_led(i, 1);
		}
		chThdSleepMilliseconds(100);
		for(int i=0; i<4; i++){
			set_led(i, 0);
		}
	}
}

void LedSet_ALL(unsigned int led_number, unsigned int value){

	switch(led_number)
	{
		case 0:
			set_led(LED1, value);
			break;
		case 1: // Change only the red led of the RGB to have the same color as other "normal" leds.
			if(value >= 2) {
				toggle_red_led(LED2, LED_RGB_INTENSITY);
			} else {
				set_rgb_led(LED2, value*LED_RGB_INTENSITY, 0, 0);
			}
			break;
		case 2:
			set_led(LED3, value);
			break;
		case 3: // Change only the red led of the RGB to have the same color as other "normal" leds.
			if(value >= 2) {
				toggle_red_led(LED4, LED_RGB_INTENSITY);
			} else {
				set_rgb_led(LED4, value*LED_RGB_INTENSITY, 0, 0);
			}
			break;
		case 4:
			set_led(LED5, value);
			break;
		case 5: // Change only the red led of the RGB to have the same color as other "normal" leds.
			if(value >= 2) {
				toggle_red_led(LED6, LED_RGB_INTENSITY);
			} else {
				set_rgb_led(LED6, value*LED_RGB_INTENSITY, 0, 0);
			}
			break;
		case 6:
			set_led(LED7, value);
			break;
		case 7: // Change only the red led of the RGB to have the same color as other "normal" leds.
			if(value >= 2) {
				toggle_red_led(LED8, LED_RGB_INTENSITY);
			} else {
				set_rgb_led(LED8, value*LED_RGB_INTENSITY, 0, 0);
			}
			break;
		default:
			for(int i=0; i<8; i++) {
				LedSet_ALL(i, value);
			}
			break;
	}
}

void LedSet_intensity(unsigned int led_number, unsigned int value, int intensity){

	switch(led_number)
	{
		case 0:
			set_led(LED1, value);
			break;
		case 1: // Change only the red led of the RGB to have the same color as other "normal" leds.
			if(value >= 2) {
				toggle_red_led(LED2, intensity);
			} else {
				set_rgb_led(LED2, value*intensity, 0, 0);
			}
			break;
		case 2:
			set_led(LED3, value);
			break;
		case 3: // Change only the red led of the RGB to have the same color as other "normal" leds.
			if(value >= 2) {
				toggle_red_led(LED4, intensity);
			} else {
				set_rgb_led(LED4, value*intensity, 0, 0);
			}
			break;
		case 4:
			set_led(LED5, value);
			break;
		case 5: // Change only the red led of the RGB to have the same color as other "normal" leds.
			if(value >= 2) {
				toggle_red_led(LED6, intensity);
			} else {
				set_rgb_led(LED6, value*intensity, 0, 0);
			}
			break;
		case 6:
			set_led(LED7, value);
			break;
		case 7: // Change only the red led of the RGB to have the same color as other "normal" leds.
			if(value >= 2) {
				toggle_red_led(LED8, intensity);
			} else {
				set_rgb_led(LED8, value*intensity, 0, 0);
			}
			break;
		default:
			for(int i=0; i<8; i++) {
				LedSet_ALL(i, value);
			}
			break;
	}
}





