#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>
#include <chbsem.h>
#include <hal.h>
#include "selector.h"


#include <main.h>
#include <camera/po8030.h>
#include <camera/dcmi_camera.h>

#include "sensors/VL53L0X/VL53L0X.h"
#include "sensors/proximity.h"
#include <leds.h>


#include <process_image.h>
#include <puck_led.h>
#include "selector.h"

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


//to use the threads and functions so that the robot can play sounds and melodies
#include <audio/audio_thread.h>
#include <audio/microphone.h>
#include <audio/play_melody.h>
#include <audio/play_sound_file.h>	//to play specific sounds from the SD card

//include our files
#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <puck_led.h>
#include <process_image.h>
#include <control.h>

#include <panic_mode.h>

//include the file .h for the main
#include <main.h>
#include <puck_movement.h>	//--->>> to merge with danse_mode and proximity_sensors maybe




static float distance_cm = 0;
static uint16_t line_position = IMAGE_BUFFER_SIZE/2;	//middle

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);
MUTEX_DECL(image_buffer_mutex);

static BSEMAPHORE_DECL(morning_sem, TRUE);
static BSEMAPHORE_DECL(night_sem, FALSE);

//***TEST
/* internal variables for prox */
//int ambient_ir[8];				 // ambient light measurement
//void test_lecture_(void){
//
////	for(int i=0; i<8;i++){
////
////		int am_light = get_ambient_light(i);
////		chprintf((BaseSequentialStream *)&SDU1, "ambient light dans le for = %d\n", am_light);
////
////	}
//
//	int ir_1 = 0;
//
//	int am_light = 0;
//	int prox_light=0;
//
//	int final_try = 0;
//
//	am_light = get_ambient_light(ir_1);
//	prox_light = get_prox(ir_1);
//
//	final_try = am_light -prox_light;
//	if(am_light<1000){
//
//		palSetPad(GPIOB, GPIOB_LED_BODY);
//		palClearPad(GPIOD, GPIOD_LED_FRONT);
//
//	}else if(am_light<1000){
//
//		palClearPad(GPIOB, GPIOB_LED_BODY);
//
//	}else{
//
//		palSetPad(GPIOD, GPIOD_LED_FRONT);
//	}
////	chprintf((BaseSequentialStream *)&SDU1, "ambient light dans le for = %d\n", am_light);
////	chprintf((BaseSequentialStream *)&SDU1, "prox light dans le for = %d\n", prox_light);
//
//
//}


//gets ambient light average and compares it to DUSKDAWN
void ambient_light(uint8_t *buffer1,uint8_t *buffer2, uint8_t *buffer3) {

	static uint8_t newday = 1;

//	chprintf((BaseSequentialStream *)&SDU1, "im in here\n");

	uint32_t mean1 = 0;
	uint32_t mean2 = 0;
	uint32_t mean3 = 0;

	for(uint16_t i = 0 ; i < IMAGE_BUFFER_SIZE ; i++) {
			mean1 += buffer1[i];
			mean2 += buffer2[i];
			mean3 += buffer3[i];
		}

		mean1 /= IMAGE_BUFFER_SIZE;
		mean2 /= IMAGE_BUFFER_SIZE;
		mean3 /= IMAGE_BUFFER_SIZE;

	if (mean1 >= DUSK && mean2 >= DUSK && mean3 >= DUSK && newday) {
		//call sayGM function
//		chprintf((BaseSequentialStream *)&SDU1, "i wanna say GM %d\n");
		newday = 0;
		chBSemSignal(&morning_sem);
//		GoodMorning_LED();

	} else if (mean1 < DAWN && mean2 < DAWN && mean3 < DAWN && !(newday)) {
		//call sayGN function
		newday = 1;
		chBSemSignal(&night_sem);
//		GoodNight_LED();
	}
}


static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

//    static uint8_t to_be_processed = 0;

//	/* Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 10 + 11 (minimum 2 lines because reasons)
//	 * (reasons being how camera's library is written) */
//	po8030_advanced_config(FORMAT_RGB565, 0, 10, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
//
//	dcmi_enable_double_buffering();
//	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
//	dcmi_prepare();

    while(1) {

    	get_dcmi_capture();
//       //starts a capture
//		dcmi_capture_start();
//		//waits for the capture to be done
//		wait_image_ready();
		//signals an image has been captured
//		chBSemSignal(&image_ready_sem);
    	chBSemSignal(&image_ready_sem);
//		to_be_processed++;
//		if (to_be_processed > 4) {
//		//signals an image has been captured
//			chBSemSignal(&image_ready_sem);
//			to_be_processed = 0;
//		}
		chThdSleepSeconds(2);
    }
}

static THD_WORKING_AREA(waPolitePuck, 256);
static THD_FUNCTION(PolitePuck, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1) {
    	chBSemWait(&morning_sem);
    	GoodMorning_LED();
    	chBSemWait(&night_sem);
    	GoodNight_LED();
//    	chThdSleepSeconds(5);
    }

}


static THD_WORKING_AREA(waProcessImage, 2048);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
//    systime_t time;

	uint8_t *img_buff_ptr;
	uint8_t imagered[IMAGE_BUFFER_SIZE] = {0};
	uint8_t imagegreen[IMAGE_BUFFER_SIZE] = {0};
	uint8_t imageblue[IMAGE_BUFFER_SIZE] = {0};
	//uint16_t lineWidth = 0;

	//bool send_to_computer = true;

    while(1) {

    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);

//		chprintf((BaseSequentialStream *)&SDU1, "image ready %d\n");
//      time = chVTGetSystemTime();
//		//gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

//        extract_pixels_RGB (imagered, imagegreen, imageblue, &img_buff_ptr);

		//Extracts only the red pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2) {
			//extracts first 5bits of the first byte
			//takes nothing from the second byte
			imagered[i/2] = (uint8_t)((img_buff_ptr[i]&0xF8)>>2);
			imagegreen[i/2] = (uint8_t)((img_buff_ptr[i]&0x07)<<3)+(uint8_t)((img_buff_ptr[i+1] & 0xE0)>>5);				// red value scaled to green size
			imageblue[i/2] = (uint8_t)((img_buff_ptr[i+1]&0x1F)<<1);			// blue value scaled to green size
		}

//		extract_pixels_R(imagered, img_buff_ptr);
//		extract_pixels_G(imagegreen, img_buff_ptr);
//		extract_pixels_B(imageblue, img_buff_ptr);

		ambient_light(imagered, imagegreen, imageblue);

//		test_lecture_();
		//search for a line in the image and gets its width in pixels
		//lineWidth = extract_line_width(image);
//		chThdSleepSeconds(5);
//		chThdSleepSeconds(5);
    }
}

void extract_pixels_RGB (uint8_t *red_buffer, uint8_t *green_buffer, uint8_t *blue_buffer, uint8_t** image_buffer_ptr) {

//	chMtxLock(&image_buffer_mutex);
	*image_buffer_ptr = dcmi_get_last_image_ptr();
//	chMtxUnlockAll();//&image_buffer_mutex);

//	get_ptr_to_img_array(image_buffer_ptr);
	extract_pixels_R(red_buffer,*image_buffer_ptr);
	extract_pixels_G(green_buffer,*image_buffer_ptr);
	extract_pixels_B(blue_buffer,*image_buffer_ptr);

}

//void get_ptr_to_img_array(uint8_t *image_buffer_ptr) {
//	image_buffer_ptr = dcmi_get_last_image_ptr();
//}

void extract_pixels_R (uint8_t *red_buffer, uint8_t *image_buffer_ptr) {
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2) {
		red_buffer[i/2] = (uint8_t)((image_buffer_ptr[i]&0xF8)>>2);
	}
}

void extract_pixels_G (uint8_t *green_buffer, uint8_t *image_buffer_ptr) {
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2) {
		green_buffer[i/2] = (uint8_t)((image_buffer_ptr[i]&0x07)<<3)+(uint8_t)((image_buffer_ptr[i+1] & 0xE0)>>5);
	}
}

void extract_pixels_B (uint8_t *blue_buffer, uint8_t *image_buffer_ptr) {
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2) {
		blue_buffer[i/2] = (uint8_t)((image_buffer_ptr[i+1]&0x1F)<<1);
	}
}

float get_distance_cm(void){
	return distance_cm;
}

void get_dcmi_capture (void) {
	chMtxLock(&image_buffer_mutex);
	dcmi_capture_start();
	//waits for the capture to be done
	wait_image_ready();
	chMtxUnlockAll();
}

uint16_t get_line_position(void){
	return line_position;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO+5, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO+5, CaptureImage, NULL);
}

void polite_puck_start(void) {
	GoodMorning_LED();
	chThdCreateStatic(waPolitePuck, sizeof(waPolitePuck), NORMALPRIO+4, PolitePuck, NULL);
}

//void polite_puck_start(void){
////	Good_Morning();
//	chThdCreateStatic(waPolitePuck, sizeof(waProcessImage), NORMALPRIO, PolitePuck, NULL);
//}

//void Good(bool day){
//	if(day==DAY){
////		playMelody(EXTERNAL_SONG, ML_FORCE_CHANGE,'GoodMorning.wav' );
////		waitMelodyHasFinished();
//	} else {
////		playMelody(EXTERNAL_SONG, ML_FORCE_CHANGE,'GoodNight.wav' );
////		waitMelodyHasFinished();
//		all_leds_off()
//	}
//}

//static THD_WORKING_AREA(waPolitePuck, 256);
//static THD_FUNCTION(PolitePuck, arg) {
//    chBSemWait(&Day);
//    Good(MORNING);
//    chBSemWait(&Night);
//    Good(NIGHT);
//}

//static THD_WORKING_AREA(selector_thd_wa, 2048);
//static THD_FUNCTION(selector_thd, arg)
//{
//    (void) arg;
//    chRegSetThreadName(__FUNCTION__);
//
//    uint8_t stop_loop = 0;
//    static uint8_t newday = 1;
//
//    while(stop_loop == 0) {
//
//		switch(get_selector()) {
//			case 0: // Aseba.
//				newday = 0;
//				if (newday) {
//					GoodMorning_LED();
//				}
//				stop_loop = 1;
//				break;
//			case 8:
//				//goes to sleep
//				newday = 1;
//				if (!newday) {
//					GoodNight_LED();
//				}
//				//inits the motors
//				motors_init();
//				//inits mic
//				mic_start(&processAudioData);
//				//inits control
//				Control_start();
//			    //inits the I2C communication
//			    i2c_start();
//			    //inits imu
//				imu_start();
//
//		}
//    }
//}
//
//void create_static_thd_selector(void) {
//	chThdCreateStatic(selector_thd_wa, sizeof(selector_thd_wa), NORMALPRIO, selector_thd, NULL);
//}
