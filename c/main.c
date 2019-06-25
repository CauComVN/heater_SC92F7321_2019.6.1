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

void main(void)
{
    uint idata ADCTempValue=0;
	
		uint last_temp_value=0;

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
			
				Timer_Init();

        //=====================主循环
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

                printf("%bd\n",current_out_temp);

                PIDCalc(best_temp_out, current_out_temp);

                b_btm_int_flag=0;
            }
						
						
						
						if(zero_int_flag==1)
						{
							zero_int_flag=0;
							
							//if(last_temp_value!=current_out_temp)
							{
							//last_temp_value=current_out_temp;
							//PIDCalc(best_temp_out, current_out_temp);
							}
			
						}
	
		

						//串口接收到数据，处理
						Uart_Process();
      }
    }
    break;    
    case 5:
    {
//			Uart0_Test();
    }
    break;
    case 10:
        //Zero_Crossing_EXTI_Test();
        break;
    case 11:
        //Water_Detection_EXTI_Test();
        break;
    
    case 13:
        Leakage_EX_Init();
        while(1)
        {
        }
        break;

    default:
        ;
    }
}

