#include "H/Function_Init.H"
#include <math.h>
#include <stdio.h>

/**************************************************************
说明：
1、Options for Target‘Target1’：BL51 Locate->Code Range:0x100，烧录选项请选择DISRST，复位脚作为普通IO使用；
2、改变TEST的定义，可以分别测试对应的功能；
3、注意：先在Function.H里面选择测试型号（SC92F7320无LCD/LED和PWM功能）
***************************************************************/

void main(void)
{
    uint idata ADCTempValue=0;

		//=====================初始化=====================
    IO_Init();
    WDTCON |= 0x10;		    //清看门狗
    
		//串口
    Uart0_Init();
	
		//漏电保护
    Leakage_EX_Init();
    //=====================主循环=====================
    while(1)
    {
				/*
        //====================test==========================
        //水流状态标记 0：无水流 1：少水流 2：多水流，正常
        if(water_flow_flag >= 1 && heater_relay_on==0)
        {
						//开始PID算法标记
            b_start_pid=0;

            heater_relay_on=1;
            Scr_Driver_Control_Heat_RLY(heater_relay_on);

            //过零检测中断
            Zero_Crossing_EX_Init();

            //基础定时器中断，采集出水温度值，采集周期和处理周期0.5s
            BTM_Init();
        }
				
				if(water_flow_flag < 1 && heater_relay_on==1)
        {
            heater_relay_on=0;
            Scr_Driver_Control_Heat_RLY(heater_relay_on);
        }
				//====================test==========================
				*/

        if(heater_relay_on==1)
        {
            //检测温度保险 HEAT ERROR 直接检测端口值 P03   轮询方式
            Scr_Driver_Check_Insurance();
        }        

        if(b_btm_int_flag==1)
        {
            //出水温度
            ADC_Init(AIN8);
            ADCTempValue=ADC_Convert(); //启动ADC转换，获得转换值
            get_temp_table(ADCTempValue,&current_out_temp);

            printf("%bd\n",current_out_temp);

            PIDCalc(best_temp_out, current_out_temp);

            b_btm_int_flag=0;
        }

        //串口接收到数据，处理
        Uart_Process();
    }
}

