#ifndef _MOTOR_H
#define _MOTOR_H
#include "ti_msp_dl_config.h"
#include "board.h"

extern float Velcity_Kp,  Velcity_Ki, Velcity_Kd; //�����ٶ�PID����
extern float Kp_Yaw,Kd_Yaw,Ki_Yaw,cur_bias;
extern int encoderA_cnt,encoderB_cnt;
extern float Yaw_new;


int yaw2vel(float Angle_Target);

int Velocity_A(int TargetVelocity, int CurrentVelocity);
int Velocity_B(int TargetVelocity, int CurrentVelocity);
void Set_PWM(int pwma,int pwmb);
#endif