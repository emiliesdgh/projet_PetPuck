#ifndef PUCK_MOVEMENT_H
#define PUCK_MOVEMENT_H

//#include "main.h" ??


void initial_proximity(void);
//uint16_t get_distance_toStop(void);
int get_ambient_testing(void);

void motor_set_danse_speed(float speed_r, float speed_l);
void dancing_puck(void);
//void DancingPuck_start(void);

//uint16_t get_distance_toStop(void);
//void proximityToStop_start(void);

int16_t motors_speed(uint16_t distance_mm);
uint32_t get_colors(void);
void ObstacleEncounter_start(void);

//void process_color_start(void);

#endif
