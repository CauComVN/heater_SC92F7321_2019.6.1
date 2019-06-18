#include "H/Function_Init.H"
#include <math.h>

bit b_btm_int_flag=false;
uint count=0;

void BTM_Init(void);
void BTM_Test(void);

// Sv设定温度值  Pv当前温度值
int PIDCalc(int Sv,int Pv);

/*****************************************************
*函数名称：void BTM_Test(void)
*函数功能：BTM测试
*入口参数：void
*出口参数：void
*****************************************************/
void BTM_Test(void)
{
	BTM_Init();
	while(1)
	{
	}
}
/*****************************************************
*函数名称：void BTM_Init(void)
*函数功能：BTM初始化
*入口参数：void
*出口参数：void
*****************************************************/
void BTM_Init(void)
{
//	BTMCON = 0x00;  //不开启
	BTMCON = 0x80;  //每15.6ms产生一个中断
//	BTMCON = 0x81;	//每31.3ms产生一个中断
//	BTMCON = 0x82;	//每62.5ms产生一个中断
//	BTMCON = 0x83;	//每125ms产生一个中断
//	BTMCON = 0x84;	//每0.25s产生一个中断
//	BTMCON = 0x85;	//每0.5s产生一个中断
//	BTMCON = 0x86;	//每1s产生一个中断
	BTMCON = 0x87;	//每2s产生一个中断
	EA = 1;           //开启总中断
	IE1 |= 0x04;       //开启BTM中断
}

/*****************************************************
*函数名称：void BTM_Int(void) interrupt 9
*函数功能：中断函数
*入口参数：void
*出口参数：void
*****************************************************/
void BTM_Int(void) interrupt 9
{
	if(!(BTMCON&0X40))		//中断标志位判断
	{
		uint ADCTempValue=0;
		int ret=0;
		uint pidret=0;
		
//		count++;
//		if(count>=20)
//		b_btm_int_flag=true;
		
		//出水温度
			ADC_Init(AIN8);
			ADCTempValue=ADC_Convert(); //启动ADC转换，获得转换值
			ret = get_temp_table(ADCTempValue,&current_out_temp);
			
			UART_SentChar(current_out_temp);
		
			if(ret==-1) { //通知检测温度异常，超过最低温度，发送主板BEEP报警
					ex_flag=Ex_Out_Water_Temp_Low;
					
			}
			else if(ret==-2) { //通知检测温度异常，超过最高温度发送主板BEEP报警
					ex_flag=Ex_Out_Water_Temp_High;
			}
			else {
				//调节温度到一个合适的范围内
				//HEAT TRA  功率调节方式 flag 0:不用调节 1：增加功率 2：减少功率
//				if(current_out_temp<best_temp_out)
//				{
//					Scr_Driver_power_Adjust(1);
//				}
//				if(current_out_temp>best_temp_out)
//				{
//					Scr_Driver_power_Adjust(2);
//				}
				
				//0 --- 20000  42度-28度=14度 20000/14=1428.57
				
				
				//设定值大于实际值否？
				//偏差大于2为上限幅值输出(全速加热)
				if(best_temp_out-current_out_temp>2) ////偏差大于2?
				{
					scr_open_time=0;//8600;//17200;//20000;//5;//低电平 8.6ms 17200---0  高电平 10ms  20000---0
					scr_curr_time=0;
				}
				else{
					pidret=PIDCalc(best_temp_out,current_out_temp);
					
					pidret=1400*pidret;
					
					if(current_out_temp<best_temp_out)
					{
						//Scr_Driver_power_Adjust(1);
						
						scr_curr_time -= abs(pidret);
						if(scr_curr_time<1)
						{
							scr_curr_time=0;
						}
					}
					if(current_out_temp>best_temp_out)
					{
						//Scr_Driver_power_Adjust(2);
						
						scr_curr_time += abs(pidret);
						if(scr_curr_time>=(scr_open_time_max-zero_peroid_last_time))
						{
							scr_curr_time=(scr_open_time_max-zero_peroid_last_time);
						}
					}
				}				
			}
	}
}

// Sv设定温度值  Pv当前温度值
int PIDCalc(int Sv,int Pv)
{ 		
	int DERR1 = 0;       //
	int DERR2 = 0;       //

	float Pout = 0;       //比例结果
	float Iout = 0;       //积分结果
	float Dout = 0;       //微分结果
	unsigned int Out = 0; //总输出
	static unsigned int Out1;  //记录上次输出

	static int ERR;       //当前误差
	static int ERR1;      //上次误差
	static int ERR2;      //上上次误差

	/******************
	*以下四项是需要根据实际情况调试的
	******************/
	static int pidt = 0.0156;// 15.6ms;//300;     //300MS计算一次 计算周期
	static char Kp = 5;       //比例系数
	static unsigned int Ti= 5000; //积分时间
	static unsigned int Td= 600; //微分时间

//	static uint Upper_Limit= 100; //PID输出上限
//	static uint Lower_Limit= 0; //PID输出下限


//	if(pidtimer < pidt)     //计算周期   pidtimer可以用定时器计时
//		return ;  //

	ERR = Sv - Pv;   //算出当前误差
	DERR1 = ERR - ERR1;   //上次
	DERR2 = ERR - 2 * ERR1 + ERR2; //上上次

//	Pout = Kp * DERR1;    //输出P
//	Iout = (float)(ERR * ((Kp * pidt) / Ti));  //输出I
//	Dout = (float)(DERR2 * ((Kp * Td) / pidt));   //输出D
//	Out = (unsigned int)(Out1 + Pout + Iout + Dout);
	
	//先Kp
	Pout = Kp * DERR1;    //输出P
	Iout = 0;//(float)(ERR * ((Kp * pidt) / Ti));  //输出I
	Dout = 0;//(float)(DERR2 * ((Kp * Td) / pidt));   //输出D
	Out = (uint)(Out1 + Pout + Iout + Dout);
	

//	if(Out >= Upper_Limit) { //如果输出大于等于上限
//		Out = Upper_Limit;
//	} 
//	else if(Out <= Lower_Limit) { //如果输出小于等于下线
//		Out = Lower_Limit;
//	}
	Out1 = Out;      //记录这次输出的值

	ERR2 = ERR1;    //记录误差
	ERR1 = ERR;     //记录误差
//	pidtimer = 0;   //定时器清零重新计数

	return Out;
}