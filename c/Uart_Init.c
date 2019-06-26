#include "H/Function_Init.H" 
#include <stdio.h>
#include <string.h>

//uchar idata Uart0BuffNumber=0;
//uchar code Uart0Buff[UART0_BUFF_LENGTH];

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
//	unsigned char string[5]={"1234"};
//	uint n;
//	sscanf(string,"%u",&n); //string���ַ�����%u�Ǹ�ʽ���ƴ���u���޷���ʮ��������&n�Ǳ���n�ĵ�ַ��
//	printf("%d\n",n);//����KEIL C ��������������۲�
	
	if(UartReceiveFlag)
	{
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
		
		//�������ȿ���
		if(SBUF == 0x03)
		{
			//ˮ��״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
			if(water_flow_flag == 2 && heater_relay_on==0)
			{
				b_start_pid=0;
				
				heater_relay_on=1;
				Scr_Driver_Control_Heat_RLY(heater_relay_on);				
				
				//�����ɿع����
				Zero_Crossing_EX_Init();
			}
		}
		//�رռ��ȿ���
		if(SBUF == 0x04)
		{
			if(heater_relay_on==1)
			{
				heater_relay_on=0;
				Scr_Driver_Control_Heat_RLY(heater_relay_on);
			}
		}
	}
}

/*****************************************************
*�������ƣ�void Uart0_Init(void)
*�������ܣ�Uart0�жϳ�ʼ��
*��ڲ�����void 
*���ڲ�����void
*****************************************************/
/*
void Uart0_Init(void)    //ѡ��Timer1��Ϊ�������źŷ�����
{
	P1CON &= 0XF3;
	P1PH |= 0X0C;	 //TX/RXΪ���������룻
	TX1 = 1;		 //TX��ʼ�ߵ�ƽ��
	SCON = 0X50;     //��ʽ1�������������
	PCON |= 0X80; 
	
	
	T2CON = 0x00;    //ʹ�ö�ʱ��1��UARTʱ��
	
	//TMOD = 0X20;     //��ʱ��1  8λ�Զ�����
	//TMCON = 0X02;    //��ʱ��1   Fsys��
	TMOD |= 0X20;     //��ʱ��1  8λ�Զ�����
	TMOD &=0xbf; //////
	TMCON |= 0X02;    //��ʱ��1   Fsys��
	
	TL1 = 217;		
	TH1 = 217;		 //UART ������24M�����=38400��
	TR1 = 0;
	ET1 = 0;		 //Timer1ʹ��
	TR1 = 1;		 //����Timer0
	EUART = 1;	     //����UART�ж�
//	EA = 1;		     //�����ж�
}
*/

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
