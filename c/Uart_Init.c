#include "H/Function_Init.H" 

void Uart0_Init(void);
void UartInt_Handle();
void Uart_Process();

bit UartSendFlag = 0; //�����жϱ�־λ
bit UartReceiveFlag = 0; //�����жϱ�־λ
/*****************************************************
*�������ƣ�void Uart0_Test(void)
*�������ܣ�Uart0����
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Uart0_Test(void)
{
	Uart0_Init();
	while(1)
	{
//		SBUF = 0x55;
//		while(!UartSendFlag);
//		UartSendFlag = 0;
		
		if(UartReceiveFlag)
		{
			UartReceiveFlag=0;
			
			P00 = ~P00;
		
			//HEAT TRA PWM1 ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� Duty���� 2�����ٹ��� Duty����	
			if(SBUF == 0x01)
			{				
				SBUF = 0x55+SBUF;
				while(!UartSendFlag);
				UartSendFlag = 0;
			}
			if(SBUF == 0x02)
			{				
				SBUF = 0x55+SBUF;
				while(!UartSendFlag);
				UartSendFlag = 0;
			}	
		}
	}
}

void Uart_Process()
{
	if(UartReceiveFlag)
		{
			UartReceiveFlag=0;
			
			P00 = ~P00;
		
			//HEAT TRA PWM1 ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� Duty���� 2�����ٹ��� Duty����	
			if(SBUF == 0x01)
			{
				//Scr_Driver_PWM_Adjust(1);	
				
//				time2_curr=1;
				
				Scr_Driver_Time2_Adjust(1);
				
				SBUF = 0x55+SBUF;
				while(!UartSendFlag);
				UartSendFlag = 0;
			}
			if(SBUF == 0x02)
			{
				//Scr_Driver_PWM_Adjust(2);
				
				//time2_curr=0;			

				Scr_Driver_Time2_Adjust(2);
				
				SBUF = 0x55+SBUF;
				while(!UartSendFlag);
				UartSendFlag = 0;
			}	
		}
}

/*****************************************************
*�������ƣ�void Uart0_Init(void)
*�������ܣ�Uart0�жϳ�ʼ��
*��ڲ�����void 
*���ڲ�����void
*****************************************************/
void Uart0_Init(void)    //ѡ��Timer1��Ϊ�������źŷ�����
{
	P1CON &= 0XF3;
	P1PH |= 0X0C;	 //TX/RXΪ���������룻
	P13 = 1;		 //TX��ʼ�ߵ�ƽ��
	SCON = 0X50;     //��ʽ1�������������
	PCON |= 0X80; 
	T2CON = 0x00;    //ʹ�ö�ʱ��1��UARTʱ��
	TMOD = 0X20;     //��ʱ��1  8λ�Զ�����
	TMCON = 0X02;    //��ʱ��1   Fsys��
	TL1 = 217;		
	TH1 = 217;		 //UART ������24M�����=38400��
	TR1 = 0;
	ET1 = 0;		 //Timer1ʹ��
	TR1 = 1;		 //����Timer0
	EUART = 1;	     //����UART�ж�
	EA = 1;		     //�����ж�
}

/*
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
	RCAP2L = 0xB2;
	TR2 = 0;
	ET2 = 0;
	TR2 = 1;
	EUART = 1;     //����Uart0�ж�
	EA = 1;
}
*/

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
