#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <arm_math.h>


#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <motors.h>
//#include <camera/po8030.h>
//#include <chprintf.h>
//#include <sensors/proximity.h>
//#include <sensors/VL53L0X/VL53L0X.h>
//#include <selector.h>
//#include <spi_comm.h>
//#include <i2c_bus.h>
//#include <leds.h>



#include <main.h>

#include <spi_comm.h>	//pour utilier les leds rgb

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <gpio.h>
#include <puck_led.h>
#include <selector.h>
#include <timer.h>


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

//	start des threads
//  ObstacleEncounter_start();	//thread obstacle encounter
//  motors_init();	 //inits the motors

    unsigned int a = 20;

    LedClear();

    do{
    	GoodMorning();

//        for(int i=0; i<8; i++){
//
//        	LedSet_ALL(i, 3);
//        	chThdSleepMilliseconds(1000);
//        }
    	chThdSleepMilliseconds(1000);
//        GoodMorning();
//    	CircleLed();
    	palTogglePad(GPIOB, GPIOB_LED_BODY);



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
