#include "H/Function_Init.H"
#include <math.h>
#include <stdio.h>

/**************************************************************
˵����
1��Options for Target��Target1����BL51 Locate->Code Range:0x100����¼ѡ����ѡ��DISRST����λ����Ϊ��ͨIOʹ�ã�
2���ı�TEST�Ķ��壬���Էֱ���Զ�Ӧ�Ĺ��ܣ�
3��ע�⣺����Function.H����ѡ������ͺţ�SC92F7320��LCD/LED��PWM���ܣ�
***************************************************************/

void main(void)
{
    uint idata ADCTempValue=0;

		//=====================��ʼ��=====================
    IO_Init();
    WDTCON |= 0x10;		    //�忴�Ź�
    
		//����
    Uart0_Init();
	
		//©�籣��
    Leakage_EX_Init();
    //=====================��ѭ��=====================
    while(1)
    {
				/*
        //====================test==========================
        //ˮ��״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
        if(water_flow_flag >= 1 && heater_relay_on==0)
        {
						//��ʼPID�㷨���
            b_start_pid=0;

            heater_relay_on=1;
            Scr_Driver_Control_Heat_RLY(heater_relay_on);

            //�������ж�
            Zero_Crossing_EX_Init();

            //������ʱ���жϣ��ɼ���ˮ�¶�ֵ���ɼ����ںʹ�������0.5s
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
            //����¶ȱ��� HEAT ERROR ֱ�Ӽ��˿�ֵ P03   ��ѯ��ʽ
            Scr_Driver_Check_Insurance();
        }        

        if(b_btm_int_flag==1)
        {
            //��ˮ�¶�
            ADC_Init(AIN8);
            ADCTempValue=ADC_Convert(); //����ADCת�������ת��ֵ
            get_temp_table(ADCTempValue,&current_out_temp);

            printf("%bd\n",current_out_temp);

            PIDCalc(best_temp_out, current_out_temp);

            b_btm_int_flag=0;
        }

        //���ڽ��յ����ݣ�����
        Uart_Process();
    }
}

