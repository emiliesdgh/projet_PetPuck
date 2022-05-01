#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"


#include <main.h>

#include <spi_comm.h>	//pour utilier les leds rgb
#include "sensors/proximity.h"

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <gpio.h>
#include <puck_led.h>
#include <selector.h>
#include <timer.h>

#include <obstacle_encounter.h>
#include <proximity_sensor.h>


#include <leds.h>
#include <motors.h>


static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

//static void timer12_start(void){
//    //General Purpose Timer configuration
//    //timer 12 is a 16 bit timer so we can measure time
//    //to about 65ms with a 1Mhz counter
//    static const GPTConfig gpt12cfg = {
//        1000000,        /* 1MHz timer clock in order to measure uS.*/
//        NULL,           /* Timer callback.*/
//        0,
//        0
//    };
//
//    gptStart(&GPTD12, &gpt12cfg);
//    //let the timer count to max value
//    gptStartContinuous(&GPTD12, 0xFFFF);
//}

int main(void)
{


    halInit();
    chSysInit();
    mpu_init();

    serial_start();

	spi_comm_start();	//pour utilier les leds rgb

//
	// thread start
	ObstacleEncounter_start();	//encounter an obstacle okkkk pas dans panic

	motors_init();	//inits the motors

	initial_proximity();

//	proximity_start();	// vas  dans le mode panic mais jsp pkkkkkkk


//	proximityToStop_start();
//

    //starts timer 12
//    timer12_start();

//
    unsigned int a = 20;
//    unsigned int b = 5;
//    palSetPad(GPIOD, GPIOD_LED1);
    LedClear();
//	palSetPad(GPIOD, GPIOD_LED1);
//    GoodMorning();
//	set_led(2, 2);
//	LedBlink();

    do{

    	chThdSleepMilliseconds(1000);
      GoodMorning();
//    	CircleLed();
//    	palTogglePad(GPIOB, GPIOB_LED_BODY);



//    	toggle_red_led(LED6, LED_RGB_INTENSITY);

    	a = a - 1;

    }while(a!=0);

//    /* Infinite loop. */
//    while (1) {
//    	//waits 1 second
//        chThdSleepMilliseconds(1000);
////
//    }

}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
