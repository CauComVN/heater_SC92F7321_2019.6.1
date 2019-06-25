#include "H/Function_Init.H"
#include <math.h>
#include <stdio.h>

/**************************************************************
˵����
1��Options for Target��Target1����BL51 Locate->Code Range:0x100����¼ѡ����ѡ��DISRST����λ����Ϊ��ͨIOʹ�ã�
2���ı�TEST�Ķ��壬���Էֱ���Զ�Ӧ�Ĺ��ܣ�
3��ע�⣺����Function.H����ѡ������ͺţ�SC92F7320��LCD/LED��PWM���ܣ�
***************************************************************/
// Normal:100 	BTM:0 EXTI:1 		Timer:2 	LCD:3 	ScrDriverPWM:4 	Uart0:5 	ADC:7 	IAP:8
// SerialKey:9 	ZeroCrossing:10 	WaterCheckEXTI:11 	WaterCheckTimer:12 Leakage:13 SoftDelay:14
// Leakage: 13

//#define Test  5
#define Test  100

void main(void)
{
    uint idata ADCTempValue=0;

    IO_Init();
    WDTCON |= 0x10;		    //�忴�Ź�

    switch(Test)
    {
    //��������
    case 100:
    {
        //=====================��ʼ��
        Uart0_Init();

        //©�籣��
        //Leakage_EX_Init();

        //ˮ�����
        Water_Detection_EX_Init();

        //ˮ����ⶨʱ��
        Water_Detection_Timer_Init();
			
				Timer_Init();

        //=====================��ѭ��
        while(1)
        {
            /**/
            //test........
            //ˮ��״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
            if(water_flow_flag >= 1 && heater_relay_on==0)
            {
                b_start_pid=0;

                heater_relay_on=1;
                Scr_Driver_Control_Heat_RLY(heater_relay_on);

                //�����ɿع����
                Zero_Crossing_EX_Init();

                //��ʱ1s���ɿع�ȫ����
                BTM_Init();
            }

            if(heater_relay_on==1)
            {
                //����¶ȱ��� HEAT ERROR ֱ�Ӽ��˿�ֵ P03   ��ѯ��ʽ
                Scr_Driver_Check_Insurance();
            }

            if(water_flow_flag < 1 && heater_relay_on==1)
            {
                heater_relay_on=0;
                Scr_Driver_Control_Heat_RLY(heater_relay_on);
            }

            if(b_btm_int_flag==1)
            {
                //��ˮ�¶�
                ADC_Init(AIN8);
                ADCTempValue=ADC_Convert(); //����ADCת�������ת��ֵ
                get_temp_table(ADCTempValue,&current_out_temp);

                //UART_SentChar(current_out_temp);

                printf("%bd\n",current_out_temp);

                //PIDCalc(best_temp_out, current_out_temp);

                b_btm_int_flag=0;
            }
						
						
						
		if(zero_int_flag==1)
		{
			zero_int_flag=0;
			
			PIDCalc(best_temp_out, current_out_temp);
			/*
			if(heater_power_tune==1)
			{				
				//ȫ����
				//if(HEAT_TRA!=1)
					HEAT_TRA=1;
				
				//��ʱ���ر�
				//if(TR1!=0)
					TR1 = 0;
			}			
			else if(heater_power_tune==0)
			{				
				//�޹���
				//if(HEAT_TRA!=0)
					HEAT_TRA=0;
		
				//��ʱ���ر�
				//if(TR1!=0)
					TR1 = 0;
			}
			else
			{
				if(ZERO==1)
				{
					scr_open_time_max=zero_period_high_time;
				}
				else
				{
					scr_open_time_max=zero_period_low_time;
				}				
				HEAT_TRA=1;
				scr_open_time=scr_tune_time;
				
					
				scr_open_flag=0;		
				TL1 = (65536 - scr_open_time)%256;     //���ʱ�䣺ʱ��ΪFsys/12����scr_open_time*��1/(Fsys/12)��=scr_open_time*0.5us;
				TH1 = (65536 - scr_open_time)/256;
				TR1 = 1;//�򿪶�ʱ��0
				
					}
					*/
				}
	
		

				//���ڽ��յ����ݣ�����
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

