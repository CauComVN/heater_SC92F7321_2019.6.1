#include "H/Function_Init.H"

uint current_PWMDTY1=6;//15; //��ǰ���ʵ���PWMռ�ձ�

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
	
	//50hz 20ms �������10ms 10ms/10=1ms PWMʱ��ΪFsys/128 187*128/24=997.33 187-1=186
	PWMCON  = 0x16;		//PWM�����IO��PWMʱ��ΪFsys/128 HEAT TRA PWM1
	PWMPRD  = 186;		//PWM����=(186+1)*(1*128/24us)=997.33��=1ms;
	PWMCFG  = 0x10;		//PWM1�������,�����P01
	PWMDTY1 = current_PWMDTY1;//15;     //PWM1��Duty = 15/60 =1/4
	PWMDTYA = 0x00;		//PWMռ�ձ�΢���Ĵ��������ﲻ΢��
	PWMCON |= 0x80;     //����PWM
	IE1 |= 0x02;        //����PWM�ж�
	EA = 1;
}

//HEAT TRA PWM1 ���ʵ��ڷ�ʽ flag 0:���õ��� 1�����ӹ��� Duty���� 2�����ٹ��� Duty����
void Scr_Driver_PWM_Adjust(uint flag)
{
	if(flag==1 || flag==2)
	{
		EA=0;
		IE1 &= 0xfd;        //�ر�PWM�ж�		
		
		if(flag==1){ //���ӹ���
			PWMDTY1=current_PWMDTY1+3;
			if(PWMDTY1>59)
			{
				PWMDTY1=59;
			}
		}
		else if(flag==2)
		{
			PWMDTY1=current_PWMDTY1-3;
			if(PWMDTY1<1)
			{				
				PWMDTY1=1;
			}
		}			
		
		IE1 |= 0x02;        //����PWM�ж�
		EA=1;
	}
}

//�жϴ���
void Scr_Driver_PWMInt_Handle()
{
    //���ݳ�ˮ/��ˮ�¶��Զ�����PWM1��Duty
	int i=0;
}