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

#define UART0_BUFF_LENGTH	  4	//UART0接受缓冲区的容量（Byte)


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
////31.3msMS计算一次 采集周期 计算周期
////比例系数  0.01 --- 10  采样频率低（如500ms），Kp一般是0.01级别；采样频率高（如1ms），Kp一般是1级别
////功率一半，得温度37  29-37 8 20000/8=2500 17200/8=2150
//#define  Kp   2400  
////积分时间 Ki一般是比Kd还要小0.01个级别
//#define  Ti  100 
////微分时间 Kp/Kd=2 比例I/微分D=2，一般还要更大（10倍左右）
//#define  Td  0

//8位单片机不支持浮点运算，提高控制精度，乘10处理，这里除以10
//370-290=80 10000/80=125 8600/80=107
//#define  Kp   240 
#define  Kp   116 
#define  Ti   10
#define  Td   24

//过零检测周期高电平时间和低电平时间不等
//10ms 时钟Fsys/12
#define zero_period_high_time 	20000  
//8.6ms 时钟Fsys/12
#define zero_period_low_time		17200
//过零检测周期高低电平的最后部分，软件处理成最低功率
#define zero_peroid_last_time		200

//根据仿真确认少水流阈值，参考和比较numberPulse变量值
#define waterThreshold		2 

//8位单片机不支持浮点运算，提高控制精度，乘10处理
//最佳出水温度低值
#define good_temp_out_low 290
//最佳出水温度高值
#define good_temp_out_high 500

/***********ADC采样口选择*************/	
#if (IC_MODEL == SC92F7321)
	enum Channel {AIN0=0,AIN1,AIN2,AIN3,AIN8=8,AIN9,VDD4=15};
#endif

/***********串口协议，Mod-Bus方式*************/		
//	crc：校验和 
//	主发报文：开/关/设定温度增1/设定温度减1/用设定温度值(1)+设定温度T(1)+crc(2)
//	从发报文：实际温度T(1)+故障字(1)+crc(2)
	
//下位机热水器通信协议
enum Protocol_Heater 
{
	Protocol_Heater_Default						= 0x00,			//默认状态
	//0x01 0x24 0x86 0xdb
	Protocol_Heater_Start						  = 0x01, 		//启动热水器
	//0x02 0x24 0x8c 0xdb
	Protocol_Heater_Stop  						= 0x02, 		//停止热水器
	//0x03 0x24 0x0a 0xd8
	Protocol_Heater_Inc_Power				  = 0x03,			//热水器增加功率
	//0x04 0x24 0x98 0xdb
	Protocol_Heater_Red_Power	  		  = 0x04,			//热水器减少功率
	//0x05 0x24 0x1e 0xd8
	Protocol_Heater_Reset_Temp				= 0x05,			//热水器温度设定
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
		Ex_Uart_Crc_Error,
}Enum_Ex_Flag;

//温度采集ADC转换标记
extern volatile bit AdcFlag;

//可控硅触发时间 低电平 8.6ms 17200---0  高电平 10ms  20000---0
extern uint scr_open_time;
//可控硅开通标志 用于标记可控硅触发，关断定时器，关断可控硅
extern bit scr_open_flag;

//抗饱和积分 
//8位单片机不支持浮点运算，提高控制精度，乘10处理
extern int idata umax;//=500;//=50;//理想最大温度值
extern int idata umin;//=290;//=29;//理想最小温度值	

extern int idata Out1;  		//记录上次输出
extern int idata ERR;       //当前误差
extern int idata ERR1;      //上次误差
extern int idata ERR2;      //上上次误差	
extern int idata integral;		//积分分离
// 0:无功率 1：全功率
extern volatile uchar heater_power_status; 
//当前热水器运行或停止状态 控制继电器动作 0：停止 1：运行
extern volatile bit heater_relay_on;
//开始PID算法标记
extern volatile bit b_start_pid;
////热水器内部异常状态
extern Enum_Ex_Flag idata ex_flag;
//35度~60度 自动调节  最佳：40 - 50
extern int idata best_temp_out;
//当前出水温度
extern volatile char  current_out_temp; 
//可控硅触发时间最大值 
extern uint idata scr_open_time_max;
//实时计算的可控硅触发时间
extern volatile int  scr_curr_time;
//实时计算的可控硅触发时间副本，用于解决主循环和过零检测中断内全局变量scr_curr_time问题
extern volatile int  scr_tune_time;

//记录水流脉冲触发中断个数
extern volatile uint idata  numberPulse;
//水流状态标记 0：无水流 1：少水流 2：多水流，正常
extern volatile uchar idata water_flow_flag;

//采集温度定时器中断标记 基础定时器中断，采集出水温度值，采集周期和处理周期0.5s	
extern volatile bit b_btm_int_flag;


extern void IO_Init(void);

extern void ADC_Init(uint Channel);
extern int ADC_Convert(void);
extern int get_temp_table(uint nADValue, uchar* temp);

//软件延时
extern void soft_delay(uint n);

extern int  Scr_Driver_Check_Insurance();//检测温度保险
extern void Scr_Driver_Control_Heat_RLY(int on);//继电器控制 HEAT RLY P02
extern void Zero_Crossing_EX_Init(void);

extern void Water_Detection_EX_Init(void);
extern void Water_Detection_Timer_Init(void);

extern void Leakage_EX_Init(void);

// Sv设定温度值  Pv当前温度值
extern void PIDCalc(int Sv,int Pv);

extern void BTM_Init(void);

extern void Timer_Init(void);

extern void Uart0_Init(void);
extern void Uart_Process();
extern void UART_SendChar(uchar chr);
//extern void UART_SendString(uchar *str);

extern void start_heater();
extern void stop_heater();

//中断
extern void ADC_Interrupt_Handle(void);
extern void Water_Detection_EX2_Handle();
extern void Water_Detection_Timer0_Handle();
extern void Zero_Crossing_EX2_Handle();
extern void UartInt_Handle();
extern void Timer1Int_Handle();
extern void Leakage_EX0_Handle(void);

#endif