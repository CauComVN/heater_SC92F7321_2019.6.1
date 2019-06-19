#include "H/Function_Init.H"
#include <math.h>

/**************************************************************
说明：
1、Options for Target‘Target1’：BL51 Locate->Code Range:0x100，烧录选项请选择DISRST，复位脚作为普通IO使用；
2、改变TEST的定义，可以分别测试对应的功能；
3、注意：先在Function.H里面选择测试型号（SC92F7320无LCD/LED和PWM功能）
***************************************************************/
// Normal:100 	BTM:0 EXTI:1 		Timer:2 	LCD:3 	ScrDriverPWM:4 	Uart0:5 	ADC:7 	IAP:8 
// SerialKey:9 	ZeroCrossing:10 	WaterCheckEXTI:11 	WaterCheckTimer:12 Leakage:13 SoftDelay:14
// Leakage: 13

//#define Test  5
#define Test  100   

void AppInit();

//正常运行函数
void AppHandle();

// Sv设定温度值  Pv当前温度值
int PIDCalc(int Sv,int Pv);

void main(void)
{
	IO_Init();
	WDTCON |= 0x10;		    //清看门狗
	
	switch(Test)
	{
		//正常运行
		case 100:
			AppHandle();
			break;
		
		case 0: 
			//BTM_Test();
		break;
		case 1: 
		{
//			//用AD_IN_WTR P16  AD_OUT_WTR P17口做调功率按键中断，出水温度 进水温度替换
//			
//			// 需要做防抖动处理（未处理）
//			EXTI_Test();
		}			
		break;
		case 2: Timer_Test();
		break;
		case 3: 
//			LCD_Test();
		break;
		case 4: 
		{
//			Scr_Driver_PWM_Test();
		}
		break;
		case 5: 
		{
			Uart0_Test();
		}
		break;
		case 7: ADC_Test();
		break;
//		case 8: IAP_Test(0x1fff,IapROM);   //操作ROM，对地址0x1fff进行读写测试
////						IAP_Test(0x7f,IapEPPROM);   //操作EPPROM，对地址0x7f进行读写测试
//		break;
		
		case 9: Serial_Key_Test();
		break;
		case 10: Zero_Crossing_EXTI_Test();
		break;
		case 11: Water_Detection_EXTI_Test();
		break;
		case 12: Water_Detection_Timer_Test();
		break;
		case 13: 
			Leakage_EXTI_Test();
		break;
		case 14:
			//delay_1ms(10);
			//delay(300);
			break;
		default:	
		;			
	}
}

//正常运行函数
void AppHandle()
{
		
	//=====================初始化
	uint ADCTempValue=0;
	int ret=0;
	int pidret=0;
	
	AppInit();
	
	Uart0_Init();
	
	//漏电保护
	//Leakage_EX_Init();
	
	
	//水流检测
  Water_Detection_EX_Init(); 
	
	//水流检测定时器
  Water_Detection_Timer_Init();
	/*
	//初始化按键，用串口Rx Tx做中断来调节功率 需要做防抖动处理（未处理）
	//Serial_Key_Init();
	*/
	
		
	//=====================主循环
	/**/
	while(1)
	{
		/**/
			//test........
			//水流状态标记 0：无水流 1：少水流 2：多水流，正常
			if(water_flow_flag >= 1 && heater_relay_on==0)
			{
				heater_relay_on=1;
				Scr_Driver_Control_Heat_RLY(heater_relay_on);
				
				//启动可控硅控制
				Zero_Crossing_EX_Init();
				
				//延时1s，可控硅全功率
				BTM_Init();
			}
			
		
//			if(heater_relay_on==1)
//			{
//				//检测温度保险 HEAT ERROR 直接检测端口值 P03   轮询方式
//				Scr_Driver_Check_Insurance();
//			}
		
			if(water_flow_flag < 1 && heater_relay_on==1)
			{
					heater_relay_on=0;
					Scr_Driver_Control_Heat_RLY(heater_relay_on);
			}
			
			
			if(b_btm_int_flag==true)
			{
			
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
						
						scr_curr_time += pidret;
						if(scr_curr_time<1)
						{
							scr_curr_time=0;
						}
						if(scr_curr_time>=(scr_open_time_max-zero_peroid_last_time))
						{
							scr_curr_time=(scr_open_time_max-zero_peroid_last_time);
						}
						
//						if(current_out_temp<best_temp_out)
//						{
//							//Scr_Driver_power_Adjust(1);
//							
//							scr_curr_time -= abs(pidret);
//							if(scr_curr_time<1)
//							{
//								scr_curr_time=0;
//							}
//						}
//						if(current_out_temp>best_temp_out)
//						{
//							//Scr_Driver_power_Adjust(2);
//							
//							scr_curr_time += abs(pidret);
//							if(scr_curr_time>=(scr_open_time_max-zero_peroid_last_time))
//							{
//								scr_curr_time=(scr_open_time_max-zero_peroid_last_time);
//							}
//						}
					}				
				}
				
				
				b_btm_int_flag=false;
			}
			
			
			
			
			
			//串口接收到数据，处理
			Uart_Process();
			
	}
	
}

//全局变量初始化，避免部分全局变量值不确定，导致逻辑问题
void AppInit()
{
	//下位机热水器通信协议接收到的命令协议数据
	//uchar heater_receive_data;

	//当前热水器运行或停止状态 继电器动作 0：停止 1：运行
	heater_relay_on=0;

	//热水器内部异常状态
	ex_flag=Ex_Normal;

	//水流状态标记 0：无水流 1：少水流 2：多水流，正常
	 water_flow_flag=0;
	
	//温度预设值
	best_temp_out=37;

	current_out_temp=28; //当前出水温度
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
//	static int pidt = 0.0156;// 15.6ms;//300;     //300MS计算一次 计算周期
	static char Kp = 5;       //比例系数
//	static unsigned int Ti= 5000; //积分时间
//	static unsigned int Td= 600; //微分时间

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