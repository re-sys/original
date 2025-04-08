#include "key.h"
#define LONG_PRESS_TIME 1000            // 长按时间阈值，单位ms





uint8_t Key_Scan(void)
{
    if(!DL_GPIO_readPins(KEY_PORT, KEY_key_1_PIN))
    {
        return 1;
    }
    else if(!DL_GPIO_readPins(KEY_PORT, KEY_key_2_PIN))
    {
        return 2;
    }
    
    return 0;
}

uint8_t Key_GetNum(void)
{
    static uint32_t PressTime = 0;       // 静态变量保持其值
 
    static uint8_t KeyNum = 0;                  // 默认键码为0
   
    
    KeyNum = Key_Scan();
    if (KeyNum != 0)
    {
        while(Key_Scan() == KeyNum)
        {
            delay_ms(10);
            PressTime += 10;
            // CurrentTime += 10; 
        }
        
        if (PressTime >= 20 && PressTime < LONG_PRESS_TIME)
        {
                if(KeyNum==1)
                {
                    mode_num++;
                    if(mode_num>4) mode_num=0;
                }
                else {
                   mode_set = mode_num;
                   mode_num = 0;
                }
                return KeyNum;           // 返回单击键码
            
        }
    }
    else
    {		
					
        PressTime = 0;                   // 按键松开时重置PressTime
		// long_press = 0;
        // CurrentTime += 10;               // 模拟系统时间的增加
    }
    
    return 0;
}