#include "H/Function_Init.H"

volatile uint   scr_open_time_max=zero_period_high_time;
uint  scr_open_time=0;//17200;//20000;//5;//低电平 8.6ms 17200---0  高电平 10ms  20000---0
volatile bit scr_open_flag=0;//可控硅开通标志 用于关断定时器 关断可控硅

void Timer_Init(void);

/*****************************************************
*函数名称：void Timer_Init(void)
*函数功能：T0/T1/T2初始化
*入口参数：void
*出口参数：void
*****************************************************/
void Timer_Init(void)
{
	TMCON &= 0xfd;   //时钟Fsys/12
	TMOD &=0x9f; 
	TMOD |=0x10;
	
	
	//关闭可控硅 设置可控硅开通标记
	HEAT_TRA=0;
	scr_open_flag=0;
		
	TL1 = (65536 - scr_open_time)%256;     //溢出时间：时钟为Fsys/12，则scr_open_time*（1/(Fsys/12)）=scr_open_time*0.5us;
	TH1 = (65536 - scr_open_time)/256;
	TR1 = 0;
	ET1 = 1;//定时器0允许
	TR1 = 1;//打开定时器0
	
	
//     EA = 1;
	
}


//void Timer_Init(void)
//{
//	TMCON &= 0xfd;   //时钟Fsys/12
//	TMOD &=0x9f; 
//	TMOD |=0x10;
//	
//	
//	//关闭可控硅 设置可控硅开通标记
//	HEAT_TRA=0;
//	
//	scr_open_flag=0;
//		
////	TL1 = (65536 - scr_open_time)%256;     //溢出时间：时钟为Fsys/12，则scr_open_time*（1/(Fsys/12)）=scr_open_time*0.5us;
////	TH1 = (65536 - scr_open_time)/256;
//	TR1 = 0;
//	ET1 = 1;//定时器0允许
//	//TR1 = 1;//打开定时器0
//	
//	
////     EA = 1;
//}

void Timer1Int_Handle()
{
		if(scr_open_flag==0)
		{
			
			scr_open_flag=1;	

			//768us
			TL1 = 0;// (65536 - 1536)%256;     //溢出时间：时钟为Fsys/12，则scr_open_time*（1/(Fsys/12)）=scr_open_time*0.5us;
			TH1 = 250;//(65536 - 1536)/256;
						
			if(HEAT_TRA!=1)
					HEAT_TRA=1;
		}
		else
		{
			HEAT_TRA=0;
			TR2 = 0;
			scr_open_flag=0;
		}
}

