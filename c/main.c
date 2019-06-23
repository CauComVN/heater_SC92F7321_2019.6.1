#include "H/Function_Init.H"
#include <math.h>
#include <stdio.h>

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

//正常运行函数
//void AppHandle();
	

void main(void)
{
	uint idata ADCTempValue=0;
	
	IO_Init();
	WDTCON |= 0x10;		    //清看门狗
	
	switch(Test)
	{
		//正常运行
		case 100:
			
		{
			//=====================初始化
	
	
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
				b_start_pid=0;
					
				heater_relay_on=1;
				Scr_Driver_Control_Heat_RLY(heater_relay_on);
				
				//启动可控硅控制
				Zero_Crossing_EX_Init();
				
				//延时1s，可控硅全功率
				BTM_Init();
			}
			
		
			if(heater_relay_on==1)
			{
				//检测温度保险 HEAT ERROR 直接检测端口值 P03   轮询方式
				Scr_Driver_Check_Insurance();
			}
		
			if(water_flow_flag < 1 && heater_relay_on==1)
			{
					heater_relay_on=0;
					Scr_Driver_Control_Heat_RLY(heater_relay_on);
			}			
			
			if(b_btm_int_flag==1)
			{			
				//出水温度
				ADC_Init(AIN8);
				ADCTempValue=ADC_Convert(); //启动ADC转换，获得转换值
				get_temp_table(ADCTempValue,&current_out_temp);
				
				//UART_SentChar(current_out_temp);
				
				printf("%d\n",current_out_temp);
				
				PIDCalc(best_temp_out, current_out_temp);
			
				b_btm_int_flag=0;
			}
			
			//串口接收到数据，处理
			Uart_Process();			
	}	
		}
			break;
		
		
		case 1: 
		{
//			//用AD_IN_WTR P16  AD_OUT_WTR P17口做调功率按键中断，出水温度 进水温度替换
//			
//			// 需要做防抖动处理（未处理）
//			EXTI_Test();
		}			
		break;
		case 2: 
			//Timer_Test();
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
//			Uart0_Test();
		}
		break;
//		case 8: IAP_Test(0x1fff,IapROM);   //操作ROM，对地址0x1fff进行读写测试
////						IAP_Test(0x7f,IapEPPROM);   //操作EPPROM，对地址0x7f进行读写测试
//		break;
		
		case 9: 
			//Serial_Key_Test();
		break;
		case 10: 
			//Zero_Crossing_EXTI_Test();
		break;
		case 11: 
			//Water_Detection_EXTI_Test();
		break;
		case 12: 
			//Water_Detection_Timer_Test();
		break;
		case 13: 
//			Leakage_EXTI_Test();
			Leakage_EX_Init();
			while(1)
			{					
			}
		break;
		case 14:
			//delay_1ms(10);
			//delay(300);
			break;
		default:	
		;			
	}
}

