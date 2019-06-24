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

//#define UART0_BUFF_LENGTH	  5	//UART0接受缓冲区的容量（Byte)


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


/******************
	*以下四项是需要根据实际情况调试的
	******************/
	//15.6msMS计算一次 计算周期
//#define pidt   0.5
//比例系数  0.01 --- 10  采样频率低（如500ms），Kp一般是0.01级别；采样频率高（如1ms），Kp一般是1级别
//kp=1
#define  Kp   5  
//积分时间 60
#define  Ti  500
//微分时间 600
#define  Td  500



//过零检测周期高电平时间和低电平时间不等
//10ms 时钟Fsys/12
#define zero_period_high_time 	20000  
//8.6ms 时钟Fsys/12
#define zero_period_low_time		17200
//过零检测周期高低电平的最后部分，软件处理成最低功率
#define zero_peroid_last_time		200


//根据仿真确认少水流阈值，参考和比较numberPulse变量值
#define waterThreshold		2 


//最佳出水温度低值
#define good_temp_out_low 28
//最佳出水温度高值
#define good_temp_out_high 50



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

//当前热水器运行或停止状态 继电器动作 0：停止 1：运行
extern volatile  bit heater_relay_on;

extern volatile  bit scr_open_flag;

//热水器内部异常状态
extern Enum_Ex_Flag idata ex_flag;

//水流状态标记 0：无水流 1：少水流 2：多水流，正常
extern volatile  int  idata  water_flow_flag;


extern int idata best_temp_out;

extern volatile bit b_start_pid;

extern volatile uchar leakage_flag;
extern volatile uchar heater_power_tune;
extern volatile int  scr_tune_time;

extern volatile  uchar current_out_temp; //当前出水温度

extern volatile uint  scr_open_time_max;
extern uint  scr_open_time;//低电平 8.6ms 17200---0  高电平 10ms  20000---0

//extern volatile  int   scr_curr_time;
//volatile
extern void IO_Init(void);

//extern void EXTI_Test(void);
//extern void EX_Init(void);

//extern void Uart0_Test(void);

extern void ADC_Init(uint Channel);
extern int ADC_Convert(void);
extern int get_temp_table(uint nADValue, uchar* temp);

//软件延时
extern void soft_delay(uint n);

extern int  Scr_Driver_Check_Insurance();//检测温度保险
extern void Scr_Driver_Control_Heat_RLY(int on);//继电器控制 HEAT RLY P02
//extern void Zero_Crossing_EXTI_Test(void);
extern void Zero_Crossing_EX_Init(void);
//extern void Water_Detection_EXTI_Test(void);
//extern void Water_Detection_Timer_Test(void);
extern void Water_Detection_EX_Init(void);
extern void Water_Detection_Timer_Init(void);
//extern void Leakage_EXTI_Test(void);

extern void Leakage_EX_Init(void);

// Sv设定温度值  Pv当前温度值
extern void PIDCalc(int Sv,int Pv);

extern void BTM_Init(void);
extern volatile   bit b_btm_int_flag;

extern void Timer_Init(void);

extern void Uart0_Init(void);
extern void Uart_Process();
//extern void UART_SentChar(uchar chr);
extern void UART_SendString(uchar *str);

//中断
extern void ADC_Interrupt_Handle(void);
extern void Water_Detection_EX2_Handle();
extern void Water_Detection_Timer0_Handle();
extern void Zero_Crossing_EX2_Handle();
extern void EX1_Handle();
extern void UartInt_Handle();
extern void Timer1Int_Handle();
extern void Leakage_EX0_Handle(void);

#endif