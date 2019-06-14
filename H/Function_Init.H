#ifndef _Function_Init_H_
#define _Function_Init_H_

#include "SC92F732x_C.H"
#include "intrins.H"
#define uchar unsigned char
#define uint unsigned int

#define IapROM  0x00	  //0x00：选择ROM区操作  0x02：选择EEPROM区操作
#define IapEPPROM  0x02	  //0x00：选择ROM区操作  0x02：选择EEPROM区操作

/*****************************************
说明：选择测试IC
*****************************************/
#define SC92F7323  0
#define SC92F7322  1
#define SC92F7321  2
#define SC92F7320  3

#define IC_MODEL  SC92F7321



//定义MCU管脚
#define 		BEE            P00 
#define 		HEAT_TRA       P01	
#define 		HEAT_RLY       P02	
#define 		HEAT_ERROR     P03	

//INT00
#define		OS	  P10	

// P12/INT02/T0/RX/TCK
#define RX1  P12 

// P13/INT03/T1/TX/TDI0
#define TX1  P13 

//P16/AIN9
#define AD_IN_WTR  P16 

//P17/AIN8
#define AD_OUT_WTR  P17 


//INT24
#define 	ZERO  P20		
//INT25
#define HALL_LLJ  P21			

#define LED_WORK	P24			

//=============================================

// 可以设置的最大最小值
#define ref_max  39 //0~39
#define ref_min  0

//可控硅开通时间  200us
#define open_max  49  //最大功率
#define open_min  40; //最小功率

//设置最大值
//time2_count_max=open_max-time2_curr/4;
//============================================



/***********ADC采样口选择*************/
#if (IC_MODEL == SC92F7323)
	enum Channel {AIN0=0,AIN1,AIN2,AIN3,AIN4,AIN5,AIN6,AIN7,AIN8,AIN9,VDD4=15};
#endif

#if (IC_MODEL == SC92F7322)
	enum Channel {AIN0=0,AIN1,AIN4=4,AIN5,AIN6,AIN7,AIN8,AIN9,VDD4=15};
#endif
	
#if (IC_MODEL == SC92F7321)
	enum Channel {AIN0=0,AIN1,AIN2,AIN3,AIN8=8,AIN9,VDD4=15};
#endif
	
#if (IC_MODEL == SC92F7320)
	enum Channel {AIN0=0,AIN1,AIN6=6,AIN7,VDD4=15};
#endif


//下位机热水器通信协议
enum Protocol_Heater 
{
	Protocol_Heater_Default						= 0x00,			//初始值
	Protocol_Heater_Start						  = 0x01, 		//启动热水器
	Protocol_Heater_Stop  						= 0x02, 		//停止热水器
	Protocol_Heater_Increases_Power   = 0x03,			//热水器增加功率
	Protocol_Heater_Reduce_Power			= 0x04,			//热水器减少功率 
};

//热水器内部异常状态
typedef enum Heater_Exception
{
    Ex_Normal=0,
    Ex_Leakage,
    Ex_In_Water_Temp_Low,
    Ex_In_Water_Temp_High,
    Ex_Out_Water_Temp_Low,
    Ex_Out_Water_Temp_High,
		Ex_Water_No_Flow,
		Ex_Water_Little_Flow,
		Ex_Thermal_Switch_Error,
} Enum_Ex_Flag;

//下位机热水器通信协议接收到的命令协议数据
extern uchar heater_receive_data;

//当前热水器运行或停止状态 继电器动作 0：停止 1：运行
extern bit heater_relay_on;

//热水器内部异常状态
extern Enum_Ex_Flag ex_flag;

//水流状态标记 0：无水流 1：少水流 2：多水流，正常
extern int water_flow_flag;

//最佳出水温度低值
extern int good_temp_out_low;
//最佳出水温度高值
extern int good_temp_out_high;
extern int best_temp_out;

extern int current_out_temp; //当前出水温度
extern uint current_PWMDTY1; //当前功率调节PWM占空比

extern uint duty_app_time; //9ms
extern uint max_pwm_duty; //10ms 50Hz市电半个周期内9ms内最大PWM周期数目
extern uint step_app_duty; //0 -- 180 调节步长
extern uint curr_app_duty; //0 -- 180
extern uint max_app_duty; //0 -- 180
extern uint low_pwm_duty_cout; //low_pwm_duty_cout个PWM周期数目为低电平
extern uint curr_low_pwm_duty_count;

extern uint time2_count;
extern int time2_curr;
extern uint time2_count_max;

extern void IO_Init(void);
//extern void BTM_Test(void);
//extern void EXTI_Test(void);
extern void Timer_Test(void);
//extern void LCD_Test(void);
//extern void EX_Init(void);

extern void Uart0_Test(void);
extern void ADC_Test(void);
extern void IAP_Test(uint Address,uchar IAPArea);

extern void ADC_Init(uint Channel);
extern int ADC_Convert(void);
extern void ADC_channel(unsigned char channel);
extern int get_temp_table(uint nADValue, int* temp);

//软件延时
extern void soft_delay(uint n);

//extern void Scr_Driver_PWM_Test(void);
//extern void Scr_Driver_PWM_Init(void);
extern int  Scr_Driver_Check_Insurance();//检测温度保险
extern void Scr_Driver_Control_Heat_RLY(int on);//继电器控制 HEAT RLY P02
extern void Zero_Crossing_EXTI_Test(void);
extern void Zero_Crossing_EX_Init(void);
extern void Water_Detection_EXTI_Test(void);
extern void Water_Detection_Timer_Test(void);
extern void Water_Detection_EX_Init(void);
extern void Water_Detection_Timer_Init(void);
extern void Leakage_EXTI_Test(void);

extern void Leakage_EX_Init(void);

//HEAT TRA PWM1 功率调节方式 flag 0:不用调节 1：增加功率 Duty增大 2：减少功率 Duty减少
extern void Scr_Driver_PWM_Adjust(uint flag);

//HEAT TRA  功率调节方式 flag 0:不用调节 1：增加功率 2：减少功率
extern void Scr_Driver_Time2_Adjust(uint flag);

extern void Timer_Init(void);

extern void Uart0_Init(void);
extern void Uart_Process();
extern void UART_SentChar(uchar chr);

//中断
extern void ADC_Interrupt_Handle(void);
extern void Water_Detection_EX2_Handle();
extern void Water_Detection_Timer0_Handle();
extern void Zero_Crossing_EX2_Handle();
//extern void Scr_Driver_PWMInt_Handle();
extern void Serial_Key_EX0_Handle();
extern void EX1_Handle();
extern void UartInt_Handle();
extern void Timer2Int_Handle();
extern void Leakage_EX0_Handle(void);

extern void Serial_Key_Init(void);
extern void Serial_Key_Test(void);

#endif