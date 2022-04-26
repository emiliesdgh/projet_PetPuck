#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <gpio.h>
#include <puck_led.h>
#include <selector.h>
#include <timer.h>


int main(void)
{

    halInit();
    chSysInit();
    mpu_init();
 //from tp4:
    //start the USB communication
    usb_start();
    //starts the camera
    dcmi_start();
	po8030_start();
	//inits the motors
	motors_init();

	//stars the thread for the processing of the image
	process_image_start();
//end of from TP4

//    LedClear();
//    LedSet();
//    GoodMorning();
//    LedClear();

//    blahblahbnejhrbsdcf,thncg

    /* Infinite loop. */
    while (1) {
    	//waits 1 second
        chThdSleepMilliseconds(1000);

    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
