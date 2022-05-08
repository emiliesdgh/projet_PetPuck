// C standard header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ChibiOS headers
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <chprintf.h>

#include <camera/po8030.h>



//include  the files from the given library
#include <spi_comm.h>			//to be able to use the RGB LEDs
#include "sensors/proximity.h"	//to be  able to use the proximity threads

#include <leds.h>				//to use the different LED functions that exist already
#include <motors.h>				//to use the different motor functions that exist already

//to use the threads and functions so that the robot can play sounds and melodies
#include <audio/audio_thread.h>
#include <audio/play_melody.h>
#include <audio/play_sound_file.h>	//to play specific sounds from the SD card

//include our files
#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <gpio.h>		//on utilise pas il me semble
#include <puck_led.h>
#include <selector.h>	//on utilise pas il me semble
#include <timer.h>		//on utilise pas il me semble
#include <process_image.h>

#include <obstacle_encounter.h>	//--->>> to merge with danse_mode and proximity_sensors maybe
#include <proximity_sensor.h>
#include <danse_mode.h>

//include the file .h for the main
#include <main.h>


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

    //start the serial communication
    serial_start();
    //start the USB communication
    usb_start();
    //start the camera
//	po8030_start();
//  dcmi_start();
    //start the audio
    dac_start();
    //start the RGB LEDs
	spi_comm_start();
	//start the image processing ??
//	process_image_start();
	motors_init();				//initialization of the motors
	//start the mic audio processing  ?
	mic_start(&processAudioData);
//
	#ifdef TESTING
    static float send_tab[MICSAMPLESIZE];
    while (1) { //trying to send the PCM data to the computer, need to edit python script?
    			//so far, copied from TP5 files--NOTE: edited audio_processing.c and .h too
    //waits until a result must be sent to the computer
    wait_send_to_computer();
    //we copy the buffer to avoid conflicts
    arm_copy_f32(get_audio_buffer_ptr(MIC_L_INPUT), send_tab, MICSAMPLESIZE);
    SendFloatToComputer((BaseSequentialStream *) &SD3, send_tab, MICSAMPLESIZE);
    }
	#endif //TESTING
//
	//threads start
	//%%%%%%%%%%%%%%%%%%%%%%%%%
//	ObstacleEncounter_start();	//initialization for the obstacle encounter thread


//	initial_proximity();		//initialization for the proximity thread
	playMelodyStart();			//initialization for the melody thread
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
//
//	 for(int i = 0; i<10; i++){
//
//		 danseMode_sansArgument();
//	 }
//	    	GoodNight();

//	while(1){
//
////		danseMode(speed_main);
//
////		dancing_puck();
////		Led_dance_mode();
//
//	}


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
