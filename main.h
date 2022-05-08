#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "sensors/proximity.h"
#include "parameter/parameter.h"

//max intensity for the RGB LEDs
#define LED_RGB_INTENSITY 100

//define for process image
#define DAWN					25//50
#define DUSK					100//70
#define IMAGE_BUFFER_SIZE		640

//define for color image
#define WIDTH_SLOPE				5
#define MIN_LINE_WIDTH			40

#define ROTATION_THRESHOLD		10
#define ROTATION_COEFF			2
#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE 			10.0f
#define MAX_DISTANCE 			25.0f
#define ERROR_THRESHOLD			0.1f	//[cm] because of the noise of the camera
#define KP						800.0f//20.0f //800.0f
#define KI 						3.5f //1.1f//10.1f//3.5f	//must not be zero
#define MAX_SUM_ERROR 			(MOTOR_SPEED_LIMIT/KI)



//define for obstacle encounter
#define SPEED_MAX	1000
#define	DISTANCE_MIN	75.0f

//define for dance mode
#define NSTEP_ONE_TURN      1000 // number of step for 1 turn of the motor
#define NSTEP_ONE_EL_TURN   4  //number of steps to do 1 electrical turn
#define NB_OF_PHASES        4  //number of phases of the motors
#define WHEEL_PERIMETER     13 // [cm]
#define SPEED_CONTROL       0
#define POSITION_CONTROL    1

//define for panic mode
#define ANGULAR_ACC_DEATH		2.0f


//uncomment to use python script and read microhpone data -- careful: also change MICSAMPLESIZE to 1024 in audio_processing.h
#define TESTING


/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif


#endif
