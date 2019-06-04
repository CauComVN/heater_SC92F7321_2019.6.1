#include "H/Function_Init.H"

uint current_PWMDTY1=0;//6;//15; //��ǰ���ʵ���PWMռ�ձ� x/186

uint duty_app_time=9000; //9ms
uint max_pwm_duty=9; //10ms 50Hz�е���������9ms�����PWM������Ŀ duty_app_time*24/128/180=9000*24/128/180=9.375
uint step_app_duty=2; //0 -- 180 ���ڲ���
uint curr_app_duty=30; //0 -- 180
uint max_app_duty=180; //0 -- 180
uint low_pwm_duty_cout=0; //m_duty��PWM������ĿΪ�͵�ƽ
uint curr_low_pwm_duty_count=0;

//HEAT TRA PWM1 ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� Duty���� 2�����ٹ��� Duty����
void Scr_Driver_PWM_Adjust(uint flag);

void Scr_Driver_PWM_Init(void);
void Scr_Driver_PWMInt_Handle();
/*****************************************************
*�������ƣ�void Scr_Driver_PWM_Test(void)
*�������ܣ�PWM����
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Scr_Driver_PWM_Test(void)
{
	Scr_Driver_PWM_Init();
	while(1)
	{
	}
}
/*****************************************************
*�������ƣ�void PWM_Init(void)
*�������ܣ�PWM��ʼ��
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Scr_Driver_PWM_Init(void)
{
	//HEAT TRA PWM1
	
	current_PWMDTY1=0;
	
	curr_low_pwm_duty_count=0;
	low_pwm_duty_cout=(duty_app_time-50*curr_app_duty)*24/10240; // 128*180=10240
	
	//=========================================================================================
	
	//50hz 20ms �������10ms 10ms/10=1ms PWMʱ��ΪFsys/128 187*128/24=997.33 187-1=186
	PWMCON  = 0x16;		//PWM�����IO��PWMʱ��ΪFsys/128 HEAT TRA PWM1
	PWMPRD  = 180;		//PWM����=(186+1)*(1*128/24us)=997.33��=1ms;
	PWMCFG  = 0x10;//0x10;//0x00;		//PWM1�������,�����P01  INV 1:������� 0����������� 000x
	PWMDTY1 = current_PWMDTY1;//15;     //PWM1��Duty = 15/60 =1/4
	PWMDTYA = 0x00;		//PWMռ�ձ�΢���Ĵ��������ﲻ΢��
	PWMCON |= 0x80;     //����PWM
	IE1 |= 0x02;        //����PWM�ж�
	EA = 1;
}

////HEAT TRA PWM1 ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� Duty���� 2�����ٹ��� Duty����
//void Scr_Driver_PWM_Adjust(uint flag)
//{
//	if(flag==1 || flag==2)
//	{
//		EA=0;
//		IE1 &= 0xfd;        //�ر�PWM�ж�		
//		
//		if(flag==1){ //���ӹ���
//			current_PWMDTY1=current_PWMDTY1+10;
//			if(current_PWMDTY1>185)
//			{
//				current_PWMDTY1=185;
//			}
//		}
//		else if(flag==2)
//		{
//			current_PWMDTY1=current_PWMDTY1-10;
//			if(current_PWMDTY1<1)
//			{				
//				current_PWMDTY1=1;
//			}
//		}			
//		
//		IE1 |= 0x02;        //����PWM�ж�
//		EA=1;
//	}
//}

////�жϴ���
//void Scr_Driver_PWMInt_Handle()
//{
//    //���ݳ�ˮ/��ˮ�¶��Զ�����PWM1��Duty
//	int i=0;
//	
//	PWMDTY1 = current_PWMDTY1;
//}


//HEAT TRA PWM1 ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� Duty���� 2�����ٹ��� Duty����
void Scr_Driver_PWM_Adjust(uint flag)
{
	if(flag==1 || flag==2)
	{
		EA=0;
		IE1 &= 0xfd;        //�ر�PWM�ж�		
		
		if(flag==1){ //���ӹ���
			curr_app_duty=curr_app_duty+step_app_duty;
			if(curr_app_duty>max_app_duty)
			{
				curr_app_duty=max_app_duty;
			}
			
			low_pwm_duty_cout=(duty_app_time-50*curr_app_duty)*24/10240; // 128*180=10240
		}
		else if(flag==2)
		{
			curr_app_duty=curr_app_duty-step_app_duty;
			if(curr_app_duty<1)
			{				
				curr_app_duty=0;
			}
			
			low_pwm_duty_cout=(duty_app_time-50*curr_app_duty)*24/10240; // 128*180=10240
		}			
		
		IE1 |= 0x02;        //����PWM�ж�
		EA=1;
	}
}

//�жϴ���
void Scr_Driver_PWMInt_Handle()
{
	curr_low_pwm_duty_count++;
	if(curr_low_pwm_duty_count<=low_pwm_duty_cout)
	{	
		//pwmռ�ձ�0���͵�ƽ
		current_PWMDTY1=0;
	}
	else
	{
		//pwmռ�ձ�Ϊ180���ߵ�ƽ
		current_PWMDTY1=180;
	}
	
	if(current_PWMDTY1>max_pwm_duty)
	{
		//�ر�pwm�жϣ��ȴ���һ�ι����⵽��
		IE1 &= 0xfd;        //�ر�PWM�ж�	
	}	
}