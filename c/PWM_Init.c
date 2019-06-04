#include "H/Function_Init.H"

uint current_PWMDTY1=0;//6;//15; //当前功率调节PWM占空比 x/186

uint duty_app_time=9000; //9ms
uint max_pwm_duty=9; //10ms 50Hz市电半个周期内9ms内最大PWM周期数目 duty_app_time*24/128/180=9000*24/128/180=9.375
uint step_app_duty=2; //0 -- 180 调节步长
uint curr_app_duty=30; //0 -- 180
uint max_app_duty=180; //0 -- 180
uint low_pwm_duty_cout=0; //m_duty个PWM周期数目为低电平
uint curr_low_pwm_duty_count=0;

//HEAT TRA PWM1 功率调节方式 flag 0:不用调节 1：增加功率 Duty增大 2：减少功率 Duty减少
void Scr_Driver_PWM_Adjust(uint flag);

void Scr_Driver_PWM_Init(void);
void Scr_Driver_PWMInt_Handle();
/*****************************************************
*函数名称：void Scr_Driver_PWM_Test(void)
*函数功能：PWM测试
*入口参数：void
*出口参数：void
*****************************************************/
void Scr_Driver_PWM_Test(void)
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
	
	current_PWMDTY1=0;
	
	curr_low_pwm_duty_count=0;
	low_pwm_duty_cout=(duty_app_time-50*curr_app_duty)*24/10240; // 128*180=10240
	
	//=========================================================================================
	
	//50hz 20ms 半个周期10ms 10ms/10=1ms PWM时钟为Fsys/128 187*128/24=997.33 187-1=186
	PWMCON  = 0x16;		//PWM输出到IO，PWM时钟为Fsys/128 HEAT TRA PWM1
	PWMPRD  = 180;		//PWM周期=(186+1)*(1*128/24us)=997.33≈=1ms;
	PWMCFG  = 0x10;//0x10;//0x00;		//PWM1输出正向,输出至P01  INV 1:输出反向 0：输出不反向 000x
	PWMDTY1 = current_PWMDTY1;//15;     //PWM1的Duty = 15/60 =1/4
	PWMDTYA = 0x00;		//PWM占空比微调寄存器，这里不微调
	PWMCON |= 0x80;     //开启PWM
	IE1 |= 0x02;        //开启PWM中断
	EA = 1;
}

////HEAT TRA PWM1 功率调节方式 flag 0:不用调节 1：增加功率 Duty增大 2：减少功率 Duty减少
//void Scr_Driver_PWM_Adjust(uint flag)
//{
//	if(flag==1 || flag==2)
//	{
//		EA=0;
//		IE1 &= 0xfd;        //关闭PWM中断		
//		
//		if(flag==1){ //增加功率
//			current_PWMDTY1=current_PWMDTY1+10;
//			if(current_PWMDTY1>185)
//			{
//				current_PWMDTY1=185;
//			}
//		}
//		else if(flag==2)
//		{
//			current_PWMDTY1=current_PWMDTY1-10;
//			if(current_PWMDTY1<1)
//			{				
//				current_PWMDTY1=1;
//			}
//		}			
//		
//		IE1 |= 0x02;        //开启PWM中断
//		EA=1;
//	}
//}

////中断处理
//void Scr_Driver_PWMInt_Handle()
//{
//    //根据出水/进水温度自动调节PWM1的Duty
//	int i=0;
//	
//	PWMDTY1 = current_PWMDTY1;
//}


//HEAT TRA PWM1 功率调节方式 flag 0:不用调节 1：增加功率 Duty增大 2：减少功率 Duty减少
void Scr_Driver_PWM_Adjust(uint flag)
{
	if(flag==1 || flag==2)
	{
		EA=0;
		IE1 &= 0xfd;        //关闭PWM中断		
		
		if(flag==1){ //增加功率
			curr_app_duty=curr_app_duty+step_app_duty;
			if(curr_app_duty>max_app_duty)
			{
				curr_app_duty=max_app_duty;
			}
			
			low_pwm_duty_cout=(duty_app_time-50*curr_app_duty)*24/10240; // 128*180=10240
		}
		else if(flag==2)
		{
			curr_app_duty=curr_app_duty-step_app_duty;
			if(curr_app_duty<1)
			{				
				curr_app_duty=0;
			}
			
			low_pwm_duty_cout=(duty_app_time-50*curr_app_duty)*24/10240; // 128*180=10240
		}			
		
		IE1 |= 0x02;        //开启PWM中断
		EA=1;
	}
}

//中断处理
void Scr_Driver_PWMInt_Handle()
{
	curr_low_pwm_duty_count++;
	if(curr_low_pwm_duty_count<=low_pwm_duty_cout)
	{	
		//pwm占空比0，低电平
		current_PWMDTY1=0;
	}
	else
	{
		//pwm占空比为180，高电平
		current_PWMDTY1=180;
	}
	
	if(current_PWMDTY1>max_pwm_duty)
	{
		//关闭pwm中断，等待下一次过零检测到来
		IE1 &= 0xfd;        //关闭PWM中断	
	}	
}