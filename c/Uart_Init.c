#include "H/Function_Init.H"
#include <stdio.h>
#include <string.h>

bit UartSendFlag = 0; //�����жϱ�־λ
bit UartReceiveFlag = 0; //�����жϱ�־λ

uchar idata Uart0RecvBuffNum=0;
char idata Uart0SendBuff[UART0_BUFF_LENGTH];
char idata Uart0RecvBuff[UART0_BUFF_LENGTH];

int idata crc=0;
int idata i=0;
int idata result=0;

int CalCrc(int crc, const char *buf, int len);
void init_heater();
void start_heater();
void stop_heater();

void Uart0_Init(void);
void UartInt_Handle();
void Uart_Process();
void UART_SendChar(uchar chr);

//�ȼ���printf
//void UART_SendString(uchar *str);

char putchar(char c)//������дprintf
{
    SBUF = c;
    while(!UartSendFlag);
    UartSendFlag=0;
    return c;
}

void Uart_Process()
{
    //���͹�����Ϣ����λ��
    if(ex_flag!=Ex_Normal)
    {
        //ֹͣ��ˮ��
        stop_heater();

        //�����쳣Э�����λ��  ��ǰ�¶�ֵ+������+CSCУ��
        Uart0SendBuff[0]=current_out_temp;
        Uart0SendBuff[1]=ex_flag;
        crc = CalCrc(0x00, Uart0SendBuff, 2);//����õ���16λCRCУ����
        Uart0SendBuff[2] = (char)(crc >> 8);//ȡУ����ĸ߰�λ
        Uart0SendBuff[3] = (char)crc;//ȡУ����ĵͰ�λ
			
				UART_SendChar(Uart0SendBuff[0]);
				UART_SendChar(Uart0SendBuff[1]);
				UART_SendChar(Uart0SendBuff[2]);
				UART_SendChar(Uart0SendBuff[3]);
    }

    if(Uart0RecvBuffNum==UART0_BUFF_LENGTH)				//���ռ���
    {
        result = CalCrc(0x00, Uart0RecvBuff, UART0_BUFF_LENGTH);
        if(result==0)//CRC��֤ͨ��
        {
					switch(Uart0RecvBuff[0])
					{
						case Protocol_Heater_Start:
							start_heater();
							break;
						case Protocol_Heater_Stop:
							stop_heater();
							break;
						case Protocol_Heater_Inc_Power:
							best_temp_out=best_temp_out-1;
							if(best_temp_out<good_temp_out_low)
							{
									best_temp_out=good_temp_out_low;
							}
							break;
						case Protocol_Heater_Red_Power:
							best_temp_out=best_temp_out+1;
							if(best_temp_out>good_temp_out_high)
							{
									best_temp_out=good_temp_out_high;
							}
							break;
						case Protocol_Heater_Reset_Temp:
							//best_temp_out=Uart0RecvBuff[1];
							//8λ��Ƭ����֧�ָ������㣬��߿��ƾ��ȣ���10����
							best_temp_out=10*Uart0RecvBuff[1];
							break;
						default:
							break;
					}					
        }
				else
				{
					////CRC��֤��ͨ��
					ex_flag=Ex_Uart_Crc_Error;
				}
				
				Uart0RecvBuffNum = 0;//����������ָ��ʼ
				for(i=0; i<UART0_BUFF_LENGTH; i++)	//��ս��ջ���׼����һ�εĽ���
				{
						Uart0RecvBuff[i] = 0;
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

			
			  if(Uart0RecvBuffNum<UART0_BUFF_LENGTH)
				{
					Uart0RecvBuff[Uart0RecvBuffNum] = SBUF; //�����յ����ݴ��뻺����
					Uart0RecvBuffNum++;
				}
    }
}

void UART_SendChar(uchar chr)
{
	SBUF = chr;
	while(!UartSendFlag);
	UartSendFlag = 0;
}

//void UART_SendString(uchar *str)
//{
//  while(*str != '\0')
//  {
//      UART_SendChar(*str++);
//  }
//}

//CRCУ�����16λ�ģ�Ҳ��32λ�ġ�CRCУ����㷨ԭ��
//CRCУ��;��ǽ�һ�ζ��������ݽ��м��ܣ�����һ������ʽ����
//Ȼ��õ�һ��У���롣�����У�����������ζ��������ݺ�߾����ˡ�
//Ȼ����շ��ڽ��յ�����֮���ٶ����У������н��롣
int CalCrc(int crc, const char *buf, int len)
{
    unsigned int idata byte;
    unsigned char idata k;
    unsigned short idata ACC,TOPBIT;
//    unsigned short idata remainder = 0x0000;
    unsigned short idata remainder = crc;
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

void init_heater()
{
    //�¶Ȳɼ�ADCת�����
    AdcFlag = 0;
	
		//�����ͻ��� 
		//8λ��Ƭ����֧�ָ������㣬��߿��ƾ��ȣ���10����
		umax=500;//=50;//��������¶�ֵ
		umin=290;//=29;//������С�¶�ֵ	

		Out1=0;  		//��¼�ϴ����
		ERR=0;       //��ǰ���
		ERR1=0;      //�ϴ����
		ERR2=0;      //���ϴ����	
		integral=0;		//���ַ���
    // 0:�޹��� 1��ȫ����
    heater_power_status=0;
    //��ǰ��ˮ�����л�ֹͣ״̬ ���Ƽ̵������� 0��ֹͣ 1������
    heater_relay_on=0;
    //��ʼPID�㷨���
    b_start_pid=0;
    ////��ˮ���ڲ��쳣״̬
    ex_flag=Ex_Normal;
    //35��~60�� �Զ�����  ��ѣ�40 - 50
		//8λ��Ƭ����֧�ָ������㣬��߿��ƾ��ȣ���10����
		best_temp_out=380;//38;
		//��ǰ��ˮ�¶�
		//8λ��Ƭ����֧�ָ������㣬��߿��ƾ��ȣ���10����
		current_out_temp=290;//29;
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
}

void start_heater()
{
    //��ˮ����ʼ��
    init_heater();

    //ˮ���������ж�
    Water_Detection_EX_Init();

    //ˮ����ⶨʱ���ж�
    Water_Detection_Timer_Init();  

		soft_delay(50000); 

    //ˮ��״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
    if(water_flow_flag >=1 && heater_relay_on==0)
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
		heater_relay_on=0;
    Scr_Driver_Control_Heat_RLY(heater_relay_on);
	
		//������
		soft_delay(50000); // (1+1+(1+2)*50000)*0.5us=75001us =75ms
	
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
