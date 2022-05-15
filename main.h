#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

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
#include <camera/po8030.h>
#include "camera/dcmi_camera.h"

//include  the files from the given library
#include <spi_comm.h>			//to be able to use the RGB LEDs
#include "sensors/proximity.h"	//to be  able to use the proximity threads
#include "parameter/parameter.h"
#include <leds.h>				//to use the different LED functions that exist already
#include <motors.h>				//to use the different motor functions that exist already
//for the  panic mode : gyroscope + accéléromètre
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
#include <puck_led.h>
#include <process_image.h>
#include <control.h>
#include <puck_movement.h>	//--->>> to merge with danse_mode and proximity_sensors maybe

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif


#endif
