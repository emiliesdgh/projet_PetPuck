#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <camera/po8030.h>
#include <chprintf.h>

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
#include <process_image.h>


#include <obstacle_encounter.h>
#include <proximity_sensor.h>
#include <danse_mode.h>


#include <leds.h>
#include <motors.h>

//uncomment to use python script and read microhpone data
//#define TESTING

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
    //start usb communication
    usb_start();
	po8030_start();
    //starts the camera
    dcmi_start();
    //
	spi_comm_start();	//pour utilier les leds rgb

//	process_image_start();
	mic_start(&processAudioData);

	#ifdef TESTING
    static float send_tab[MICSAMPLESIZE];
    while (1) { //trying to send the PCM data to the computer, need to edit python script?
    			//so far, copied from TP5 files--NOTE: edited audio_processing.c and .h too
    //waits until a result must be sent to the computer
    wait_send_to_computer();
    //we copy the buffer to avoid conflicts
    arm_copy_f32(get_audio_buffer_ptr(MIC_R_INPUT), send_tab, MICSAMPLESIZE);
    SendFloatToComputer((BaseSequentialStream *) &SD3, send_tab, MICSAMPLESIZE);
    }
	#endif //TESTING
//
	//threads start
	//%%%%%%%%%%%%%%%%%%%%%%%%%
//	ObstacleEncounter_start();	//initialisation for the obstacle encounter thread

	motors_init();	//inits the motors

//	initial_proximity();	//initialisation for the proximity thread
	//%%%%%%%%%%%%%%%%%%%%%%%%%
//

    //starts timer 12
//    timer12_start();

//
//    unsigned int a = 22;
//    unsigned int b = 5;
//    palSetPad(GPIOD, GPIOD_LED1);
//    LedClear();
//	palSetPad(GPIOD, GPIOD_LED1);
//
//	int16_t speed_main = 100;
//
//	 for(int i = 0; i<10; i++){
//		 if(i==8){
//			 speed_main=0;
//		 }
//		 danseMode(speed_main);
//	 }


//    palTogglePad(GPIOB, GPIOB_LED_BODY);
//	  set_led(2, 2);
//	  LedBlink();
//    while (1) {
//    	GoodNight();
//    	chThdSleepMilliseconds(1000);
//    	palTogglePad(GPIOB, GPIOB_LED_BODY);

		//mic_start(&processAudioData);
//
//    }
//    do{
//
//        GoodMorning();
//
//        palTogglePad(GPIOB, GPIOB_LED_BODY);
//
//    	chThdSleepMilliseconds(1000);
//
//    	palTogglePad(GPIOB, GPIOB_LED_BODY);

//    	GoodNight();
//    	CircleLed();
//    	palTogglePad(GPIOB, GPIOB_LED_BODY);



//    	toggle_red_led(LED6, LED_RGB_INTENSITY);

//    	a = a - 1;

//    }while(a!=0);

//    /* Infinite loop. */
    while (1) {
//    	//waits 1 second
        chThdSleepMilliseconds(1000);
////
    }

}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
