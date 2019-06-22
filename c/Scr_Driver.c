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
//#define  Kp   (5/10)    //һ�����������ã�keil ��֧��
//����ʱ��
#define  Ti  5000
//΢��ʱ��
#define  Td 600 

//55*10-28*10=270
#define Upper_Limit  270
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

//��ǰ��ˮ�����л�ֹͣ״̬ ���Ƽ̵������� 0��ֹͣ 1������
bit heater_relay_on=0;

bit b_start_pid=false;

////��ˮ���ڲ��쳣״̬
Enum_Ex_Flag idata Ex_Flag;

//35��~60�� �Զ�����  ��ѣ�40 - 50
int idata best_temp_out=37;
int  current_out_temp=28; //��ǰ��ˮ�¶�

int idata scr_curr_time=0;//zero_period_high_time/2;//20000;//6;



//int idata Out1=0;  //��¼�ϴ����

//int idata ERR=0;       //��ǰ���
//int idata ERR1=0;      //�ϴ����
//int  idata ERR2=0;      //���ϴ����


//void Zero_Crossing_EXTI_Test(void);
void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();

//HEAT TRA  ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� 2�����ٹ���
//void Scr_Driver_power_Adjust(uint flag);

//�����ʱ
void soft_delay(uint n);
//void delay_1ms(uint x);
//void delay(uint n);

// Sv�趨�¶�ֵ  Pv��ǰ�¶�ֵ
void PIDCalc(int Sv,int Pv);

int Scr_Driver_Check_Insurance();//����¶ȱ���
void Scr_Driver_Control_Heat_RLY(int on);//�̵������� HEAT RLY P02

//HEAT TRA PWM1 ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� Duty���� 2�����ٹ��� Duty����
//void Scr_Driver_PWM_Adjust(uint flag);

//uchar Zero_Crossing_INT1_flag = 0x00;
/*****************************************************
*�������ƣ�void EXTI_Test(void)
*�������ܣ��ⲿ�жϲ���
*��ڲ�����void
*���ڲ�����void
*****************************************************/
//void Zero_Crossing_EXTI_Test(void)
//{
//    Zero_Crossing_EX_Init();
//    while(1)
//    {
//    }
//}
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
    EA = 1;
}

void Zero_Crossing_EX2_Handle()
{
	
    //����ж�2����·���룬���������ػ����½�����ȷ�ϣ��������жϣ����Զ˿ڵ�ƽ��1
    //if(ZERO == 1) //INT24 P20 ZERO �����⵽���
    {			
			//ȫ����
//			HEAT_TRA=1;
			
//			//�������жϣ��ɿع�ر�
			HEAT_TRA=0;
			
			//��ʱ���ر�
			//TR1 = 0;
			
			if(scr_curr_time==0)
			{
				//ȫ����
				HEAT_TRA=1;
				
				//��ʱ���ر�
				TR1 = 0;
			}			
			else if(scr_curr_time==zero_period_high_time)
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

				if(scr_curr_time<=(scr_open_time_max-zero_peroid_last_time))
				{
					if(scr_open_time != scr_curr_time)
					{
						scr_open_time=scr_curr_time;
					}
		
					Timer_Init();
				}
				else
				{
					HEAT_TRA=0;
					
					//��ʱ���ر�
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

 CLRָ������1����������

 MOVָ������1����������

 INCָ������1����������                                      

 CJNEָ������2����������

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


#if 0

//https://blog.csdn.net/nanfeibuyi/article/details/83577641 
//��������delay_1ms(uint x)
//���ܣ����ö�ʱ��0��ȷ��ʱ1ms
//���ú�����
//���������x,1ms����
//���������
//˵������ʱ��ʱ��Ϊ1ms����x
//��ʱ1s��delay_1ms(1000); //1000ms = 1s 
void delay_1ms(uint x)
{
	TMOD=0X01;//����ʱ��0��������ʽΪ1
	TR0=1;//������ʱ��0��
	while(x--)
	{
		TH0=0Xfc;//��ʱ1ms��ֵ�ĸ�8λװ��TH0
		TL0=0X18;//��ʱ1ms��ֵ�ĵ�8λװ��TL0
		while(!TF0);//�ȴ���ֱ��TF0Ϊ1
		TF0=0;	   //�������λ��־
	}		
	TR0=0;//ֹͣ��ʱ��0��
}

////https://blog.csdn.net/nanfeibuyi/article/details/83577641 
//�����Ƭ������Ƶ��24MHz����������=12*(1/24)us=0.5us
//ѭ����ʱΪ ((1+1+600+1+2)*n+1+1)*0.5
void delay(uint n)//��ʱ����
{
	uint i, j;
	for(i=0; i<n; i++) //ִ��n�� (1+1+600+1+2)*n
		for(j=0; j<200; j++) //��1+2��*200=600����
			;
}

#endif


// Sv�趨�¶�ֵ  Pv��ǰ�¶�ֵ
void PIDCalc(int Sv,int Pv)
{ 		
	//int pidret=0;
	
	 //�¶�ֵ��10������
	int target_temp=Sv*10;
	int curr_temp=Pv*10;
	
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
	
	
	 ERR = target_temp - curr_temp;   //�����ǰ���
	DERR1 = ERR - ERR1;   //�ϴ�
	
	//DERR2 = ERR - 2*ERR1 + ERR2; //���ϴ�  //��Ҫ����������жϳ�����ͬʱ��8bit���ϵĳ˳������㣬�����
	DERR2= ERR  + ERR2;
	DERR2= DERR2 - ERR1;
	DERR2= DERR2 - ERR1;
	
	//��Kp
	Pout = DERR1/2;//DERR1*Kp;    //���P
	Iout = 0;//(float)(ERR * ((Kp * pidt) / Ti));  //���I
	Dout = 0;//(float)(DERR2 * ((Kp * Td) / pidt));   //���D
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
	
	/**/
			
	if(Out>0)
	{
		printf("111");
		
		//ƫ�����2��Ϊ���޷�ֵ���(ȫ�ټ���)
		if(best_temp_out-current_out_temp>20)//�¶�ƫ�����2?
		{
			if(scr_curr_time<1)
			{
				scr_curr_time=0;
			}
		}
		else
		{
			//PID�㷨����
			if(b_start_pid==false)
			{
				b_start_pid=true;
				
				//ȫ���ʵ���80% ���ʵ������෴�� (100-80)/100=1/5
				scr_curr_time = zero_period_low_time/5;
			}
			else
			{
				//20000/270=74
		
				//һ��Ҫ�������Ϊ���ʵ������෴�ģ�scr_curr_timeԽС������Խ��
				scr_curr_time -= Out*74;
				if(scr_curr_time<1)
				{
					scr_curr_time=0;
				}
			}
		}
	}
	else if(Out<0)
	{
		//UART_SentChar(0x57);
		
		printf("222");
		
		if(HEAT_TRA!=0)
			HEAT_TRA=0;
		
		//��ʱ���ر�
		if(TR1!=0)
			TR1 = 0;
		
		scr_curr_time=zero_period_high_time;//scr_open_time_max-zero_peroid_last_time;
	}
	else
	{
		printf("333");
	}
}
