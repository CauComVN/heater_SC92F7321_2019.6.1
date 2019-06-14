//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"

//INT24 P20 ZERO

//HEAT TRA PWM1 P01

//HEAT RLY �̵������� P02

//T/S1 �¶ȱ��� HEAT ERROR COM3/P03
//�����⵽�¶ȹ��ߣ�T/S1���Զ���բ���Ͽ���· 
//�̵���ֹͣ���� ���������ɿع�������·
//HEAT ERROR Ϊ����ˣ�������ڸߵ�ƽ��������ˮ���¶ȹ���
//���Ϊ�͵�ƽ��������ˮ���¶���������Χ��

//��ǰ��ˮ�����л�ֹͣ״̬ ���Ƽ̵������� 0��ֹͣ 1������
bit heater_relay_on=0;

////��ˮ���ڲ��쳣״̬
Enum_Ex_Flag Ex_Flag;

//35��~60�� �Զ�����  ��ѣ�40 - 50
int good_temp_out_low=28;
int good_temp_out_high=60;
int best_temp_out=37;
int current_out_temp=28; //��ǰ��ˮ�¶�

void Zero_Crossing_EXTI_Test(void);
void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();

//HEAT TRA  ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� 2�����ٹ���
void Scr_Driver_Time2_Adjust(uint flag);

//�����ʱ
void soft_delay(uint n);

int Scr_Driver_Check_Insurance();//����¶ȱ���
void Scr_Driver_Control_Heat_RLY(int on);//�̵������� HEAT RLY P02

//HEAT TRA PWM1 ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� Duty���� 2�����ٹ��� Duty����
void Scr_Driver_PWM_Adjust(uint flag);

uchar Zero_Crossing_INT1_flag = 0x00;
/*****************************************************
*�������ƣ�void EXTI_Test(void)
*�������ܣ��ⲿ�жϲ���
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Zero_Crossing_EXTI_Test(void)
{
    Zero_Crossing_EX_Init();
    while(1)
    {
    }
}
/*****************************************************
*�������ƣ�void Zero_Crossing_EX_Init(void)
*�������ܣ��ⲿ�жϳ�ʼ��
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Zero_Crossing_EX_Init(void)
{    
    P2CON &= 0XFE;     //�ж�IO������Ϊ��������
    P2PH  |= 0x01;     //�ж�IO������Ϊ���������

    //�����жϿ�INT24  �����غ��½��ض�֧��
    //�½�������
    INT2F |= 0x10;  //0000 xxxx  0�ر� 1ʹ�� ���½���
		//INT2F &= 0x2F; //�ر��½���
    //����������
    INT2R |= 0X10 ;    //0000 xxxx  0�ر� 1ʹ��

    //�ⲿ�ж����ȼ����
    IE1 |= 0x08;	//0000 x000  INT2ʹ��
    IP1 |= 0X00;
    EA = 1;
}

void Zero_Crossing_EX2_Handle()
{
	
    //����ж�2����·���룬���������ػ����½�����ȷ�ϣ��������жϣ����Զ˿ڵ�ƽ��1
    //if(ZERO == 1) //INT24 P20 ZERO �����⵽���
    {
        //PWM����ֵ����
//		Scr_Driver_PWM_Init();
			
			//ȫ����
//			HEAT_TRA=1;
			
//			//�������жϣ��ɿع�ر�
			HEAT_TRA=0;
			
			Timer_Init();
    }
 /*   if(HALL_LLJ == 1) //INT25 P21 ˮ��������
    {

    }
	*/
}

//����¶ȱ��� HEAT ERROR ֱ�Ӽ��˿�ֵ P03   ��ѯ��ʽ
int Scr_Driver_Check_Insurance()
{	
	if(HEAT_ERROR==0)
	{
		//�¶�������Χ�ڣ��¶ȱ��ղ���բ
		return 0;
	}
	else if(HEAT_ERROR==1)
	{
		//�¶��쳣��Χ�ڣ��¶ȱ�������բ
		if(heater_relay_on==1)
		{
				heater_relay_on=0;
				Scr_Driver_Control_Heat_RLY(heater_relay_on);
		}		
		return -1;
	}
	return -1;
}

//�̵������� HEAT RLY P02
void Scr_Driver_Control_Heat_RLY(int on)
{
	P0VO = P0VO&0xfb; //P02�˿����ó���ͨI/O��  1111 1011
	if(on == 1)
	{
		HEAT_RLY=1;
	}
	else
	{
		HEAT_RLY=0;
	}
	
	//�����ʱ����֤heater_relay_on����������ɣ�������ѭ���߼�������߻���
	soft_delay(48000); //48000/24=2000=2ms
}

//HEAT TRA  ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� 2�����ٹ���
void Scr_Driver_Time2_Adjust(uint flag)
{
	/*
	if(flag==1 || flag==2)
	{		
		if(flag==1){ //���ӹ���
			time2_curr=time2_curr-1;
			if(time2_curr<0)
			{
				time2_curr=0;
			}
			
			//�������ֵ
			time2_count_max=open_max-time2_curr/4;
		}
		else if(flag==2) ////���ٹ���
		{
			time2_curr=time2_curr+1;
			if(time2_curr>ref_max)
			{
				time2_curr=ref_max;
			}	

			//�������ֵ
			time2_count_max=open_max-time2_curr/4;
		}		

		//���ڴ�ӡlog�����ԡ�����
		//UART_SentChar(time2_curr);
	}
	*/
	
	if(flag==1 || flag==2)
	{		
		if(flag==1){ //���ӹ���
			scr_curr_time -= scr_adjust_step;
			if(scr_curr_time<1)
			{
				scr_curr_time=0;
			}
		}
		else if(flag==2) //���ٹ���
		{
			scr_curr_time += scr_adjust_step;
			if(scr_curr_time>=scr_open_time_max/scr_adjust_step)
			{
				scr_curr_time=scr_open_time_max;
			}
		}		

		//���ڴ�ӡlog�����ԡ�����
		//UART_SentChar(scr_curr_time);
	}
	
}

//�����ʱ
void soft_delay(uint n)
{
	uint k;
	for(k=0;k<n;k++)
	{
		_nop_();
	}
}

