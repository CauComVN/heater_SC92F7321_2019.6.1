#include "H/Function_Init.H"

uint time2_count=0; //9ms/1ms=9
uint time2_count_max=44;//45; // 9000/100=90 每等分100us 定时100us中断一次 9000/200=45
uint time2_curr =44;//4;//0; //初始化不启动

void Timer_Init(void);
/*****************************************************
*函数名称：void Timer_Test(void);
*函数功能：T0/T1/T2测试
*入口参数：void
*出口参数：void
*****************************************************/
void Timer_Test(void)
{
	Timer_Init();
	while(1)
	{
	}
}
/*****************************************************
*函数名称：void Timer_Init(void)
*函数功能：T0/T1/T2初始化
*入口参数：void
*出口参数：void
*****************************************************/
void Timer_Init(void)
{ 
	TMCON = TMCON|0X04;    //------100 ;Tiemr2选择时钟Fsys
	
//	//T0设置
//	TMOD |= 0x01;                 //0000 0001;Timer0设置工作方式1
//	TL0 = (65536 - 24000)%256;    //溢出时间：时钟为Fsys，则24000*（1/Fsys）=1ms;
//	TH0 = (65536 - 24000)/256;
//	TR0 = 0;
//	ET0 = 1;//定时器0允许
//	TR0 = 1;//打开定时器0
//	//T1设置
//	TMOD |= 0x20;            //0010 0000;Timer1设置工作方式2
//	TL1 = 256 - 240;   //溢出时间：时钟为Fsys，则240*（1/Fsys）=10us;
//	TH1 = 256 - 240;
//	TR1 = 0;
//	ET1 = 1;//定时器1允许
//	TR1 = 1;//打开定时器1
	//T2设置
	T2MOD = 0x00;
	T2CON = 0x00;	 //设置为16位重载寄存器
	
//	RCAP2H = (65536-48000)/256;     //溢出时间：时钟为Fsys，则48000*（1/Fsys）=2ms;
//	RCAP2L = (65536-48000)%256;
//	RCAP2H = (65536-24000)/256;     //溢出时间：时钟为Fsys，则24000*（1/Fsys）=1ms;
//	RCAP2L = (65536-24000)%256;
//	RCAP2H = (65536-2400)/256;     //溢出时间：时钟为Fsys，则2400*（1/Fsys）=100us;
//	RCAP2L = (65536-2400)%256;    
	RCAP2H = (65536-4800)/256;     //溢出时间：时钟为Fsys，则4800*（1/Fsys）=200us;
	RCAP2L = (65536-4800)%256;    
	
	TR2 = 0;
	ET2 = 1;//定时器2允许
	TR2 = 1;//打开定时器2		
	
//	P01=1;
	time2_count=0;
//	if(time2_curr==0)
//	{
//		P01=0;
//	}	
	
	P01=0;
	
	/*
	//Timer2捕获功能
	T2CON = 0X09; //使能EXT2，16位捕获模式
	T2MOD = 0X00;
	TH2 = 0X00;
	TL2 = 0X00;
	RCAP2H = 0X00;
	RCAP2L = 0X00;
	TR2 = 0;
	ET2 = 1;//定时器2允许
	TR2 = 1;//打开定时器2	
	*/
	
	EA = 1;	
}

/**************************************************
*函数名称：void timer0/1/2() interrupt 1/3/5
*函数功能：定时器中断产生方波
*入口参数：void
*出口参数：void
**************************************************/
//void timer0() interrupt 1
//{
//    TL0 = (65536 - 24000)%256;
//	TH0 = (65536 - 24000)/256;
//	P20 = ~P20;	
//}

//void timer1() interrupt 3
//{
//	P21 = ~P21;
//}

//void Timer2Int(void) interrupt 5
//{		
//	TF2 = 0;   //溢出清零
//    P26 = ~P26;
//}

void Timer2Int_Handle()
{
	TF2 = 0;   //溢出清零	
	
	time2_count=time2_count+1;
	
//	if((time2_count<time2_curr && time2_curr!=0) || time2_curr == 0 )
//	{
//		if(P01!=0)
//		{
//			P01=0;
//		}
//	}
//	else if(time2_count==time2_curr)
//	{
//		if(P01!=1)
//		{
//			P01=1;
//			
//			TR2 = 0;
//		}
//	}
//	else
//	{
//		if(P01!=0)
//		{
//			P01=0;
//		}
//	}
//	
//	if(time2_count>time2_count_max)
//	{
//		//TR2 = 0;
//		//P01=0;
//		
//		time2_count=0;
//	}
//	


	if(time2_count<time2_curr)
	{
		if(P01!=0)
		{
			P01=0;
		}
	}
	else if(time2_count==time2_curr)
	{
		if(P01!=1)
		{
			P01=1;
			
//			TR2 = 0;
		}
	}
	else
	{
		if(P01!=0)
		{
			P01=0;
			
			TR2 = 0;
		}
	}
}
