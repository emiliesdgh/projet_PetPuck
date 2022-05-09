#ifndef OBSTACLE_ENCOUNTER_H
#define OBSTACLE_ENCOUNTER_H

//#include "main.h" ??



void initial_proximity(void);
void proximityToStop_start(void);

uint16_t get_distance_toStop(void);
int get_ambient_testing(void);

void ObstacleEncounter_start(void);
int16_t motors_speed(uint16_t distance_mm);

uint32_t get_colors(void);
//void process_color_start(void);

#endif
