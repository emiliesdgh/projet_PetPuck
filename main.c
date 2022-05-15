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
//for the  panic mode : gyroscope + accéléromètre
//#include <angles.h>
#include <sensors/imu.h>
#include <msgbus/messagebus.h>
#include <i2c_bus.h>
#include <selector.h>

//to use the threads and functions so that the robot can play sounds and melodies
#include <audio/audio_thread.h>
#include <audio/microphone.h>
#include <audio/play_melody.h>
#include <audio/play_sound_file.h>	//to play specific sounds from the SD card

//include our files
#include <audio_processing.h>
#include <puck_led.h>
#include <process_image.h>
#include <control.h>


//include the file .h for the main
#include <main.h>
#include <puck_movement.h>	//--->>> to merge with danse_mode and proximity_sensors maybe

static msg_t obstacleEncounterThd_resume_test;
static int8_t selector_flag_GN = 0;
static int8_t selector_flag_GM = 0;

//
//void SendUint8ToComputer(uint8_t* data, uint16_t size)
//{
//	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
//	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
//	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
//}

void set_selector_flag_GN(int8_t value){
	selector_flag_GN = value;
}

void set_selector_flag_GM(int8_t value){
	selector_flag_GM = value;
}

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
//TO BE INCLUDES IN MAIN.C ????
//messagebus_t bus;
//MUTEX_DECL(bus_lock);
//CONDVAR_DECL(bus_condvar);

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
////}
//static THD_WORKING_AREA(selector_thd_wa, 2048);
//
//static THD_FUNCTION(selector_thd, arg)
//{
//    (void) arg;
//    chRegSetThreadName(__FUNCTION__);
//
////    uint8_t stop_loop = 0;
//    systime_t time;
//
//    while(1) {
//       	time = chVTGetSystemTime();
//
//   		switch(get_selector()) {
//   			case 0:
//   				palSetPad(GPIOB, GPIOB_LED_BODY);
//
////   				obstacleEncounterThd_resume_test = ObstacleEncounter_suspend();
//   				GoodNight_LED();
//
//   				break;
////   			case 1:
////   				chThdSleepMilliseconds(1000);
////   				break;
////   			case 2:
////				chThdSleepMilliseconds(1000);
////				break;
////   			case 3:
////				chThdSleepMilliseconds(1000);
////				break;
////   			case 4:
////				chThdSleepMilliseconds(1000);
////				break;
////   			case 5:
////				chThdSleepMilliseconds(1000);
////				break;
////   			case 6:
////				chThdSleepMilliseconds(1000);
////				break;
////   			case 7:
////				chThdSleepMilliseconds(1000);
////				break;
//   			case 8:
//
////   				startAll();
//
//   				GoodMorning_LED();
//
////   				palClearPad(GPIOB, GPIOB_LED_BODY);
////   				ObstacleEncounter_resume(obstacleEncounterThd_resume_test);
//   				break;
////   			case 9:
////				chThdSleepMilliseconds(1000);
////				break;
////			case 10:
////				chThdSleepMilliseconds(1000);
////				break;
////			case 12:
////				chThdSleepMilliseconds(1000);
////				break;
////			case 13:
////				chThdSleepMilliseconds(1000);
////				break;
////			case 14:
////				chThdSleepMilliseconds(1000);
////				break;
////			case 15:
////				chThdSleepMilliseconds(1000);
////				break;
//			default :
//   				chThdSleepMilliseconds(1000);
//   				break;
//
//   	    }
//   		//fréquence de 100Hz
//   		chThdSleepUntilWindowed(time, time + MS2ST(10));
//   	}
//}
//void stopAll(void){
//	obstacleEncounter_stop();
//}
//
//void startAll(void){
//
//	mic_start(&processAudioData);
//	Control_start();
//
//    //inits the I2C communication
//    i2c_start();
//
//	imu_start();
//	//start the image processing ??
//	process_image_start();
//	//start the mic audio processing  ?
//	ObstacleEncounter_start();	//initialization for the obstacle encounter thread
//	calibrate_gyro();
//	calibrate_acc();
////	PanicMode_start();
//
//}


int main(void)		//clear all leds at the beggining
{
//	static thread_reference_t trp = NULL;
//	static thread_t *Controlp;
    halInit();
    chSysInit();
    mpu_init();

    //start the serial communication
    serial_start();
    //start the USB communication
    usb_start();
    //start the camera
	po8030_start();
	dcmi_start();
    //start the audio
    dac_start();
    //start the RGB LEDs
	spi_comm_start();
	initial_proximity();		//initialization for the proximity thread
	//ATTENTION A L'ORDRE DES APPELS DE  CES FONCTIONS !!


	motors_init();				//initialization of the motors
//	startAll();
	mic_start(&processAudioData);
	Control_start();

    //inits the I2C communication
    i2c_start();

	imu_start();


	ObstacleEncounter_start();	//initialization for the obstacle encounter thread
	calibrate_gyro();
	calibrate_acc();
	PanicMode_start();

	playMelodyStart();			//initialization for the melody thread


//
//	chThdCreateStatic(selector_thd_wa, sizeof(selector_thd_wa), NORMALPRIO+2, selector_thd, NULL);

//	while (1) {
//
//		switch(get_selector()) {
//			case 0:
////				ObstacleEncounter_start();
//				break;
//			case 1:
////				ObstacleEncounter_stop();
//
//				break;
//			default :
//				break;
//		}
//
//	}



	//starts the calibration of the sensors


    //%%%%%%%%%%%%%%%%%%%%%%%%%
//
//	PanicMode_start();


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
//	GoodMorning();
//	chThdSleepMilliseconds(1000);
//
//	GoodNight();
//	chThdSleepMilliseconds(1000);
//
//	Led_panic_mode();
//	chThdSleepMilliseconds(1000);

//	Led_uhOh();


//	while(1){
////		dancing_puck();
////		danseMode(speed_main);
////		test_main_panic();
////		dancing_puck();
////		Led_dance_mode();
//
//	}

	   /* Infinite loop. */
	    while (1) {
	    	switch(get_selector()) {
	    	case 0:
//	    		chThdSleepMilliseconds(1000);
	    		chprintf((BaseSequentialStream *)&SDU1, "just in case 0 =  %d\n", get_selector());
	    		set_selector_flag_GN(1);
	    		chprintf((BaseSequentialStream *)&SDU1, "before while selector =  %d\n", get_selector());
	    		palSetPad(GPIOB, GPIOB_LED_BODY);
	    		while(selector_flag_GN==1  || get_selector()==0){
//	    			palSetPad(GPIOB, GPIOB_LED_BODY);
	    			GoodNight_LED();

	    			chprintf((BaseSequentialStream *)&SDU1, "in while selector =  %d\n", get_selector());
	    		}
	    		chprintf((BaseSequentialStream *)&SDU1, "out while ");
//	    		do{
////	    			chThdSleepMilliseconds(1000);
//	    			GoodNight_LED();
//	    		}while(selector_flag_GN==0);
//	    		palSetPad(GPIOB, GPIOB_LED_BODY);
	    		break;
//	    	case 1:
////	    		chThdSleepMilliseconds(1000);
////	    		selector_flag_GN++;
////				palSetPad(GPIOB, GPIOB_LED_BODY);
////				while(selector_flag_GN==0){
//////	    			palSetPad(GPIOB, GPIOB_LED_BODY);
////					GoodNight_LED();
////				}
////	    		do{
//////	    			chThdSleepMilliseconds(1000);
////	    			GoodNight_LED();
////	    		}while(selector_flag_GN==0);
//	    		palClearPad(GPIOB, GPIOB_LED_BODY);
//				break;
	    	case 8:
//	    		selector_flag_GM++;
	    		if(selector_flag_GM==0){
	    			GoodMorning_LED();
	    			selector_flag_GM++;
	    		}
	    		break;
	    	default:
//	    		palClearPad(GPIOB, GPIOB_LED_BODY);
	    		set_selector_flag_GN(0);
	    		set_selector_flag_GM(0);
//	    		chThdSleepMilliseconds(1000);
	    		break;
	    	}
	    }


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
//    while (1) {
////    	//waits 1 second
//        chThdSleepMilliseconds(1000);
//////
//    }



}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
