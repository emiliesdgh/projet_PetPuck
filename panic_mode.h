#ifndef PANIC_MODE_H
#define PANIC_MODE_H

//#include "main.h" ??
#include <hal.h>
#include "sensors/mpu9250.h"

#include <sensors/imu.h>


#define NB_SAMPLES_OFFSET     200
#define STANDARD_GRAVITY    9.80665f
#define DEG2RAD(deg) (deg / 180 * M_PI)

#define RES_2G		2.0f
#define RES_250DPS	250.0f
#define MAX_INT16	32768.0f

#define ACC_RAW2G	 (RES_2G / MAX_INT16)	//2G scale for 32768 raw value
#define GYRO_RAW2DPS (RES_250DPS / MAX_INT16)	//250DPS (degrees per second) scale for 32768 raw value

void imu_compute_offset(messagebus_topic_t * imu_topic, uint16_t nb_samples);

void test_main_panic(void);

void show_gravity(imu_msg_t *imu_values);
void PanicMode_start(void);
void led_test_start(void);



#ifdef __cplusplus
}
#endif

#endif


