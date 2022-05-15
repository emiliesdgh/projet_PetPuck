#include <leds.h>
#include <main.h>
#include <puck_led.h>

/*	function to initialise the LED, put them all in state OFF	*/
void LedClear(void){
	clear_leds();
	palClearPad(GPIOD, GPIOD_LED_FRONT);
	palClearPad(GPIOB, GPIOB_LED_BODY);
}
/*	function to be called when thread Good Morning detects that it's morning	*/
void GoodMorning_LED(void){
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
/*	function to be called when thread Good Night detects that it's night	*/
void GoodNight_LED(void){
	LedClear();
	int led_intensity = LED_RGB_INTENSITY;
	int led_value =  1;

	for(int j=0 ; j<4; j++){
		set_led(j,1);
	}
	do{
		for(int i=0; i<4; i++){
			LedSet_intensity(i, led_value, led_intensity);
		}
		led_intensity = led_intensity - 1;
		if(led_intensity == 2){
			led_value = 0;
		}
		if(led_intensity == 80){
			set_led(0,0);
		}
		if(led_intensity == 60){
			set_led(1,0);
		}
		if(led_intensity == 40){
			set_led(2,0);
		}
		if(led_intensity == 20){
			set_led(3,0);
		}
		chThdSleepMilliseconds(30);

	}while(led_intensity !=  0);
}
/*	function to be called when thread panic detects panic mode
 * 	all LEDs blinking and the RDG in every color	*/
void PanicMode_LED(void){
	LedClear();
	for(int i=0; i<4; i++){
		set_led(i, 1);
	}
	chThdSleepMilliseconds(80);
	for(int i=0; i<4; i++){
		set_led(i, 0);
	}
	chThdSleepMilliseconds(80);
	palTogglePad(GPIOB, GPIOB_LED_BODY);
	chThdSleepMilliseconds(80);
	palTogglePad(GPIOB, GPIOB_LED_BODY);
	chThdSleepMilliseconds(80);

	for(int i=0; i<4; i++){
		set_led(i, 1);
	}
	chThdSleepMilliseconds(80);
	for(int i=0; i<4; i++){
		set_led(i, 0);
	}
	palTogglePad(GPIOD, GPIOD_LED_FRONT);
	chThdSleepMilliseconds(80);
	palTogglePad(GPIOD, GPIOD_LED_FRONT);
	chThdSleepMilliseconds(80);

	for(int j=0; j<3; j++){
		for(int i=0; i<4; i++){
			toggle_rgb_led(i, j, LED_RGB_INTENSITY);
		}
		chThdSleepMilliseconds(80);
		for(int i=0; i<4; i++){
			toggle_rgb_led(i, j, LED_RGB_INTENSITY);
		}
		chThdSleepMilliseconds(80);
		for(int i=0; i<4; i++){
			set_led(i, 1);
		}
		chThdSleepMilliseconds(80);
		for(int i=0; i<4; i++){
			set_led(i, 0);
		}
	}
}
/*	function that set all LEDs to  the value chosen (value = 0 : OFF, value = 1 : ON)
 *	and puts the RGB LEDs on maximum intensity in red color only	*/
void LedSet_ALL(unsigned int led_number, unsigned int value){
	switch(led_number)
	{
		case 0:
			set_led(LED1, value);
			break;
		case 1:
			if(value >= 2){
				toggle_red_led(LED2, LED_RGB_INTENSITY);
			} else {
				set_rgb_led(LED2, value*LED_RGB_INTENSITY, 0, 0);
			}
			break;
		case 2:
			set_led(LED3, value);
			break;
		case 3:
			if(value >= 2){
				toggle_red_led(LED4, LED_RGB_INTENSITY);
			} else {
				set_rgb_led(LED4, value*LED_RGB_INTENSITY, 0, 0);
			}
			break;
		case 4:
			set_led(LED5, value);
			break;
		case 5:
			if(value >= 2){
				toggle_red_led(LED6, LED_RGB_INTENSITY);
			} else {
				set_rgb_led(LED6, value*LED_RGB_INTENSITY, 0, 0);
			}
			break;
		case 6:
			set_led(LED7, value);
			break;
		case 7:
			if(value >= 2){
				toggle_red_led(LED8, LED_RGB_INTENSITY);
			} else {
				set_rgb_led(LED8, value*LED_RGB_INTENSITY, 0, 0);
			}
			break;
		default:
			for(int i=0; i<8; i++){
				LedSet_ALL(i, value);
			}
			break;
	}
}
/*	function that sets the red color of RGB LEDs depending on the wanted intensity
 *	used for the Good Night mode	*/
void LedSet_intensity(unsigned int led_number, unsigned int value, int intensity){
	switch(led_number){
		case 0:
			if(value >= 2){
				toggle_red_led(LED2, intensity);
			} else {
				set_rgb_led(LED2, value*intensity, 0, 0);
			}
			break;
		case 1:
			if(value >= 2){
				toggle_red_led(LED4, intensity);
			} else {
				set_rgb_led(LED4, value*intensity, 0, 0);
			}
			break;
		case 2:
			if(value >= 2){
				toggle_red_led(LED6, intensity);
			} else {
				set_rgb_led(LED6, value*intensity, 0, 0);
			}
			break;
		case 3:
			if(value >= 2){
				toggle_red_led(LED8, intensity);
			} else {
				set_rgb_led(LED8, value*intensity, 0, 0);
			}
			break;
		default:
			for(int i=0; i<8; i++){
				LedSet_ALL(i, value);
			}
			break;
	}
}
