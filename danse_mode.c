// C standard header files
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <math.h>

 // ChibiOS headers
 #include "ch.h"
 #include "hal.h"
 #include "memory_protection.h"
 #include "msgbus/messagebus.h"
 #include "chprintf.h"
 #include <usbcfg.h>

 //// e-puck 2 main processor headers
 //#include "sensors/VL53L0X/VL53L0X.h"
 //#include "sensors/proximity.h"
 //
 //#include <leds.h>
 //#include <spi_comm.h>

 #include <selector.h>
 #include <main.h>
 #include <motors.h>
 //
 //#include <obstacle_encounter.h>
//#include <proximity_sensor.h>



void danseMode(int16_t danse_speed){

 	for(int i=0;i<3;i++){

 		left_motor_set_speed(danse_speed);
 		right_motor_set_speed(-danse_speed);
 		chThdSleepMilliseconds(1000);
 	}
 	for(int i=0;i<3;i++){

 		left_motor_set_speed(-danse_speed);
 		right_motor_set_speed(danse_speed);
 		chThdSleepMilliseconds(1000);
 	}

// 	for(int i = 0; i<10; i++){
// 		left_motor_set_pos(5);
// 		right_motor_set_pos(-5);
// 	}


 }
