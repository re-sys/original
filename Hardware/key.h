#ifndef __KEY_H
#define __KEY_H
#include "ti_msp_dl_config.h"
#include "board.h"
extern uint8_t mode_num;
extern bool is_enter;
extern uint8_t mode_set;

uint8_t Key_Scan(void);
uint8_t Key_GetNum(void);


#endif
