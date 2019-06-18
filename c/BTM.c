#include "H/Function_Init.H"
#include <math.h>

bit b_btm_int_flag=false;
uint count=0;

void BTM_Init(void);
void BTM_Test(void);

// Sv�趨�¶�ֵ  Pv��ǰ�¶�ֵ
int PIDCalc(int Sv,int Pv);

/*****************************************************
*�������ƣ�void BTM_Test(void)
*�������ܣ�BTM����
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void BTM_Test(void)
{
	BTM_Init();
	while(1)
	{
	}
}
/*****************************************************
*�������ƣ�void BTM_Init(void)
*�������ܣ�BTM��ʼ��
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void BTM_Init(void)
{
//	BTMCON = 0x00;  //������
	BTMCON = 0x80;  //ÿ15.6ms����һ���ж�
//	BTMCON = 0x81;	//ÿ31.3ms����һ���ж�
//	BTMCON = 0x82;	//ÿ62.5ms����һ���ж�
//	BTMCON = 0x83;	//ÿ125ms����һ���ж�
//	BTMCON = 0x84;	//ÿ0.25s����һ���ж�
//	BTMCON = 0x85;	//ÿ0.5s����һ���ж�
//	BTMCON = 0x86;	//ÿ1s����һ���ж�
	BTMCON = 0x87;	//ÿ2s����һ���ж�
	EA = 1;           //�������ж�
	IE1 |= 0x04;       //����BTM�ж�
}

/*****************************************************
*�������ƣ�void BTM_Int(void) interrupt 9
*�������ܣ��жϺ���
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void BTM_Int(void) interrupt 9
{
	if(!(BTMCON&0X40))		//�жϱ�־λ�ж�
	{
		uint ADCTempValue=0;
		int ret=0;
		uint pidret=0;
		
//		count++;
//		if(count>=20)
//		b_btm_int_flag=true;
		
		//��ˮ�¶�
			ADC_Init(AIN8);
			ADCTempValue=ADC_Convert(); //����ADCת�������ת��ֵ
			ret = get_temp_table(ADCTempValue,&current_out_temp);
			
			UART_SentChar(current_out_temp);
		
			if(ret==-1) { //֪ͨ����¶��쳣����������¶ȣ���������BEEP����
					ex_flag=Ex_Out_Water_Temp_Low;
					
			}
			else if(ret==-2) { //֪ͨ����¶��쳣����������¶ȷ�������BEEP����
					ex_flag=Ex_Out_Water_Temp_High;
			}
			else {
				//�����¶ȵ�һ�����ʵķ�Χ��
				//HEAT TRA  ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� 2�����ٹ���
//				if(current_out_temp<best_temp_out)
//				{
//					Scr_Driver_power_Adjust(1);
//				}
//				if(current_out_temp>best_temp_out)
//				{
//					Scr_Driver_power_Adjust(2);
//				}
				
				//0 --- 20000  42��-28��=14�� 20000/14=1428.57
				
				
				//�趨ֵ����ʵ��ֵ��
				//ƫ�����2Ϊ���޷�ֵ���(ȫ�ټ���)
				if(best_temp_out-current_out_temp>2) ////ƫ�����2?
				{
					scr_open_time=0;//8600;//17200;//20000;//5;//�͵�ƽ 8.6ms 17200---0  �ߵ�ƽ 10ms  20000---0
					scr_curr_time=0;
				}
				else{
					pidret=PIDCalc(best_temp_out,current_out_temp);
					
					pidret=1400*pidret;
					
					if(current_out_temp<best_temp_out)
					{
						//Scr_Driver_power_Adjust(1);
						
						scr_curr_time -= abs(pidret);
						if(scr_curr_time<1)
						{
							scr_curr_time=0;
						}
					}
					if(current_out_temp>best_temp_out)
					{
						//Scr_Driver_power_Adjust(2);
						
						scr_curr_time += abs(pidret);
						if(scr_curr_time>=(scr_open_time_max-zero_peroid_last_time))
						{
							scr_curr_time=(scr_open_time_max-zero_peroid_last_time);
						}
					}
				}				
			}
	}
}

// Sv�趨�¶�ֵ  Pv��ǰ�¶�ֵ
int PIDCalc(int Sv,int Pv)
{ 		
	int DERR1 = 0;       //
	int DERR2 = 0;       //

	float Pout = 0;       //�������
	float Iout = 0;       //���ֽ��
	float Dout = 0;       //΢�ֽ��
	unsigned int Out = 0; //�����
	static unsigned int Out1;  //��¼�ϴ����

	static int ERR;       //��ǰ���
	static int ERR1;      //�ϴ����
	static int ERR2;      //���ϴ����

	/******************
	*������������Ҫ����ʵ��������Ե�
	******************/
	static int pidt = 0.0156;// 15.6ms;//300;     //300MS����һ�� ��������
	static char Kp = 5;       //����ϵ��
	static unsigned int Ti= 5000; //����ʱ��
	static unsigned int Td= 600; //΢��ʱ��

//	static uint Upper_Limit= 100; //PID�������
//	static uint Lower_Limit= 0; //PID�������


//	if(pidtimer < pidt)     //��������   pidtimer�����ö�ʱ����ʱ
//		return ;  //

	ERR = Sv - Pv;   //�����ǰ���
	DERR1 = ERR - ERR1;   //�ϴ�
	DERR2 = ERR - 2 * ERR1 + ERR2; //���ϴ�

//	Pout = Kp * DERR1;    //���P
//	Iout = (float)(ERR * ((Kp * pidt) / Ti));  //���I
//	Dout = (float)(DERR2 * ((Kp * Td) / pidt));   //���D
//	Out = (unsigned int)(Out1 + Pout + Iout + Dout);
	
	//��Kp
	Pout = Kp * DERR1;    //���P
	Iout = 0;//(float)(ERR * ((Kp * pidt) / Ti));  //���I
	Dout = 0;//(float)(DERR2 * ((Kp * Td) / pidt));   //���D
	Out = (uint)(Out1 + Pout + Iout + Dout);
	

//	if(Out >= Upper_Limit) { //���������ڵ�������
//		Out = Upper_Limit;
//	} 
//	else if(Out <= Lower_Limit) { //������С�ڵ�������
//		Out = Lower_Limit;
//	}
	Out1 = Out;      //��¼��������ֵ

	ERR2 = ERR1;    //��¼���
	ERR1 = ERR;     //��¼���
//	pidtimer = 0;   //��ʱ���������¼���

	return Out;
}