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
extern uint good_temp_out_low;
//最佳出水温度高值
extern uint good_temp_out_high;

extern void IO_Init(void);
extern void BTM_Test(void);
extern void EXTI_Test(void);
extern void Timer_Test(void);
extern void LCD_Test(void);
extern void PWM_Test(void);
extern void Uart0_Test(void);
extern void ADC_Test(void);
extern void IAP_Test(uint Address,uchar IAPArea);

extern void PWM_Init(void);
extern int get_temp_table(uint nADValue, int* temp);

void Serial_Key_Init(void);
void Serial_Key_Test(void);

#endif