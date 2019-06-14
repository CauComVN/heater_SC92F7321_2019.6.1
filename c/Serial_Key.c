#include "Function_Init.H"

void Serial_Key_Init(void);
void Serial_Key_Test(void);
void Serial_Key_EX0_Handle();

void Serial_Key_Test(void)
{
		Serial_Key_Init();
    while(1)
    {        
    }
}

void Serial_Key_Init(void)
{
		//用串口P12 P13模拟控制功率 下降沿触发
		////////////////////////////////////////////////////////////
		//配置中断口INT02 INT03 P12 P13 下降沿
		P1CON &= 0XF3;     //中断IO口设置为高阻输入
		P1PH  |= 0x0c;     //中断IO口设置为高阻带上拉	 
		
			//下降沿设置	
		INT0F |= 0x0c;   //xxxx 0000  0关闭 1使能
			//上升沿设置	
		INT0R &= 0Xf3 ;    //xxxx 0000  0关闭 1使能 
		
		//外部中断优先级设置
		IE  |= 0x01;	//0000 0x0x  INT0使能
		IP  |= 0X00;
		
		EA = 1;
		//////////////////////////////////////////////////////////		
}

void Serial_Key_EX0_Handle()
{	
	BEE = ~BEE;
	
	//HEAT TRA PWM1 功率调节方式 flag 0:不用调节 1：增加功率 Duty增大 2：减少功率 Duty减少	
	//if(RX1==0) //增加功率
	{
//		Scr_Driver_PWM_Adjust(1);		
	}
//	if(TX1==0) //降低功率
//	{
//		Scr_Driver_PWM_Adjust(2);
//	}
}