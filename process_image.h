#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

//define for process image
#define DAWN					30// 70//100//25
#define DUSK					60//30//100
#define IMAGE_BUFFER_SIZE		640

#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>
#include <chbsem.h>
#include <hal.h>
#include "selector.h"
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
#include <spi_comm.h>				//to be able to use the RGB LEDs
#include "sensors/proximity.h"		//to be able to use the proximity threads
#include <leds.h>					//to use the different LED functions
#include <motors.h>					//to use the different motor functions
#include <sensors/imu.h>
#include <msgbus/messagebus.h>
#include <i2c_bus.h>
#include <audio/audio_thread.h>
#include <audio/microphone.h>
#include <audio/play_melody.h>
#include <audio/play_sound_file.h>	//to play specific sounds from the SD card
//include our files
#include <audio_processing.h>
#include <puck_led.h>
#include <control.h>
#include <puck_movement.h>

extern mutex_t image_buffer_mutex;

void test_lecture_(void);
float get_distance_cm(void);
uint16_t get_line_position(void);
void process_image_start(void);
void ambient_light(uint8_t *buffer1, uint8_t *buffer2, uint8_t *buffer3);
void get_dcmi_capture (void);
void polite_puck_start(void);

//unused:
//void extract_pixels_R(uint8_t *red_buffer, uint8_t *image_buffer_ptr);
//void extract_pixels_G(uint8_t *green_buffer, uint8_t *image_buffer_ptr);
//void extract_pixels_B(uint8_t *blue_buffer, uint8_t *image_buffer_ptr);
//void extract_pixels_RGB(uint8_t *red_buffer, uint8_t *green_buffer, uint8_t *blue_buffer, uint8_t **image_buffer_ptr);

#endif /* PROCESS_IMAGE_H */
