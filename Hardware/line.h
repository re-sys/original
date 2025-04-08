#ifndef _LINE_H
#define _LINE_H
#include "ti_msp_dl_config.h"
#include <stdint.h>
#include"board.h"

extern volatile int Filtered_Arr[8]; // 滤波后的值存储
extern volatile int Line_Arr[8];
extern volatile uint8_t last_sum ;
extern volatile uint8_t flag_sum ;
extern float MAX_SPEED_DIFF;
uint8_t read_lines(void);
int calculate_speed_difference(volatile int sensor_values[]);

void Updata_TempArr() ;
void filterValues();
void Reset_Line();

#endif