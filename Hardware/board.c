#include "board.h"
#include "line.h"
#include "stdio.h"
#include "string.h"
#define RE_0_BUFF_LEN_MAX	128
#define RPY 0x45  // 欧拉角功能字芄1�7



volatile float Speed_Set[2] = {0,0};
float Angle_Target = 0;
volatile uint8_t  recv0_buff[RE_0_BUFF_LEN_MAX] = {0};
volatile uint16_t recv0_length = 0;
volatile uint8_t  recv0_flag = 0;


//�����δ���ʱ��ʵ�ֵľ�ȷus��ʱ
void delay_us(unsigned long __us) 
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 38;

    // ������Ҫ��ʱ���� = �ӳ�΢���� * ÿ΢����ʱ����
    ticks = __us * (32000000 / 1000000);

    // ��ȡ��ǰ��SysTickֵ
    told = SysTick->VAL;

    while (1)
    {
        // �ظ�ˢ�»�ȡ��ǰ��SysTickֵ
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;

            told = tnow;

            // �����ﵽ����Ҫ��ʱ���������˳�ѭ��
            if (tcnt >= ticks)
                break;
        }
    }
}
//�����δ���ʱ��ʵ�ֵľ�ȷms��ʱ
void delay_ms(unsigned long ms) 
{
	delay_us( ms * 1000 );
}

void delay_1us(unsigned long __us){ delay_us(__us); }
void delay_1ms(unsigned long ms){ delay_ms(ms); }

//���ڷ��͵����ַ�
void uart0_send_char(char ch)
{
	//������0æ��ʱ���ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
	while( DL_UART_isBusy(UART_0_INST) == true );
	//���͵����ַ�
	DL_UART_Main_transmitData(UART_0_INST, ch);

}
//���ڷ����ַ���
void uart0_send_string(char* str)
{
	//��ǰ�ַ�����ַ���ڽ�β ���� �ַ����׵�ַ��Ϊ��
	while(*str!=0&&str!=0)
	{
		//�����ַ����׵�ַ�е��ַ��������ڷ�������֮���׵�ַ����
		uart0_send_char(*str++);
	}
}


#if !defined(__MICROLIB)
//��ʹ��΢���Ļ�����Ҫ���������ĺ���
#if (__ARMCLIB_VERSION <= 6000000)
//������������AC5  �Ͷ������������ṹ��
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
#endif


//printf�����ض���
int fputc(int ch, FILE *stream)
{
	//������0æ��ʱ���ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
	while( DL_UART_isBusy(UART_0_INST) == true );

	DL_UART_Main_transmitDataBlocking(UART_0_INST, ch);

	return ch;
}

int fputs(const char* restrict s,FILE* restrict stream)
{
   uint16_t i,len;
   len = strlen(s);
   for(i=0;i<len;i++)
   {
       DL_UART_Main_transmitDataBlocking(UART_0_INST,s[i]);
   }
   return len;
}

int puts(const char *_ptr)
{
    int count = fputs(_ptr,stdout);
    count += fputs("\n",stdout);
    return count;
}

//******************************************


void parse_received_data() {
    // ȷ�� recv0_buff ���� '\0' ��β���ַ���
    recv0_buff[recv0_length] = '\0';

    if (recv0_buff[0] == '@') {
        float kp, ki, kd,kp_y,kd_y,ki_y;
        if (sscanf((const char*)recv0_buff, "@%f,%f,%f,%f,%f,%f", &kp, &ki, &kd,&kp_y,&kd_y,&ki_y) == 6) {
            Velcity_Kp = kp;
            Velcity_Ki= ki;
            Velcity_Kd = kd;
            Kp_Yaw  = kp_y;
            Kd_Yaw  =  kd_y;
            Ki_Yaw =  ki_y;
        }
    } else if (recv0_buff[0] == '#') {
        float speed1, speed2;
        if (sscanf((const char*)recv0_buff, "#%f,%f", &speed1, &speed2) == 2) {
            Speed_Set[0] = speed1;
            Speed_Set[1] = speed2;
        }
    }
    //   else if (recv0_buff[0] == '<') {
    //     float Angle;
    //     if (sscanf((const char*)recv0_buff, "<%f", &Angle) == 1) {
    //         Angle_Target = Angle;
    //     }
    //   }
      else if (recv0_buff[0] == '>') {
        float speed1,Angle;
        if (sscanf((const char*)recv0_buff, ">%f,%f", &speed1,&Angle) == 2) {
            Angle_Target = Angle;
            Speed_Set[0] = speed1;
            Speed_Set[1] = speed1;
        }
      }
      else if (recv0_buff[0] == '!') {
        float diff;
        if (sscanf((const char*)recv0_buff, "!%f", &diff) == 1) {
          MAX_SPEED_DIFF = diff;
       }
     }
     else if (recv0_buff[0] == '<') {
        int a1,a2;
        if (sscanf((const char*)recv0_buff, "<%d,%d", &a1,&a2) == 2) {
          angle1=a1;
          angle2=a2;
       }
     }
  

    // ���ý��ջ�����
    recv0_length = 0;
    memset((void*)recv0_buff, 0, sizeof(recv0_buff));
}




//*******************************************

//���ڵ��жϷ�������
void UART_0_INST_IRQHandler(void)
{
	uint8_t receivedData = 0;
	
	//���������˴����ж�
	switch( DL_UART_getPendingInterrupt(UART_0_INST) )
	{
		case DL_UART_IIDX_RX://�����ǽ����ж�
			
			// ���շ��͹��������ݱ���
			receivedData = DL_UART_Main_receiveData(UART_0_INST);

			// ���黺�����Ƿ�����
			if (recv0_length < RE_0_BUFF_LEN_MAX - 1)
			{
				recv0_buff[recv0_length++] = receivedData;

				// �������������ٷ��ͳ�ȥ�������ش�����ע�͵�
				 uart0_send_char(receivedData);
			}
			else
			{
				recv0_length = 0;
			}

			// ���ǽ��ձ�־
			recv0_flag = 1;
            if (receivedData == '\n' || receivedData == '\r') {
                parse_received_data();
            }
		
			break;
		
		default://�����Ĵ����ж�
			break;
	}
}


//**************************************第二个uart
void USART_send_byte(uint8_t ch)
{
	while( DL_UART_isBusy(UART_1_INST) == true );// 等待发��完戄1�7
	DL_UART_Main_transmitData(UART_1_INST, ch);

}

void USART_Send_bytes(uint8_t *Buffer, uint8_t Length)
{
	uint8_t i = 0;
	while (i < Length)
	{
		USART_send_byte(Buffer[i++]);
	}
}

void USART_Send(uint8_t *Buffer, uint8_t Length)
{
	uint8_t i = 0;
	while (i < Length)
	{
		if (i < (Length - 1))
			Buffer[Length - 1] += Buffer[i]; // 累加Length-1前的数据
		USART_send_byte(Buffer[i++]);
	}
}

void send_out(int16_t *data, uint8_t length, uint8_t send)
{
	uint8_t TX_DATA[20], i = 0, k = 0;
	memset(TX_DATA, 0, (2 * length + 5)); // 清零缓存TX_DATA
	TX_DATA[i++] = 0X5A; // 帧头
	TX_DATA[i++] = 0X5A; // 帧头
	TX_DATA[i++] = send; // 功能字节
	TX_DATA[i++] = 2 * length; // 数据长度
	for (k = 0; k < length; k++) // 存入数据到缓存TX_DATA
	{
		TX_DATA[i++] = (uint16_t)data[k] >> 8;
		TX_DATA[i++] = (uint16_t)data[k];
	}
	USART_Send(TX_DATA, sizeof(TX_DATA));
}

uint8_t stata_reg = 0; // 接收状��寄存器
uint8_t data4_buf[7] = {0}; // 欧拉角数据缓孄1�7

uint8_t Check(uint8_t *data)
{
	uint8_t i = 0, flag = 0, length = 0, sum = 0x5a + 0x5a;
	if (stata_reg)
	{
		// GPIO_ResetBits(GPIOB, GPIO_Pin_9); // LED亄1�7
		if (stata_reg & RPY)
		{
			stata_reg ^= RPY; // 清标志位
			flag = 0x45; // 记录功能字节
			length = 6; // 记录数据长度
			/* 转移数据到data，避免串口中断对data4_buf的影哄1�7 */
			memcpy(data, data4_buf, sizeof(data4_buf));
			sum = sum + flag + length; // 累加帧头、功能字节��数据长庄1�7
		}
		for (i = 0; i < length; i++) // 累加数据
		{
			sum += data[i];
		}
		if (sum != data[i]) // 判断校验和是否正硄1�7
			return 0;
		else
			return flag; // 返回功能字节
	}
	else
		return 0;
}

void UART_1_INST_IRQHandler(void)
{
    static uint8_t rebuf[13] = {0}, i = 0;
    if (DL_UART_getPendingInterrupt(UART_1_INST))
    {
        rebuf[i++] = DL_UART_Main_receiveData(UART_1_INST);
        if (rebuf[0] != 0x5a) // 判断帧头
            i = 0;
        if ((i == 2) && (rebuf[1] != 0x5a)) // 判断帧头
            i = 0;
        if (i > 4) // 当i计数倄1�7=5时，功能字节接收完毕，数据长度字节接收完毄1�7
        {
            if (rebuf[2] == 0x45) // 判断功能字节为欧拉角数据
            {
                data4_buf[i - 5] = rebuf[i - 1]; // 保存欧拉角数捄1�7

                // 判断数据接收完毕
                // rebuf[3]为数据个数，5=两帧处1�7+1个功能字芄1�7+1个数据长度字芄1�7+1个数据校验和字节
                if (i == rebuf[3] + 5)
                {
                    stata_reg |= RPY; // 欧拉角数据接收完毄1�7
                    i = 0;
                }
            }
        }
        // USART_ClearFlag(USART1, USART_FLAG_RXNE); // 清中断标忄1�7
    }
}