/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "board.h"
#include "motor.h"
#include "oled.h"
#include <stdint.h>
#include "line.h"
#include "key.h"
#include "line.h"
#define Pi 3.14159265

int Get_Encoder_countA,encoderA_cnt,Get_Encoder_countB,encoderB_cnt,PWMA,PWMB;
uint8_t Way_Angle=1;                             //��ȡ�Ƕȵ��㷨��1����Ԫ��  2��������  3�������˲�
float Angle_Balance,Gyro_Balance,Gyro_Turn; //ƽ����� ƽ�������� ת��������
float Acceleration_Z,Target;
int offset=0;
int speed_correct;
int flag_A=0;
bool yes_angle_control = false;

uint8_t mode_num = 0;
uint8_t mode_set = 0;
bool is_enter;
bool is_init=false;
char str[30];


uint8_t data_buf[7] = {0};
int16_t ROLL = 0, PITCH = 0, YAW = 0;
int16_t rpy[3] = {0,0,0};
int16_t Init_rpy[3] = {0,0,0};
void Angle_Init(void);
void Get_Angle(uint8_t way);
void send_Instruction(void);
int angle1=40,angle2=140;

int main(void)
{
    SYSCFG_DL_init();
    OLED_Init();
    DL_Timer_startCounter(PWM_0_INST);//����PWM����
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);//�����жϱ�־λ
    NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    // MPU6050_initialize();
    // DMP_Init();
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);//�����ж�
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);

    delay_ms(2000);
    send_Instruction(); 
    

    while (1) {
        while(mode_set == 0)
        {
            Set_PWM(0,0);
             Get_Angle(1);
             Key_GetNum();
             screen_show();   
            // is_init=false;
             Reset_Line();
             printf("%f,%d,%d\n",Yaw_new,angle1,angle2);
        }

        if(mode_set == 1){
            while(Init_rpy[2] == 0)
            {   
                Angle_Init();
                //is_init=true;
            }

            flag_sum=read_lines();
            Speed_Set[0] = 20;
            Speed_Set[1] = 20;
            
            if(flag_sum == 1)
            {
                Angle_Target = 0;
                Get_Angle(1);
            }

            if (flag_sum==2)
            {
                delay_ms(20);
                Speed_Set[0]=0;
                Speed_Set[1]=0;
                
                 mode_set = 0;
                 continue;
            }
            speed_correct = calculate_speed_difference(Line_Arr);
            printf("%d,%d,%d\n",mode_num,mode_set,flag_sum);
        }

        else if(mode_set == 2){
            while(Init_rpy[2] == 0)
            {   
                Angle_Init();
                is_init=true;
            }

            flag_sum=read_lines();
            Speed_Set[0] = 20;
            Speed_Set[1] = 20;

            if(flag_sum == 1)
            {
                Angle_Target = 0;
                Get_Angle(1);
            }
            else if (flag_sum == 3)
            {
                Angle_Target = 180;
                Get_Angle(1);
            }

            if (flag_sum==5)
            {
                delay_ms(20);
                Speed_Set[0]=0;
                Speed_Set[1]=0;
                
                mode_set = 0;
                 continue;
            }
            speed_correct = calculate_speed_difference(Line_Arr);
            //  printf("%d,%f,%f,%f,%d,%d\n\r",encoderA_cnt,cur_bias,Yaw,Speed_Set[0],PWMA,offset);
            printf("%d,%d,%d,%d,%f,%d,%f\n",rpy[2],Init_rpy[2],offset,PWMB,Yaw_new,speed_correct,Speed_Set[1]);
            //  printf("%d,%d,%d\n",mode_num,mode_set,flag_sum);
        }
        else if(mode_set == 3){
            while(Init_rpy[2] == 0)
            {   
                Angle_Init();
               // is_init=true;
            }

            flag_sum=read_lines();
            Speed_Set[0] = 20;
            Speed_Set[1] = 20;
            
            if(flag_sum == 1)
            {
                Angle_Target = angle1;
                Get_Angle(1);
            }
            else if(flag_sum == 2)
            {
                if(  Yaw_new>-30)
                {
                    yes_angle_control = true;
                    Angle_Target = -30;
                }
                else{
                    yes_angle_control = false;
                }
            }
            else if (flag_sum == 3)
            {
                Angle_Target = angle2;
                Get_Angle(1);
            }

            else if(flag_sum == 4)
            {
                if(  Yaw_new<210)
                {
                    yes_angle_control = true;
                    Angle_Target = 210;
                }
                else{
                    yes_angle_control = false;
                }
            }

            if (flag_sum==5)
            {
                delay_ms(20);
                Speed_Set[0]=0;
                Speed_Set[1]=0;
                
                 mode_set = 0;
                  continue;
            }
            speed_correct = calculate_speed_difference(Line_Arr);
            //  printf("%d,%f,%f,%f,%d,%d\n\r",encoderA_cnt,cur_bias,Yaw,Speed_Set[0],PWMA,offset);
            printf("%f,%d,%d,%d\n",Yaw_new,angle1,angle2,flag_sum);
        }
        else if(mode_set == 4){
            while(Init_rpy[2] == 0)
            {   
                Angle_Init();
               // is_init=true;
            }

            flag_sum=read_lines();
            Speed_Set[0] = 30;
            Speed_Set[1] = 30;
            
            if ((flag_sum % 4)==1 && flag_sum<17)
            {
                Angle_Target = angle1;
                Get_Angle(1);
            }

            else if((flag_sum % 4)==3)
            {
                Angle_Target = angle2;
                Get_Angle(1);
            }

            else if((flag_sum % 4)==2)
            {
                if(  Yaw_new>-30)
                {
                    yes_angle_control = true;
                    Angle_Target = -30;
                }
                else{
                    yes_angle_control = false;
                }
            }
            
            else if((flag_sum % 4)==0)
            {
                if(  Yaw_new<210)
                {
                    yes_angle_control = true;
                    Angle_Target = 210;
                }
                else{
                    yes_angle_control = false;
                }
            }

            else if(flag_sum ==17 )
            {
                delay_ms(20);
                Speed_Set[0]=0;
                Speed_Set[1]=0;
                
                 mode_set = 0;
                  continue;
            }
            speed_correct = calculate_speed_difference(Line_Arr);
            //  printf("%d,%f,%f,%f,%d,%d\n\r",encoderA_cnt,cur_bias,Yaw,Speed_Set[0],PWMA,offset);
            printf("%f,%d,%d\n",Yaw_new,angle1,angle2);
        }
    }
}


void TIMER_0_INST_IRQHandler(void)
{
    if(DL_TimerA_getPendingInterrupt(TIMER_0_INST))
    {
        if(mode_set != 0)
        {
            if(DL_TIMER_IIDX_ZERO)
            {
            encoderA_cnt = -Get_Encoder_countA;
            encoderB_cnt = Get_Encoder_countB;
            Get_Encoder_countA = 0;
            Get_Encoder_countB = 0;
            // LED_Flash(100);//LED1��˸
            if((flag_sum)%2 >0 || yes_angle_control)
            {
            offset = yaw2vel(Angle_Target);    
            }      
            else{
                offset = 0;
            }
            PWMA = Velocity_A(Speed_Set[0]-offset+speed_correct,encoderA_cnt);
            PWMB = Velocity_B(Speed_Set[1]+offset-speed_correct,encoderB_cnt);
            Set_PWM(PWMA,PWMB);
            }
        }
       
    }
}

void Get_Angle(uint8_t way)
{
          if (Check(data_buf) == 0x45) // 检查欧拉角数据接收完毕
        {
            ROLL = (data_buf[0] << 8) | data_buf[1];
            PITCH = (data_buf[2] << 8) | data_buf[3];
            YAW = (data_buf[4] << 8) | data_buf[5];
            rpy[0] = ROLL;
            rpy[1] = PITCH;
            rpy[2] = YAW;
            send_out(rpy, 3, 0x45); // 发送欧拉角数据
            Yaw_new =(rpy[2]-Init_rpy[2])/100.0;
            if(Yaw_new<-135)
            {
               Yaw_new+=360;
            }    
        }

    // float gyro_x,gyro_y,gyro_z,accel_x,accel_y,accel_z;
    // float Accel_Y,Accel_Z,Accel_X,Accel_Angle_x,Accel_Angle_z,Accel_Angle_y,Gyro_X,Gyro_Z,Gyro_Y;
    // if(way==1)                           //DMP�Ķ�ȡ�����ݲɼ��ж϶�ȡ���ϸ���ѭʱ��Ҫ��
    // {
    //     Read_DMP();                          //��ȡ���ٶȡ����ٶȡ����
    //     Angle_Balance=Pitch;                 //����ƽ�����,ǰ��Ϊ��������Ϊ��
    //     Gyro_Balance=gyro[0];              //����ƽ����ٶ�,ǰ��Ϊ��������Ϊ��
    //     Gyro_Turn=gyro[2];                 //����ת����ٶ�
    //     Acceleration_Z=accel[2];  

        


    //       //����Z����ٶȼ�
    // }
    // else
    // {
    //     Gyro_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_XOUT_L);    //��ȡX��������
    //     Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //��ȡY��������
    //     Gyro_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_L);    //��ȡZ��������
    //     Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //��ȡX����ٶȼ�
    //     Accel_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_YOUT_L); //��ȡX����ٶȼ�
    //     Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //��ȡZ����ٶȼ�
    //     if(Gyro_X>32768)  Gyro_X-=65536;                 //��������ת��  Ҳ��ͨ��shortǿ������ת��
    //     if(Gyro_Y>32768)  Gyro_Y-=65536;                 //��������ת��  Ҳ��ͨ��shortǿ������ת��
    //     if(Gyro_Z>32768)  Gyro_Z-=65536;                 //��������ת��
    //     if(Accel_X>32768) Accel_X-=65536;                //��������ת��
    //     if(Accel_Y>32768) Accel_Y-=65536;                //��������ת��
    //     if(Accel_Z>32768) Accel_Z-=65536;                //��������ת��
    //     Gyro_Balance=-Gyro_X;                            //����ƽ����ٶ�
    //     Accel_Angle_x=atan2(Accel_Y,Accel_Z)*180/Pi;     //������ǣ�ת����λΪ��
    //     Accel_Angle_y=atan2(Accel_X,Accel_Z)*180/Pi;     //������ǣ�ת����λΪ��
    //     Accel_Angle_z=atan2(Accel_Y,Accel_X)*180/Pi;
    //     accel_x=Accel_X/1671.84;
    //     accel_y=Accel_Y/1671.84;
    //     accel_z=Accel_Z/1671.84;
    //     gyro_x=Gyro_X/16.4;                              //����������ת��
    //     gyro_y=Gyro_Y/16.4;                              //����������ת��
    //     gyro_z = Gyro_Z/16.4;
    //     if(Way_Angle==2)
    //     {
    //          Pitch= -Kalman_Filter_x(Accel_Angle_x,gyro_x);//�������˲�
    //          Roll = -Kalman_Filter_y(Accel_Angle_y,gyro_y);
    //     }
    //     else if(Way_Angle==3)
    //     {
    //          Pitch = -Complementary_Filter_x(Accel_Angle_x,gyro_x);//�����˲�
    //          Roll = -Complementary_Filter_y(Accel_Angle_y,gyro_y);
    //         //  Yaw = -Complementary_Filter_y(Accel_Angle_z,gyro_z);
    //     }
    //     Angle_Balance=Pitch;                              //����ƽ�����
    //     Gyro_Turn=Gyro_Z;                                 //����ת����ٶ�
    //     Acceleration_Z=Accel_Z;                           //����Z����ٶȼ�
    // }
}

void send_Instruction(void)
{
    uint8_t send_data[3] = {0};
    send_data[0] = 0xa5;
    send_data[1] = 0x45; // 欧拉角功能字节
    send_data[2] = (uint8_t)(send_data[0] + send_data[1]); // 指令校验和
    USART_Send_bytes(send_data, 3); // 发送欧拉角数据输出指令

}

void Angle_Init(void)
{
  if (Check(data_buf) == 0x45) // 检查欧拉角数据接收完毕
        {
            ROLL = (data_buf[0] << 8) | data_buf[1];
            PITCH = (data_buf[2] << 8) | data_buf[3];
            YAW = (data_buf[4] << 8) | data_buf[5];
            Init_rpy[0] = ROLL;
            Init_rpy[1] = PITCH;
            Init_rpy[2] = rpy[2];
            send_out(rpy, 3, 0x45); // 发送欧拉角数据
        }
}

void screen_show(void)
{
    // if(!is_enter)
    // {
        // if(mode_num==0)
        // {
            sprintf(str,"mode:%d",mode_num);
            OLED_ShowString(0,1,(uint8_t *)str,16);
            sprintf(str,"rpy:%d",rpy[2]);
            OLED_ShowString(0,3,(uint8_t *)str,16);
            
        //}
        // else if(mode_num==1)
        // {
        //     sprintf(str,"mode1");
        //     OLED_ShowString(0,1,(uint8_t *)str,16);
        // }
        // else if(mode_num==2)
        // {
        //     sprintf(str,"mode2");
        //     OLED_ShowString(0,1,(uint8_t *)str,16);
        // }
        // else if(mode_num==3)
        // {
        //     sprintf(str,"mode3");
        //     OLED_ShowString(0,1,(uint8_t *)str,16);
        // }
        // else if(mode_num==4)
        // {
        //     sprintf(str,"mode4");
        //     OLED_ShowString(0,1,(uint8_t *)str,16);
        // }
    // }
    // else 
    // {
    //     sprintf(str,"start!");
    //     OLED_ShowString(0,1,(uint8_t *)str,16);   
    // }
    
}


