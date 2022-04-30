#include "ch.h"
#include "hal.h"
#include "main.h"
#include <puck_led.h>
#include <leds.h>
#include <spi_comm.h>
//#include "gpio.h"

static uint8_t tab_rgbLED[NUM_RGB_LED][RED_LED];

void initial_rgb_led(rgb_led_name_t led_number, uint8_t red_val){

	tab_rgbLED[led_number][RED_LED] = red_val;
}
void LedClear(void){

	clear_leds();

	palClearPad(GPIOD, GPIOD_LED_FRONT);
	palClearPad(GPIOB, GPIOB_LED_BODY);

}


void GoodMorning(void){	//all this is done in function CicleLed

	LedClear();

	static unsigned char no_led = 0;
	if(no_led == 0)
	{
		LedSet_ALL(7, 0);
		LedSet_ALL(no_led, 1);
		no_led++;
	}
	else if(no_led == 7)
	{
		LedSet_ALL(no_led-1, 0);
		LedSet_ALL(no_led, 1);
		no_led = 0;
	}
	else
	{
		LedSet_ALL(no_led-1,0);
		LedSet_ALL(no_led, 1);
		no_led++;
	}

}

void LedBlink(void)
{
	set_led(4, 2);
	// Send command to esp32 to toggle the state of all leds...comando toggle dalla parte esp32?
}

/*! \brief turn on/off the specified LED
 *
 * The e-puck has 8 red LEDs. With this function, you can
 * change the state of these LEDs.
 * \param led_number between 0 and 7
 * \param value 0 (off), 1 (on) otherwise change the state
 * \warning if led_number is other than 0-7, all leds are set
 * to the indicated value.
 */
void LedSet_ALL(unsigned int led_number, unsigned int value)
{
	switch(led_number)
	{
		case 0:
			set_led(LED1, value);
			break;
		case 1: // Change only the red led of the RGB to have the same color as other "normal" leds.
			if(value >= 2) {
				toggle_red_led(LED2, LED_RGB_INTENSITY);
//				palTogglePad(GPIOB, GPIOB_LED_BODY);
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
