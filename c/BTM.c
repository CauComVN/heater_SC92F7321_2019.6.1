#include "H/Function_Init.H"
#include <math.h>

uchar idata b_btm_int_flag=false;
//uint count=0;

void BTM_Init(void);
//void BTM_Test(void);



/*****************************************************
*�������ƣ�void BTM_Test(void)
*�������ܣ�BTM����
*��ڲ�����void
*���ڲ�����void
*****************************************************/
//void BTM_Test(void)
//{
//	BTM_Init();
//	while(1)
//	{
//	}
//}
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
//		uint ADCTempValue=0;
//		int ret=0;
//		uint pidret=0;
		
		b_btm_int_flag=true;
		
		
//		count++;
//		if(count>=20)
//		b_btm_int_flag=true;
		
//		//��ˮ�¶�
//			ADC_Init(AIN8);
//			ADCTempValue=ADC_Convert(); //����ADCת�������ת��ֵ
//			ret = get_temp_table(ADCTempValue,&current_out_temp);
//			
//			//UART_SentChar(current_out_temp);
//		
//			if(ret==-1) { //֪ͨ����¶��쳣����������¶ȣ���������BEEP����
//					ex_flag=Ex_Out_Water_Temp_Low;
//					
//			}
//			else if(ret==-2) { //֪ͨ����¶��쳣����������¶ȷ�������BEEP����
//					ex_flag=Ex_Out_Water_Temp_High;
//			}
//			else {
//				//�����¶ȵ�һ�����ʵķ�Χ��
//				//HEAT TRA  ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� 2�����ٹ���
//				
//				//0 --- 20000  42��-28��=14�� 20000/14=1428.57
//								
//				//�趨ֵ����ʵ��ֵ��
//				//ƫ�����2Ϊ���޷�ֵ���(ȫ�ټ���)
//				if(best_temp_out-current_out_temp>2) ////ƫ�����2?
//				{
//					scr_open_time=0;//8600;//17200;//20000;//5;//�͵�ƽ 8.6ms 17200---0  �ߵ�ƽ 10ms  20000---0
//					scr_curr_time=0;
//				}
//				else{
//					pidret=PIDCalc(best_temp_out,current_out_temp);
//					
//					pidret=1400*pidret;
//					
//					if(current_out_temp<best_temp_out)
//					{
//						//Scr_Driver_power_Adjust(1);
//						
//						scr_curr_time -= abs(pidret);
//						if(scr_curr_time<1)
//						{
//							scr_curr_time=0;
//						}
//					}
//					if(current_out_temp>best_temp_out)
//					{
//						//Scr_Driver_power_Adjust(2);
//						
//						scr_curr_time += abs(pidret);
//						if(scr_curr_time>=(scr_open_time_max-zero_peroid_last_time))
//						{
//							scr_curr_time=(scr_open_time_max-zero_peroid_last_time);
//						}
//					}
//				}				
//			}
	}
}

