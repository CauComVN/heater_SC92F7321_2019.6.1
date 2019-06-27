#include "H/Function_Init.H" 
#include <stdio.h>
#include <string.h>

//uchar idata Uart0BuffNumber=0;
//uchar code Uart0Buff[UART0_BUFF_LENGTH];

int CalCrc(int crc, const char *buf, int len);
void start_heater();
void stop_heater();

void Uart0_Init(void);
void UartInt_Handle();
void Uart_Process();
//void UART_SentChar(uchar chr);
//void UART_SendString(uchar *str);

bit UartSendFlag = 0; //�����жϱ�־λ
bit UartReceiveFlag = 0; //�����жϱ�־λ

char putchar(char c)//������дprintf
{
	SBUF = c;
	while(!UartSendFlag);
	UartSendFlag=0;
	return c;
}

void Uart_Process()
{	
	if(UartReceiveFlag)
	{
		
//		unsigned char string[6]={"1234\n"};
//	unsigned int n;
//	sscanf(string,"%u",&n);//string���ַ�����%u�Ǹ�ʽ���ƴ���u���޷���ʮ��������&n�Ǳ���n�ĵ�ַ
//	printf("%d\n",n);//����KEIL C ��������������۲�
		
		UartReceiveFlag=0;
		
		BEE = ~BEE;
	
		if(SBUF == 0x01)
		{				
			best_temp_out=best_temp_out+1;
			if(best_temp_out>good_temp_out_high)
			{
				best_temp_out=good_temp_out_high;
			}
			
			//�����ʱ����֤����������ɣ�������ѭ���߼�������߻���
			soft_delay(48000); 
			
			SBUF = 0x55+SBUF;
			while(!UartSendFlag);
			UartSendFlag = 0;
		}
		if(SBUF == 0x02)
		{				
			best_temp_out=best_temp_out-1;
			if(best_temp_out<good_temp_out_low)
			{
				best_temp_out=good_temp_out_low;
			}
			
			//�����ʱ����֤����������ɣ�������ѭ���߼�������߻���
			soft_delay(48000); 
			
			SBUF = 0x55+SBUF;
			while(!UartSendFlag);
			UartSendFlag = 0;
		}	
		
		if(SBUF == 0x03)
		{	
			start_heater();			
		}
		
		//�رռ��ȿ���
		if(SBUF == 0x04)
		{
			stop_heater();
		}
	}
}

void Uart0_Init(void)    //ѡ��Timer2��Ϊ�������źŷ�����
{
	P1CON &= 0XF3;
	P1PH |= 0X0C;	 //TXΪǿ�������RXΪ���������룻
	SCON  = 0X50;    //����ͨ�ŷ�ʽΪģʽһ���������
	PCON |= 0X80; 
	TMCON |= 0X04;
	T2MOD = 0X00;
	T2CON = 0X30;
	RCAP2H = 0xFF;
	RCAP2L = 0xec;//0xB2; //38400
	TR2 = 0;
	ET2 = 0;
	TR2 = 1;
	EUART = 1;     //����Uart0�ж�
//	EA = 1;
}


/*****************************************************
*�������ƣ�void UartInt(void) interrupt 4
*�������ܣ�Uart0�жϷ�����
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void UartInt_Handle()
{
	if(TI)
	{
		TI = 0;	
		UartSendFlag = 1;		
	}
	if(RI)
	{
		RI = 0;	
		UartReceiveFlag = 1;		
	}	
}

//void UART_SentChar(uchar chr)
//{
//  //����һ���ֽ�
////  SBUF = chr;
////  while( TI == 0);
////  TI = 0;
//	
//	SBUF = chr;
//	while(!UartSendFlag);
//	UartSendFlag = 0;
//}

//void UART_SendString(uchar *str)
//{
//  while(*str != '\0')
//  {
//      UART_SentChar(*str++);
//  }
//}

//CRCУ�����16λ�ģ�Ҳ��32λ�ġ�CRCУ����㷨ԭ��
//CRCУ��;��ǽ�һ�ζ��������ݽ��м��ܣ�����һ������ʽ����
//Ȼ��õ�һ��У���롣�����У�����������ζ��������ݺ�߾����ˡ�
//Ȼ����շ��ڽ��յ�����֮���ٶ����У������н��롣
int CalCrc(int crc, const char *buf, int len)
{
    unsigned int byte;
    unsigned char k;
    unsigned short ACC,TOPBIT;
//    unsigned short remainder = 0x0000;
    unsigned short remainder = crc;
    TOPBIT = 0x8000;
    for (byte = 0; byte < len; ++byte)
    {
        ACC = buf[byte];
        remainder ^= (ACC <<8);
        for (k = 8; k > 0; --k)
        {
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^0x8005;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
    remainder=remainder^0x0000;
    return remainder;
}

void start_heater()
{		
		//�¶Ȳɼ�ADCת�����
		AdcFlag = 0;
	
		// 0:�޹��� 1��ȫ����
		heater_power_status=0; 
		//��ǰ��ˮ�����л�ֹͣ״̬ ���Ƽ̵������� 0��ֹͣ 1������
		heater_relay_on=0;
		//��ʼPID�㷨���
		b_start_pid=0;		
		////��ˮ���ڲ��쳣״̬
		ex_flag=Ex_Normal;
		//35��~60�� �Զ�����  ��ѣ�40 - 50
		best_temp_out=38;
		//��ǰ��ˮ�¶�
		current_out_temp=29;
		//�ɿع败��ʱ�����ֵ 
		scr_open_time_max=zero_period_low_time;
		//ʵʱ����Ŀɿع败��ʱ��
		scr_curr_time=0;
		//ʵʱ����Ŀɿع败��ʱ�丱�������ڽ����ѭ���͹������ж���ȫ�ֱ���scr_curr_time����
		scr_tune_time=0;

		//�ɿع败��ʱ�� �͵�ƽ 8.6ms 17200---0  �ߵ�ƽ 10ms  20000---0
		scr_open_time=0;
		//�ɿع迪ͨ��־ ���ڱ�ǿɿع败�����ض϶�ʱ�����ضϿɿع�
		scr_open_flag=0;
	
		//�ɼ��¶ȶ�ʱ���жϱ�� ������ʱ���жϣ��ɼ���ˮ�¶�ֵ���ɼ����ںʹ�������0.5s	
		b_btm_int_flag=0;		
	
		//ˮ�������������������ж�
		numberPulse = 0;
		//ˮ�����״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
		water_flow_flag=0;
		
	
    //ˮ���������ж�
    Water_Detection_EX_Init();

    //ˮ����ⶨʱ���ж�
    Water_Detection_Timer_Init();
	
		soft_delay(50000); // (1+1+(1+2)*50000)*0.5us=75001us=75.001ms
	
		//ˮ��״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
		if(water_flow_flag == 2 && heater_relay_on==0)
		{			
			heater_relay_on=1;
			Scr_Driver_Control_Heat_RLY(heater_relay_on);				
			
			//�������ж�
			Zero_Crossing_EX_Init();
			
			//������ʱ���жϣ��ɼ���ˮ�¶�ֵ���ɼ����ںʹ�������0.5s
      BTM_Init();
		}
}

void stop_heater()
{
	if(heater_relay_on==1)
	{
		heater_relay_on=0;
		Scr_Driver_Control_Heat_RLY(heater_relay_on);
	}
	
	//�ر�ˮ�������������ⲿ�����ж� INT25 P21 
	//�رչ������ⲿ�ж� INT24 P20 ZERO
	//������ʱ��Base Timer�жϲ�ʹ�� ��ˮ�¶Ȳɼ���ʱ��
	//�ر�PWM�ж�
	IE1 &= 0xf1;	
	
	//�ر�ˮ����ⶨʱ��
	TR0 = 0;//��ʱ��0ֹͣ����
  ET0 = 0;//��ʱ��0������
	
	//�رտɿع败����ʱ��	
	TR1 = 0;//��ʱ��1ֹͣ����
  ET1 = 0;//��ʱ��1������
	
	//�رճ�ˮ�¶Ȳɼ�ADCת���ж�
	EADC=0; //ADC�жϲ�ʹ��
	ADCCON &= 0x08;//�ر�ADCģ���Դ
}
