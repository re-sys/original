#include "motor.h"
#define offset_MAX 10
#define dead_PWM 0
float Velcity_Kp=30.0f,  Velcity_Ki=3.0f,  Velcity_Kd=5.0f; //�����ٶ�PID����
float Kp_Yaw=-0.4,Kd_Yaw=-100,Ki_Yaw=0;
float cur_bias=0;
float Yaw_new = 0;


/***********************************************
��˾����Ȥ�Ƽ�����ݸ�����޹�˾
Ʒ�ƣ�WHEELTEC
������wheeltec.net
�Ա����̣�shop114407458.taobao.com 
����ͨ: https://minibalance.aliexpress.com/store/4455017
�汾��V1.0
�޸�ʱ�䣺2024-07-019

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version: V1.0
Update��2024-07-019

All rights reserved
***********************************************/
/***************************************************************************
�������ܣ�������PID�ջ�����
���ڲ��������ҵ����ı�����ֵ
����ֵ  ��������PWM
***************************************************************************/
int Velocity_A(int TargetVelocity, int CurrentVelocity)
{  
    int Bias;  //�������ر���
		static int ControlVelocityA, Last_biasA,Last_last_biasA; //��̬�������������ý�������ֵ��Ȼ����
		
		Bias=TargetVelocity-CurrentVelocity; //���ٶ�ƫ��
		
		ControlVelocityA+=Velcity_Kp*(Bias-Last_biasA)+Velcity_Ki*Bias +Velcity_Kd * (Bias - 2*Last_biasA+ Last_last_biasA);  //����ʽPI������
                                                                   //Velcity_Kp*(Bias-Last_bias) ����Ϊ���Ƽ��ٶ�
	                                                                 //Velcity_Ki*Bias             �ٶȿ���ֵ��Bias���ϻ��ֵõ� ƫ��Խ�����ٶ�Խ��
		Last_last_biasA = Last_biasA;
        Last_biasA=Bias;	
	    if(ControlVelocityA>3600) ControlVelocityA=3600;
	    else if(ControlVelocityA<-3600) ControlVelocityA=-3600;
		return ControlVelocityA; //�����ٶȿ���ֵ
}

/***************************************************************************
�������ܣ�������PID�ջ�����
���ڲ��������ҵ����ı�����ֵ
����ֵ  ��������PWM
***************************************************************************/
int Velocity_B(int TargetVelocity, int CurrentVelocity)
{  
    int Bias;  //�������ر���
		static int ControlVelocityB, Last_biasB ,Last_last_biasB; //��̬�������������ý�������ֵ��Ȼ����
		
		Bias=TargetVelocity-CurrentVelocity; //���ٶ�ƫ��
		
		ControlVelocityB+=Velcity_Kp*(Bias-Last_biasB)+Velcity_Ki*Bias +Velcity_Kd * (Bias - 2*Last_biasB+ Last_last_biasB) ;  //����ʽPI������
                                                                   //Velcity_Kp*(Bias-Last_bias) ����Ϊ���Ƽ��ٶ�
                                                                     //Velcity_Ki*Bias             �ٶȿ���ֵ��Bias���ϻ��ֵõ� ƫ��Խ�����ٶ�Խ��
		Last_last_biasB = Last_biasB;
        Last_biasB=Bias;	
        
	    if(ControlVelocityB>3600) ControlVelocityB=3600;
	    else if(ControlVelocityB<-3600) ControlVelocityB=-3600;
		return ControlVelocityB; //�����ٶȿ���ֵ
}

void Set_PWM(int pwma,int pwmb)
{
	//��ʹ����˾��D153C����ģ����ʱ����PA12��AIN2��PA13��AIN1,PB16��BIN2��PB0����BIN1
	if(pwma>0)
	{
		DL_GPIO_setPins(AIN1_PORT,AIN1_PIN_12_PIN);
		DL_GPIO_clearPins(AIN2_PORT,AIN2_PIN_13_PIN);
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwma)+dead_PWM,GPIO_PWM_0_C0_IDX);
        
	}
	else
	{
		DL_GPIO_setPins(AIN2_PORT,AIN2_PIN_13_PIN);
		DL_GPIO_clearPins(AIN1_PORT,AIN1_PIN_12_PIN);
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwma)+dead_PWM,GPIO_PWM_0_C0_IDX);
	}
	if(pwmb>0)
	{
		DL_GPIO_setPins(BIN1_PORT,BIN1_Pin_Bin1_PIN);
		DL_GPIO_clearPins(BIN2_PORT,BIN2_Pin_Bin2_PIN);
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmb)+dead_PWM,GPIO_PWM_0_C1_IDX);
	}
    else
	{
		DL_GPIO_setPins(BIN2_PORT,BIN2_Pin_Bin2_PIN);
		DL_GPIO_clearPins(BIN1_PORT,BIN1_Pin_Bin1_PIN);
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmb)+dead_PWM,GPIO_PWM_0_C1_IDX);
	}		

}

int yaw2vel(float Angle_Target)
{
    static float  last_bias=0, error_rate=0;
    static int output;

    cur_bias = Angle_Target - Yaw_new;

    error_rate = cur_bias - last_bias;

    output = cur_bias * Kp_Yaw + error_rate * Kd_Yaw;

    last_bias = cur_bias;
    

    if(ABS(output)<offset_MAX)
    return (int)(output);
    else 
    return (output>0) ? offset_MAX : -offset_MAX;
}



