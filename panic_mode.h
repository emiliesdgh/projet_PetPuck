#ifndef PANIC_MODE_H
#define PANIC_MODE_H

//#include "main.h" ??
#include <hal.h>
#include "sensors/mpu9250.h"

#include <sensors/imu.h>

#define THRESHOLD_ACC	2.0
#define CST_RADIAN		(180.0/3.1415)	// used to convert radian in degrees

//
//#define NB_SAMPLES_OFFSET     200
//#define STANDARD_GRAVITY    9.80665f
//#define DEG2RAD(deg) (deg / 180 * M_PI)
//
//#define RES_2G		2.0f
//#define RES_250DPS	250.0f
//#define MAX_INT16	32768.0f
//
//#define ACC_RAW2G	 (RES_2G / MAX_INT16)	//2G scale for 32768 raw value
//#define GYRO_RAW2DPS (RES_250DPS / MAX_INT16)	//250DPS (degrees per second) scale for 32768 raw value


//void get_orientation(imu_msg_t *imu_values);

int get_inclination(imu_msg_t *imu_values);

void PanicMode_start(void);



#ifdef __cplusplus
}
#endif

#endif


