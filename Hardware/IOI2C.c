/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：5.7
修改时间：2021-04-29

 
Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:5.7
Update：2021-04-29

All rights reserved
***********************************************/
#include "IOI2C.h"

/**************************************************************************
Function: Simulate IIC start signal
Input   : none
Output  : 1
函数功能：模拟IIC起始信号
入口参数：无
返回  值：1
**************************************************************************/
int IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SCL(1);
 	IIC_SDA(0); //START:when CLK is high,DATA change form high to low 
	IIC_SDA(1);
	delay_us(5);
	IIC_SDA(0);
	delay_us(5);
	IIC_SCL(0);//钳住I2C总线，准备发送或接收数据 
	return 1;
}

/**************************************************************************
Function: Analog IIC end signal
Input   : none
Output  : none
函数功能：模拟IIC结束信号
入口参数：无
返回  值：无
**************************************************************************/	  
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL(0);
	IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	delay_us(5);
	IIC_SCL(1);
    delay_us(5);	
	IIC_SDA(1);//发送I2C总线结束信号
	delay_us(5);							   	
}

/**************************************************************************
Function: IIC wait the response signal
Input   : none
Output  : 0：No response received；1：Response received
函数功能：IIC等待应答信号
入口参数：无
返回  值：0：没有收到应答；1：收到应答
**************************************************************************/
int IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA(1);
	delay_us(5);	   
	IIC_SCL(1);
	delay_us(1);	 
	while(SDA_GET())
	{
		ucErrTime++;
		if(ucErrTime>20)
		{
			IIC_Stop();
			return 0;
		}
	  delay_us(5);
	}
	IIC_SCL(0);//时钟输出0 	   
	return 1;  
} 

/**************************************************************************
Function: IIC response
Input   : none
Output  : none
函数功能：IIC应答
入口参数：无
返回  值：无
**************************************************************************/
void IIC_Ack(void)
{
	
	SDA_OUT();
	IIC_SCL(0);
	IIC_SDA(0);
	delay_us(5);
	IIC_SCL(1);
	delay_us(5);
	IIC_SCL(0);
	IIC_SDA(1);
}
	
/**************************************************************************
Function: IIC don't reply
Input   : none
Output  : none
函数功能：IIC不应答
入口参数：无
返回  值：无
**************************************************************************/    
void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(1);
	delay_us(5);
	IIC_SCL(1);
	delay_us(5);
	IIC_SCL(0);
	IIC_SDA(0);
}
/**************************************************************************
Function: IIC sends a byte
Input   : txd：Byte data sent
Output  : none
函数功能：IIC发送一个字节
入口参数：txd：发送的字节数据
返回  值：无
**************************************************************************/	  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL(0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
			IIC_SDA((txd&0x80)>>7);
			txd<<=1; 	  
			delay_us(5);   
			IIC_SCL(1);
			delay_us(5); 
			IIC_SCL(0);	
			delay_us(5);
    }	 
} 	 
  
/**************************************************************************
Function: IIC write data to register
Input   : addr：Device address；reg：Register address；len;Number of bytes；data：Data
Output  : 0：Write successfully；1：Failed to write
函数功能：IIC写数据到寄存器
入口参数：addr：设备地址；reg：寄存器地址；len;字节数；data：数据
返回  值：0：成功写入；1：没有成功写入
**************************************************************************/
int i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
		int i;
    if (!IIC_Start())
        return 1;
    IIC_Send_Byte(addr << 1 );
    if (!IIC_Wait_Ack()) {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
		for (i = 0; i < len; i++) {
        IIC_Send_Byte(data[i]);
        if (!IIC_Wait_Ack()) {
            IIC_Stop();
            return 0;
        }
    }
    IIC_Stop();
    return 0;
}
/**************************************************************************
Function: IIC read register data
Input   : addr：Device address；reg：Register address；len;Number of bytes；*buf：Data read out
Output  : 0：Read successfully；1：Failed to read
函数功能：IIC读寄存器的数据
入口参数：addr：设备地址；reg：寄存器地址；len;字节数；*buf：读出数据缓存
返回  值：0：成功读出；1：没有成功读出
**************************************************************************/

int i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    if (!IIC_Start())
        return 1;
    IIC_Send_Byte(addr << 1);
    if (!IIC_Wait_Ack()) {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte((addr << 1)+1);
    IIC_Wait_Ack();
    while (len) {
        if (len == 1)
            *buf = IIC_Read_Byte(0);
        else
            *buf = IIC_Read_Byte(1);
        buf++;
        len--;
    }
    IIC_Stop();
    return 0;
}

/**************************************************************************
Function: IIC reads a byte
Input   : ack：Send response signal or not；1：Send；0：Do not send
Output  : receive：Data read
函数功能：IIC读取一个位
入口参数：ack：是否发送应答信号；1：发送；0：不发送
返回  值：receive：读取的数据
**************************************************************************/ 
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	 {
			IIC_SCL(0); 
			delay_us(5);
			IIC_SCL(1);
			receive<<=1;
			if(SDA_GET())receive++;   
			delay_us(5); 
    }					 
    if (ack)
        IIC_Ack(); //发送ACK 
    else
        IIC_NAck();//发送nACK  
    return receive;
}

/**************************************************************************
Function: IIC reads a byte
Input   : I2C_Addr：Device IIC address；addr:Register address
Output  : res：Data read
函数功能：读取指定设备指定寄存器的一个值
入口参数：I2C_Addr：设备IIC地址；addr:寄存器地址
返回  值：res：读取的数据
**************************************************************************/ 
unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
{
	unsigned char res=0;
	
	IIC_Start();	
	IIC_Send_Byte(I2C_Addr);	   //发送写命令
	res++;
	IIC_Wait_Ack();
	IIC_Send_Byte(addr); res++;  //发送地址
	IIC_Wait_Ack();	  
	//IIC_Stop();//产生一个停止条件	
	IIC_Start();
	IIC_Send_Byte(I2C_Addr+1); res++;          //进入接收模式			   
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0);	   
  IIC_Stop();//产生一个停止条件

	return res;
}
 
/**************************************************************************
Function: IIC continuous reading data
Input   : dev：Target device IIC address；reg:Register address；
					length：Number of bytes；*data:The pointer where the read data will be stored
Output  : count：Number of bytes read out-1
函数功能：IIC连续读数据
入口参数：dev：目标设备IIC地址；reg:寄存器地址；length：字节数；
					*data:读出的数据将要存放的指针
返回  值：count：读出来的字节数量-1
**************************************************************************/ 
uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data){
    uint8_t count = 0;
	
	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
  IIC_Wait_Ack();	  
	IIC_Start();
	IIC_Send_Byte(dev+1);  //进入接收模式	
	IIC_Wait_Ack();
	
    for(count=0;count<length;count++){
		 
		 if(count!=length-1)   data[count]=IIC_Read_Byte(1);  //带ACK的读数据
		 else                  data[count]=IIC_Read_Byte(0);  //最后一个字节NACK
	}
    IIC_Stop();//产生一个停止条件
    return count;
}
/**************************************************************************
Function: Writes multiple bytes to the specified register of the specified device
Input   : dev：Target device IIC address；reg：Register address；length：Number of bytes；
					*data：The pointer where the read data will be stored
Output  : 1
函数功能：将多个字节写入指定设备指定寄存器
入口参数：dev：目标设备地址；reg：寄存器地址；length：要写的字节数；
					*data：将要写的数据的首地址
返回  值：1：返回是否成功
**************************************************************************/ 
uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data){
  
 	uint8_t count = 0;
	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
  IIC_Wait_Ack();	  
	for(count=0;count<length;count++){
		IIC_Send_Byte(data[count]); 
		IIC_Wait_Ack(); 
	 }
	IIC_Stop();//产生一个停止条件

    return 1; //status == 0;
}

/**************************************************************************
Function: Reads a byte of the specified register of the specified device
Input   : dev：Target device IIC address；reg：Register address；*data：The pointer where the read data will be stored
Output  : 1
函数功能：读取指定设备指定寄存器的一个值
入口参数：dev：目标设备地址；reg：寄存器地址；*data：将要写的数据的首地址
返回  值：1：返回是否成功
**************************************************************************/ 
uint8_t IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data){
	*data=I2C_ReadOneByte(dev, reg);
    return 1;
}

/**************************************************************************
Function: Write a byte to the specified register of the specified device
Input   : dev：Target device IIC address；reg：Register address；data：Data to be writtenwill be stored
Output  : 1
函数功能：写入指定设备指定寄存器一个字节
入口参数：dev：目标设备地址；reg：寄存器地址；data：将要写的数据
返回  值：1
**************************************************************************/ 
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
    return IICwriteBytes(dev, reg, 1, &data);
}

/**************************************************************************
Function: Read, modify, and write multiple bits in a byte of the specified device specified register
Input   : dev：Target device IIC address；reg：Register address；length：Number of bytes；
					bitStart：Start bit of target byte；data：Stores the value of the target byte bit to be changed
Output  : 1：success；0：fail
函数功能：读 修改 写 指定设备 指定寄存器一个字节 中的多个位
入口参数：dev：目标设备地址；reg：寄存器地址；bitStart：目标字节的起始位；
					data：存放改变目标字节位的值
返回  值：1：成功；0：失败
**************************************************************************/ 
uint8_t IICwriteBits(uint8_t dev,uint8_t reg,uint8_t bitStart,uint8_t length,uint8_t data)
{

    uint8_t b;
    if (IICreadByte(dev, reg, &b) != 0) {
        uint8_t mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
        data <<= (8 - length);
        data >>= (7 - bitStart);
        b &= mask;
        b |= data;
        return IICwriteByte(dev, reg, b);
    } else {
        return 0;
    }
}


/**************************************************************************
Function: Read, modify, and write one bit in a byte of the specified device specified register
Input   : dev：Target device IIC address；reg：Register address；
					bitNum：To modify the bitnum bit of the target byte；data：When it is 0, the target bit will be cleared, otherwise it will be set
Output  : 1：success；0：fail
函数功能：读 修改 写 指定设备 指定寄存器一个字节 中的1个位
入口参数：dev：目标设备地址；reg：寄存器地址；bitNum：要修改目标字节的bitNum位；
					data：为0时，目标位将被清，否则将被置位
返回  值：1：成功；0：失败
**************************************************************************/ 
uint8_t IICwriteBit(uint8_t dev,uint8_t reg, uint8_t bitNum, uint8_t data){
   uint8_t b;
    IICreadByte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return IICwriteByte(dev, reg, b);
}


