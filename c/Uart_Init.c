#include "H/Function_Init.H" 
#include <stdio.h>
#include <string.h>

bit UartSendFlag = 0; //发送中断标志位
bit UartReceiveFlag = 0; //接收中断标志位

uchar idata Uart0RecvBuffNum=0;
char idata Uart0SendBuff[UART0_BUFF_LENGTH];
char idata Uart0RecvBuff[UART0_BUFF_LENGTH];

int CalCrc(int crc, const char *buf, int len);
void init_heater();
void start_heater();
void stop_heater();

void Uart0_Init(void);
void UartInt_Handle();
void Uart_Process();
//void UART_SentChar(uchar chr);
//void UART_SendString(uchar *str);

char putchar(char c)//用于重写printf
{
	SBUF = c;
	while(!UartSendFlag);
	UartSendFlag=0;
	return c;
}

void Uart_Process()
{	
	if(UartReceiveFlag)
	{
		
//		unsigned char string[6]={"1234\n"};
//	unsigned int n;
//	sscanf(string,"%u",&n);//string是字符串，%u是格式控制串，u是无符号十进制数，&n是变量n的地址
//	printf("%d\n",n);//可在KEIL C 开发环境中输出观察
		
		UartReceiveFlag=0;
		
		BEE = ~BEE;
	
		if(SBUF == 0x01)
		{				
			best_temp_out=best_temp_out+1;
			if(best_temp_out>good_temp_out_high)
			{
				best_temp_out=good_temp_out_high;
			}
			
			//软件延时，保证变量更新完成，避免主循环逻辑错误或者混乱
			soft_delay(48000); 
			
			SBUF = 0x55+SBUF;
			while(!UartSendFlag);
			UartSendFlag = 0;
		}
		if(SBUF == 0x02)
		{				
			best_temp_out=best_temp_out-1;
			if(best_temp_out<good_temp_out_low)
			{
				best_temp_out=good_temp_out_low;
			}
			
			//软件延时，保证变量更新完成，避免主循环逻辑错误或者混乱
			soft_delay(48000); 
			
			SBUF = 0x55+SBUF;
			while(!UartSendFlag);
			UartSendFlag = 0;
		}	
		
		if(SBUF == 0x03)
		{	
			start_heater();			
		}
		
		//关闭加热开关
		if(SBUF == 0x04)
		{
			stop_heater();
		}
		
		//发送故障信息给上位机
		if(ex_flag!=Ex_Normal)
		{
			int idata crc=0;
				
			//停止热水器
			stop_heater();
			
			//发送异常协议给上位机  当前温度值+故障字+CSC校验
			Uart0SendBuff[0]=current_out_temp;
			Uart0SendBuff[1]=ex_flag;
			crc = CalCrc(0x00, Uart0SendBuff, 2);//计算得到的16位CRC校验码
			Uart0SendBuff[2] = (char)(crc >> 8);//取校验码的高八位
			Uart0SendBuff[3] = (char)crc;//取校验码的低八位
			Uart0SendBuff[4] = '\0';
			printf(Uart0SendBuff);
		}
	}
}

void Uart0_Init(void)    //选择Timer2作为波特率信号发生器
{
	P1CON &= 0XF3;
	P1PH |= 0X0C;	 //TX为强推输出，RX为带上拉输入；
	SCON  = 0X50;    //设置通信方式为模式一，允许接收
	PCON |= 0X80; 
	TMCON |= 0X04;
	T2MOD = 0X00;
	T2CON = 0X30;
	RCAP2H = 0xFF;
	RCAP2L = 0xec;//0xB2; //38400
	TR2 = 0;
	ET2 = 0;
	TR2 = 1;
	EUART = 1;     //开启Uart0中断
//	EA = 1;
}


/*****************************************************
*函数名称：void UartInt(void) interrupt 4
*函数功能：Uart0中断服务函数
*入口参数：void
*出口参数：void
*****************************************************/
void UartInt_Handle()
{
	if(TI)
	{
		TI = 0;	
		UartSendFlag = 1;		
	}
	if(RI)
	{
		RI = 0;	
		UartReceiveFlag = 1;	

		Uart0RecvBuff[Uart0RecvBuffNum] = SBUF; //将接收的数据存入缓冲区
		Uart0RecvBuffNum++;
	}	
}

//void UART_SentChar(uchar chr)
//{
//  //发送一个字节
////  SBUF = chr;
////  while( TI == 0);
////  TI = 0;
//	
//	SBUF = chr;
//	while(!UartSendFlag);
//	UartSendFlag = 0;
//}

//void UART_SendString(uchar *str)
//{
//  while(*str != '\0')
//  {
//      UART_SentChar(*str++);
//  }
//}

//CRC校验和有16位的，也有32位的。CRC校验和算法原理：
//CRC校验和就是将一段二进制数据进行加密（乘以一个多项式），
//然后得到一个校验码。将这个校验码添加在这段二进制数据后边就行了。
//然后接收方在接收到数据之后，再对这个校验码进行解码。
int CalCrc(int crc, const char *buf, int len)
{
    unsigned int idata byte;
    unsigned char idata k;
    unsigned short idata ACC,TOPBIT;
//    unsigned short idata remainder = 0x0000;
    unsigned short idata remainder = crc;
    TOPBIT = 0x8000;
    for (byte = 0; byte < len; ++byte)
    {
        ACC = buf[byte];
        remainder ^= (ACC <<8);
        for (k = 8; k > 0; --k)
        {
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^0x8005;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
    remainder=remainder^0x0000;
    return remainder;
}

void init_heater()
{
	//温度采集ADC转换标记
		AdcFlag = 0;
	
		// 0:无功率 1：全功率
		heater_power_status=0; 
		//当前热水器运行或停止状态 控制继电器动作 0：停止 1：运行
		heater_relay_on=0;
		//开始PID算法标记
		b_start_pid=0;		
		////热水器内部异常状态
		ex_flag=Ex_Normal;
		//35度~60度 自动调节  最佳：40 - 50
		best_temp_out=38;
		//当前出水温度
		current_out_temp=29;
		//可控硅触发时间最大值 
		scr_open_time_max=zero_period_low_time;
		//实时计算的可控硅触发时间
		scr_curr_time=0;
		//实时计算的可控硅触发时间副本，用于解决主循环和过零检测中断内全局变量scr_curr_time问题
		scr_tune_time=0;

		//可控硅触发时间 低电平 8.6ms 17200---0  高电平 10ms  20000---0
		scr_open_time=0;
		//可控硅开通标志 用于标记可控硅触发，关断定时器，关断可控硅
		scr_open_flag=0;
	
		//采集温度定时器中断标记 基础定时器中断，采集出水温度值，采集周期和处理周期0.5s	
		b_btm_int_flag=0;		
	
		//水流检测霍尔计数器计数中断
		numberPulse = 0;
		//水流检测状态标记 0：无水流 1：少水流 2：多水流，正常
		water_flow_flag=0;		
}

void start_heater()
{
		//热水器初始化
		init_heater();
	
    //水流检测计数中断
    Water_Detection_EX_Init();

    //水流检测定时器中断
    Water_Detection_Timer_Init();
	
		soft_delay(50000); // (1+1+(1+2)*50000)*0.5us=75001us=75.001ms
	
		//水流状态标记 0：无水流 1：少水流 2：多水流，正常
		if(water_flow_flag == 2 && heater_relay_on==0)
		{			
			heater_relay_on=1;
			Scr_Driver_Control_Heat_RLY(heater_relay_on);				
			
			//过零检测中断
			Zero_Crossing_EX_Init();
			
			//基础定时器中断，采集出水温度值，采集周期和处理周期0.5s
      BTM_Init();
		}
}

void stop_heater()
{	
	//关闭水流霍尔传感器外部计数中断 INT25 P21 
	//关闭过零检测外部中断 INT24 P20 ZERO
	//基础定时器Base Timer中断不使能 出水温度采集定时器
	//关闭PWM中断
	IE1 &= 0xf1;	
	
	//关闭水流检测定时器
	TR0 = 0;//定时器0停止计数
  ET0 = 0;//定时器0不允许
	
	//关闭可控硅触发定时器	
	TR1 = 0;//定时器1停止计数
  ET1 = 0;//定时器1不允许
	
	//关闭出水温度采集ADC转换中断
	EADC=0; //ADC中断不使能
	ADCCON &= 0x08;//关闭ADC模块电源
	
	//热水器初始化
	init_heater();
}
