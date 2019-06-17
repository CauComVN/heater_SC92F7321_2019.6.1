#include "H/Function_Init.H"

/**************************************************************
˵����
1��Options for Target��Target1����BL51 Locate->Code Range:0x100����¼ѡ����ѡ��DISRST����λ����Ϊ��ͨIOʹ�ã�
2���ı�TEST�Ķ��壬���Էֱ���Զ�Ӧ�Ĺ��ܣ�
3��ע�⣺����Function.H����ѡ������ͺţ�SC92F7320��LCD/LED��PWM���ܣ�
***************************************************************/
// Normal:100 	BTM:0 EXTI:1 		Timer:2 	LCD:3 	ScrDriverPWM:4 	Uart0:5 	ADC:7 	IAP:8 
// SerialKey:9 	ZeroCrossing:10 	WaterCheckEXTI:11 	WaterCheckTimer:12 Leakage:13 SoftDelay:14
// Leakage: 13

//#define Test  5
#define Test  100   

void AppInit();

//�������к���
void AppHandle();

void main(void)
{
	IO_Init();
	WDTCON |= 0x10;		    //�忴�Ź�
	
	switch(Test)
	{
		//��������
		case 100:
			AppHandle();
			break;
		
		case 0: 
			//BTM_Test();
		break;
		case 1: 
		{
//			//��AD_IN_WTR P16  AD_OUT_WTR P17���������ʰ����жϣ���ˮ�¶� ��ˮ�¶��滻
//			
//			// ��Ҫ������������δ����
//			EXTI_Test();
		}			
		break;
		case 2: Timer_Test();
		break;
		case 3: 
//			LCD_Test();
		break;
		case 4: 
		{
//			Scr_Driver_PWM_Test();
		}
		break;
		case 5: 
		{
			Uart0_Test();
		}
		break;
		case 7: ADC_Test();
		break;
//		case 8: IAP_Test(0x1fff,IapROM);   //����ROM���Ե�ַ0x1fff���ж�д����
////						IAP_Test(0x7f,IapEPPROM);   //����EPPROM���Ե�ַ0x7f���ж�д����
//		break;
		
		case 9: Serial_Key_Test();
		break;
		case 10: Zero_Crossing_EXTI_Test();
		break;
		case 11: Water_Detection_EXTI_Test();
		break;
		case 12: Water_Detection_Timer_Test();
		break;
		case 13: 
			Leakage_EXTI_Test();
		break;
		case 14:
			delay_1ms(10);
			delay(300);
			break;
		default:	
		;			
	}
}

//�������к���
void AppHandle()
{
		
	//=====================��ʼ��
	uint ADCTempValue=0;
	int ret=0;
	
	AppInit();
	
	Uart0_Init();
	
	//©�籣��
	//Leakage_EX_Init();
	
	
	//ˮ�����
  Water_Detection_EX_Init(); 
	
	//ˮ����ⶨʱ��
  Water_Detection_Timer_Init();
	/*
	//��ʼ���������ô���Rx Tx���ж������ڹ��� ��Ҫ������������δ����
	//Serial_Key_Init();
	*/
	
		
	//=====================��ѭ��
	/**/
	while(1)
	{
		/**/
			//test........
			//ˮ��״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
			if(water_flow_flag >= 1 && heater_relay_on==0)
			{
				heater_relay_on=1;
				Scr_Driver_Control_Heat_RLY(heater_relay_on);
				
				//�����ɿع����
				Zero_Crossing_EX_Init();
				
				//��ʱ1s���ɿع�ȫ����
				BTM_Init();
			}
			
		
//			if(heater_relay_on==1)
//			{
//				//����¶ȱ��� HEAT ERROR ֱ�Ӽ��˿�ֵ P03   ��ѯ��ʽ
//				Scr_Driver_Check_Insurance();
//			}
		
			if(water_flow_flag < 1 && heater_relay_on==1)
			{
					heater_relay_on=0;
					Scr_Driver_Control_Heat_RLY(heater_relay_on);
			}
			
			//��ˮ�¶�
			ADC_Init(AIN8);
			ADCTempValue=ADC_Convert(); //����ADCת�������ת��ֵ
			ret = get_temp_table(ADCTempValue,&current_out_temp);
			
			//UART_SentChar(current_out_temp);
			
			//���ڴ�ӡlog�����ԡ�����
			if(b_btm_int_flag==false)
			{
				UART_SentChar(current_out_temp);
			}
			else
			{
				 //��ʱ��
				
				//IE1 |= 0x04;       //����BTM�ж�
				IE1 &= 0xfb;		//�ر�BTM�ж�
			}
			
			
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
			}
			
			
			//���ڽ��յ����ݣ�����
			Uart_Process();
			
	}
	
}

//ȫ�ֱ�����ʼ�������ⲿ��ȫ�ֱ���ֵ��ȷ���������߼�����
void AppInit()
{
	//��λ����ˮ��ͨ��Э����յ�������Э������
	//uchar heater_receive_data;

	//��ǰ��ˮ�����л�ֹͣ״̬ �̵������� 0��ֹͣ 1������
	heater_relay_on=0;

	//��ˮ���ڲ��쳣״̬
	ex_flag=Ex_Normal;

	//ˮ��״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
	 water_flow_flag=0;

	//��ѳ�ˮ�¶ȵ�ֵ
	good_temp_out_low=36;
	//��ѳ�ˮ�¶ȸ�ֵ
	good_temp_out_high=55;
	best_temp_out=37;

	current_out_temp=28; //��ǰ��ˮ�¶�
}