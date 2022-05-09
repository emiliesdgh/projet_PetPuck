#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>
#include <chbsem.h>


#include <main.h>
#include <camera/po8030.h>
#include <camera/dcmi_camera.h>

#include "sensors/VL53L0X/VL53L0X.h"
#include "sensors/proximity.h"
#include <leds.h>


#include <process_image.h>
#include <puck_led.h>



static float distance_cm = 0;
static uint16_t line_position = IMAGE_BUFFER_SIZE/2;	//middle

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

//***TEST
/* internal variables for prox */
int ambient_ir[8];				 // ambient light measurement
void test_lecture_(void){

//	for(int i=0; i<8;i++){
//
//		int am_light = get_ambient_light(i);
//		chprintf((BaseSequentialStream *)&SDU1, "ambient light dans le for = %d\n", am_light);
//
//	}

	int ir_1 = 0;

	int am_light = 0;
	int prox_light=0;

	int final_try = 0;

	am_light = get_ambient_light(ir_1);
	prox_light = get_prox(ir_1);

	final_try = am_light -prox_light;
	if(am_light<1000){

		palSetPad(GPIOB, GPIOB_LED_BODY);
		palClearPad(GPIOD, GPIOD_LED_FRONT);

	}else if(am_light<1000){

		palClearPad(GPIOB, GPIOB_LED_BODY);

	}else{

		palSetPad(GPIOD, GPIOD_LED_FRONT);
	}
	chprintf((BaseSequentialStream *)&SDU1, "ambient light dans le for = %d\n", am_light);
	chprintf((BaseSequentialStream *)&SDU1, "prox light dans le for = %d\n", prox_light);


}


//gets ambient light average and compares it to DUSKDAWN
void ambient_light(uint8_t *buffer) {

	static uint8_t newday = 1;

	uint32_t mean = 0;

	for(uint16_t i = 0 ; i < IMAGE_BUFFER_SIZE ; i++) {
			mean += buffer[i];
		}

		mean /= IMAGE_BUFFER_SIZE;

	if (mean >= DUSK && newday) {
		//call sayGM function
		newday = 0;
		GoodMorning_LED();

	}else if (mean < DAWN && !(newday)) {
		//call sayGN function
		newday = 1;
		GoodNight_LED();
	}
}




static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	/* Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 10 + 11 (minimum 2 lines because reasons)
	 * (reasons being how camera's library is written) */
	po8030_advanced_config(FORMAT_RGB565, 0, 10, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);

	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);
    }
}


static THD_WORKING_AREA(waProcessImage, 1024);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	uint8_t *img_buff_ptr;
	uint8_t image[IMAGE_BUFFER_SIZE] = {0};
	//uint16_t lineWidth = 0;

	//bool send_to_computer = true;

    while(1){
    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565    
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts only the red pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
			//extracts first 5bits of the first byte
			//takes nothing from the second byte
			image[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;
		}
//		ambient_light(image);
//		test_lecture_();
		//search for a line in the image and gets its width in pixels
		//lineWidth = extract_line_width(image);
    }
}

float get_distance_cm(void){
	return distance_cm;
}

uint16_t get_line_position(void){
	return line_position;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}
