//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"
#include <stdio.h>

/******************
	*������������Ҫ����ʵ��������Ե�
	******************/
	//15.6msMS����һ�� ��������
#define pidt   0.5
//����ϵ��  0.01 --- 10  ����Ƶ�ʵͣ���500ms����Kpһ����0.01���𣻲���Ƶ�ʸߣ���1ms����Kpһ����1����
//kp=1
#define  Kp   10  
//����ʱ�� 60
#define  Ti  100
//΢��ʱ�� 600
#define  Td  1

//55*10-28*10=270
#define Upper_Limit  30
//�����ˮ�¶ȳ���Ԥ���¶ȣ��ɿع��޹�������
#define Lower_Limit  -1


//INT24 P20 ZERO

//HEAT TRA PWM1 P01

//HEAT RLY �̵������� P02

//T/S1 �¶ȱ��� HEAT ERROR COM3/P03
//�����⵽�¶ȹ��ߣ�T/S1���Զ���բ���Ͽ���· 
//�̵���ֹͣ���� ���������ɿع�������·
//HEAT ERROR Ϊ����ˣ�������ڸߵ�ƽ��������ˮ���¶ȹ���
//���Ϊ�͵�ƽ��������ˮ���¶���������Χ��

volatile uchar heater_power_status=0; // 0:�޹��� 1��ȫ����

//��ǰ��ˮ�����л�ֹͣ״̬ ���Ƽ̵������� 0��ֹͣ 1������
 volatile bit heater_relay_on=0;

volatile bit b_start_pid=0;

////��ˮ���ڲ��쳣״̬
Enum_Ex_Flag idata Ex_Flag;

//35��~60�� �Զ�����  ��ѣ�40 - 50
int idata best_temp_out=37;
volatile uchar  current_out_temp=28; //��ǰ��ˮ�¶�

volatile int  scr_curr_time=0;//zero_period_high_time/2;//20000;//6;



//int idata Out1=0;  //��¼�ϴ����

//int idata ERR=0;       //��ǰ���
//int idata ERR1=0;      //�ϴ����
//int  idata ERR2=0;      //���ϴ����


//void Zero_Crossing_EXTI_Test(void);
void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();

//�����ʱ
void soft_delay(uint n);

// Sv�趨�¶�ֵ  Pv��ǰ�¶�ֵ
void PIDCalc(int Sv,int Pv);

int Scr_Driver_Check_Insurance();//����¶ȱ���
void Scr_Driver_Control_Heat_RLY(int on);//�̵������� HEAT RLY P02

/*****************************************************
*�������ƣ�void Zero_Crossing_EX_Init(void)
*�������ܣ��ⲿ�жϳ�ʼ��
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Zero_Crossing_EX_Init(void)
{    
    P2CON &= 0XFE;     //�ж�IO������Ϊ��������
    //P2PH  |= 0x01;     //�ж�IO������Ϊ���������
		P2PH &= 0xfe; //�������� �ⲿ����������

    //�����жϿ�INT24  �����غ��½��ض�֧��
    //�½�������
    INT2F |= 0x10;  //0000 xxxx  0�ر� 1ʹ�� ���½���
		//INT2F &= 0x2F; //�ر��½���
    //����������
    INT2R |= 0X10 ;    //0000 xxxx  0�ر� 1ʹ��

    //�ⲿ�ж����ȼ����
    IE1 |= 0x08;	//0000 x000  INT2ʹ��
    IP1 |= 0X00;
//    EA = 1;
}

void Zero_Crossing_EX2_Handle()
{	
    //����ж�2����·���룬���������ػ����½�����ȷ�ϣ��������жϣ����Զ˿ڵ�ƽ��1
    //if(ZERO == 1) //INT24 P20 ZERO �����⵽���
    {
			if(heater_power_status==1)
			{				
				//ȫ����
				if(HEAT_TRA!=1)
					HEAT_TRA=1;
				
				//��ʱ���ر�
				if(TR1!=0)
					TR1 = 0;
			}			
			else if(heater_power_status==0)
			{				
				//�޹���
				if(HEAT_TRA!=0)
					HEAT_TRA=0;
		
				//��ʱ���ر�
				if(TR1!=0)
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
				
				//scr_open_time = scr_open_time_max/2;//37��

				if(scr_curr_time>0 && scr_curr_time<=(scr_open_time_max-zero_peroid_last_time))
				{
					if(HEAT_TRA!=1)
					{
							HEAT_TRA=1;
					}			
			
					if(scr_open_time != scr_curr_time)
					{
						scr_open_time=scr_curr_time;
					}
		
					Timer_Init();
				}
				else
				{
					if(HEAT_TRA!=0)
						HEAT_TRA=0;
					
					//��ʱ���ر�
					if(TR1!=0)
						TR1 = 0;
				}				
			}
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
	soft_delay(48000); // (1+1+��1+2��*48000)*0.5us=72001us=72.001ms
}


//�����ʱ https://blog.csdn.net/nanfeibuyi/article/details/83577641 
/*

����ѭ��������INCָ���CJNEָ���ܹ�Ҫִ��n��,�����Ļ������ڣ�1+2��*n��

����CLRָ���MOVָ�ѭ�������ܹ����Ļ������ڣ�1+1+��1+2��*n

�����Ƭ���ľ���Ƶ��Ϊ24MHz����������� = 12*��1/24��us = 0.5us��

��ôforѭ�������ʱΪ(1+1+��1+2��*n)*0.5us��
*/
void soft_delay(uint n)
{
	uint k;
	for(k=0;k<n;k++)
	{
		_nop_();  //��������һ���������� ��������Ƭ���ľ�����12M�ģ���ô������������1US
//		;
	}
}

// Sv�趨�¶�ֵ  Pv��ǰ�¶�ֵ
void PIDCalc(int Sv,int Pv)
{ 	
	//�¶�ֵ��10������
//	int target_temp=Sv*10;
//	int curr_temp=Pv*10;
	
	//int Out=target_temp-curr_temp;
			
	int DERR1 = 0;       //
	int DERR2 = 0;       //

	int Pout = 0;       //�������
	int Iout = 0;       //���ֽ��
	int Dout = 0;       //΢�ֽ��
	int Out = 0; //�����
	
	
	
	
	static int Out1=0;  //��¼�ϴ����

	static int ERR=0;       //��ǰ���
	static int ERR1=0;      //�ϴ����
	static int ERR2=0;      //���ϴ����

	
//	static uint Upper_Limit= 100; //PID�������
//	static uint Lower_Limit= 0; //PID�������
	
	
	 //ERR = target_temp - curr_temp;   //�����ǰ���
	 
	 ERR=Sv-Pv;
	 /**/
	DERR1 = ERR - ERR1;   //�ϴ�
	
	//DERR2 = ERR - 2*ERR1 + ERR2; //���ϴ�  //��Ҫ����������жϳ�����ͬʱ��8bit���ϵĳ˳������㣬�����
	DERR2= ERR  + ERR2;
	DERR2= DERR2 - ERR1;
	DERR2= DERR2 - ERR1;
	
	//��Kp
	Pout = DERR1;///2;//DERR1*Kp;    //���P
	Iout = ERR * Ti;//(float)(ERR * ((Kp * pidt) / Ti));  //���I
	Dout = 0;//DERR2 * Td;//0;//(float)(DERR2 * ((Kp * Td) / pidt));   //���D
	//Out = (int)(Out1 + Pout + Iout + Dout);
	Out = Out1+ Pout;
	Out = Out+ Iout;
	Out = Out+ Dout;
	
	if(Out >= Upper_Limit) { //���������ڵ�������
		Out = Upper_Limit;
	} 
	else if(Out <= Lower_Limit) { //������С�ڵ�������
		Out = Lower_Limit;
	}
	
	Out1 = Out;      //��¼��������ֵ

	ERR2 = ERR1;    //��¼���
	ERR1 = ERR;     //��¼���
	
	
	Out=ERR;
	
	printf("%d\n",Out);	
	
	//�رչ����ж�
	//IE1 &= 0xf7;	//0000 x000  INT2�ر�
	
	if(Out>0)
	{
		//printf("111\n");
		
		if(ERR>2)
		{
			if(heater_power_status!=1)
					heater_power_status=1;
		}
		else
		{
			if(heater_power_status!=2)
					heater_power_status=2;
			
			//PID�㷨����
			if(b_start_pid==0)
			{
				b_start_pid=1;
				
				//ȫ���ʵ���90% ���ʵ������෴�� (100-90)/100=1/10
				scr_curr_time = 1000;//zero_period_low_time/10; //17200 //���������ã����Ը��̶�ֵ
			}
			else
			{		
				//һ��Ҫ�������Ϊ���ʵ������෴�ģ�scr_curr_timeԽС������Խ��
				scr_curr_time = scr_curr_time - Out;  //Out=50 Out*74=3700
				if(scr_curr_time<1)
				{					
					if(heater_power_status!=1)
						heater_power_status=1;
				}
			}
		}
		//printf("%d\n",scr_curr_time);
		/**/
	}
	else if(Out<0)
	{		
		//printf("222\n");
		
		if(heater_power_status!=0)
			heater_power_status=0;//scr_curr_time=zero_period_high_time;//scr_open_time_max-zero_peroid_last_time;
	}
	else
	{
		//printf("333\n");
	}
	
	//���������ж�
	//IE1 |= 0x08;	//0000 x000  INT2ʹ��
}
