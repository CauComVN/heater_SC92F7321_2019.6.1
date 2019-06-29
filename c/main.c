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
    //Leakage_EX_Init();
	
		//=====================Test====================
		//ex_flag = Ex_Water_No_Flow;
	
		//start_heater();
	
    //=====================主循环=====================
    while(1)
    {						
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

