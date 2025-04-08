#ifndef	__BOARD_H__
#define __BOARD_H__

#include "ti_msp_dl_config.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "dmpKey.h"
#include "dmpmap.h"
#include "mpu6050.h"
#include "IOI2C.h"
#include "filter.h"
#include "string.h"
#include "math.h"
#include "motor.h"
#define ABS(a)      (a>0 ? a:(-a))
extern int32_t Get_Encoder_countA,Get_Encoder_countB;
extern int angle1,angle2;
extern volatile float Speed_Set[2];
extern float Angle_Target;
void delay_us(unsigned long __us);
void delay_ms(unsigned long ms);
void delay_1us(unsigned long __us);
void delay_1ms(unsigned long ms);

void uart0_send_char(char ch);
void uart0_send_string(char* str);
void parse_received_data();

void USART_send_byte(uint8_t ch);
void USART_Send_bytes(uint8_t *Buffer, uint8_t Length);
void USART_Send(uint8_t *Buffer, uint8_t Length);
void send_out(int16_t *data, uint8_t length, uint8_t send);
uint8_t Check(uint8_t *data);

#endif
