#include "H/Function_Init.H"

void Scr_Driver_PWM_Init(void);
void Scr_Driver_PWMInt_Handle();
/*****************************************************
*函数名称：void PWM_Test(void)
*函数功能：PWM测试
*入口参数：void
*出口参数：void
*****************************************************/
void PWM_Test(void)
{
	Scr_Driver_PWM_Init();
	while(1)
	{
	}
}
/*****************************************************
*函数名称：void PWM_Init(void)
*函数功能：PWM初始化
*入口参数：void
*出口参数：void
*****************************************************/
void Scr_Driver_PWM_Init(void)
{
	//HEAT TRA PWM1
	
	//50hz 20ms 半个周期10ms 10ms/10=1ms PWM时钟为Fsys/128 187*128/24=997.33 187-1=186
	PWMCON  = 0x16;		//PWM输出到IO，PWM时钟为Fsys/128 HEAT TRA PWM1
	PWMPRD  = 186;		//PWM周期=(186+1)*(1*128/24us)=997.33≈=1ms;
	PWMCFG  = 0x10;		//PWM1输出反向,输出至P01
	PWMDTY1 = 15;     //PWM1的Duty = 15/60 =1/4
	PWMDTYA = 0x00;		//PWM占空比微调寄存器，这里不微调
	PWMCON |= 0x80;     //开启PWM
	IE1 |= 0x02;        //开启PWM中断
	EA = 1;
}

//中断处理
void Scr_Driver_PWMInt_Handle()
{
    //根据出水/进水温度自动调节PWM1的Duty
	int i=0;
}