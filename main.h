#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"

//define for obstacle encounter
#define SPEED_MAX	1000
#define	DISTANCE_MIN	100.0f

//define for danse mode
#define NSTEP_ONE_TURN      1000 // number of step for 1 turn of the motor
#define NSTEP_ONE_EL_TURN   4  //number of steps to do 1 electrical turn
#define NB_OF_PHASES        4  //number of phases of the motors
#define WHEEL_PERIMETER     13 // [cm]
#define SPEED_CONTROL       0
#define POSITION_CONTROL    1

//uncomment to use python script and read microhpone data -- careful: also change MICSAMPLESIZE to 1024 in audio_processing.h
#define TESTING


/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif


#endif
