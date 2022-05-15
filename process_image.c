#include <process_image.h>

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);
static BSEMAPHORE_DECL(morning_sem, TRUE);
static BSEMAPHORE_DECL(night_sem, FALSE);

//mutex
MUTEX_DECL(image_buffer_mutex);

//gets ambient light average and compares it to DUSKDAWN
void ambient_light(uint8_t *buffer1,uint8_t *buffer2, uint8_t *buffer3) {

	static uint8_t newday = 1;

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
		newday = 0;
		chBSemSignal(&morning_sem);

	} else if (mean1 < DAWN && mean2 < DAWN && mean3 < DAWN && !(newday)) {
		//call sayGN function
		newday = 1;
		chBSemSignal(&night_sem);
	}
}


static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1) {

    	get_dcmi_capture();
    	chBSemSignal(&image_ready_sem);
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
    }

}

static THD_WORKING_AREA(waProcessImage, 2048);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	uint8_t *img_buff_ptr;
	uint8_t imagered[IMAGE_BUFFER_SIZE] = {0};
	uint8_t imagegreen[IMAGE_BUFFER_SIZE] = {0};
	uint8_t imageblue[IMAGE_BUFFER_SIZE] = {0};

    while(1) {

    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);

//		//gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts RGB pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2) {
			imagered[i/2] = (uint8_t)((img_buff_ptr[i]&0xF8)>>2);
			imagegreen[i/2] = (uint8_t)((img_buff_ptr[i]&0x07)<<3)+(uint8_t)((img_buff_ptr[i+1] & 0xE0)>>5);
			imageblue[i/2] = (uint8_t)((img_buff_ptr[i+1]&0x1F)<<1);
		}

		ambient_light(imagered, imagegreen, imageblue);
    }
}

//void extract_pixels_RGB (uint8_t *red_buffer, uint8_t *green_buffer, uint8_t *blue_buffer, uint8_t** image_buffer_ptr) {
//
//	extract_pixels_R(red_buffer,*image_buffer_ptr);
//	extract_pixels_G(green_buffer,*image_buffer_ptr);
//	extract_pixels_B(blue_buffer,*image_buffer_ptr);
//
//}
//
//void extract_pixels_R (uint8_t *red_buffer, uint8_t *image_buffer_ptr) {
//	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2) {
//		red_buffer[i/2] = (uint8_t)((image_buffer_ptr[i]&0xF8)>>2);
//	}
//}
//
//void extract_pixels_G (uint8_t *green_buffer, uint8_t *image_buffer_ptr) {
//	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2) {
//		green_buffer[i/2] = (uint8_t)((image_buffer_ptr[i]&0x07)<<3)+(uint8_t)((image_buffer_ptr[i+1] & 0xE0)>>5);
//	}
//}
//
//void extract_pixels_B (uint8_t *blue_buffer, uint8_t *image_buffer_ptr) {
//	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2) {
//		blue_buffer[i/2] = (uint8_t)((image_buffer_ptr[i+1]&0x1F)<<1);
//	}
//}


void get_dcmi_capture (void) {
	chMtxLock(&image_buffer_mutex);
	//asks for a capture to be done
	dcmi_capture_start();
	//waits for the capture to be done
	wait_image_ready();
	chMtxUnlockAll();
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO+5, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO+5, CaptureImage, NULL);
}

void polite_puck_start(void) {
	GoodMorning_LED();
	chThdCreateStatic(waPolitePuck, sizeof(waPolitePuck), NORMALPRIO+4, PolitePuck, NULL);
}

