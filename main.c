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
    LedClear();
//        LedSet();
    GoodMorning();
    LedClear();

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
