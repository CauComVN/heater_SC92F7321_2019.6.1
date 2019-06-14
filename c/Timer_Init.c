#include "H/Function_Init.H"

uint time2_count=0;
uint time2_count_max=43;// 25 200us
int time2_curr=25;

uint scr_open_time_max=20000;
uint scr_open_time=0;//�͵�ƽ 8.6ms 17200---0  �ߵ�ƽ 10ms  20000---0
bit scr_open_flag=0;//�ɿع迪ͨ��־ ���ڹض϶�ʱ�� �ضϿɿع�
uint scr_adjust_step=2; //1us
uint scr_curr_time=0;

void Timer_Init(void);
/*****************************************************
*�������ƣ�void Timer_Test(void);
*�������ܣ�T0/T1/T2����
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Timer_Test(void)
{
    Timer_Init();
    while(1)
    {
    }
}
/*****************************************************
*�������ƣ�void Timer_Init(void)
*�������ܣ�T0/T1/T2��ʼ��
*��ڲ�����void
*���ڲ�����void
*****************************************************/

/*
void Timer_Init(void)
{
    TMCON = TMCON|0X04;    //------100 ;Tiemr2ѡ��ʱ��Fsys
		//TMCON &= 0Xfb;    //------000 ;Tiemr2ѡ��ʱ��Fsys/12

    //T2����
    T2MOD = 0x00;

    //T2CON = 0x00;	 //����Ϊ16λ���ؼĴ���
    T2CON &= 0x30;	 //����Ϊ16λ���ؼĴ���

//	RCAP2H = (65536-48000)/256;     //���ʱ�䣺ʱ��ΪFsys����48000*��1/Fsys��=2ms;
//	RCAP2L = (65536-48000)%256;
//	RCAP2H = (65536-24000)/256;     //���ʱ�䣺ʱ��ΪFsys����24000*��1/Fsys��=1ms;
//	RCAP2L = (65536-24000)%256;
//	RCAP2H = (65536-2400)/256;     //���ʱ�䣺ʱ��ΪFsys����2400*��1/Fsys��=100us;
//	RCAP2L = (65536-2400)%256;
    RCAP2H = (65536-4800)/256;     //���ʱ�䣺ʱ��ΪFsys����4800*��1/Fsys��=200us;
    RCAP2L = (65536-4800)%256;

    TR2 = 0;
    ET2 = 1;//��ʱ��2����
    TR2 = 1;//�򿪶�ʱ��2

//	HEAT_TRA=1;

		//�������ֵ
		time2_count_max=open_max-time2_curr/4;

    time2_count=0;
    if(time2_curr==0)
    {
        HEAT_TRA=1;
    }
    else
    {
        HEAT_TRA=0;
    }

    EA = 1;
}

void Timer2Int_Handle()
{
    TF2 = 0;   //�������

    //���ڴ�ӡlog�����ԡ����� ---->>>>> ���Բ��ܿ�������ʱ̫����һ���ᵼ������
    //UART_SentChar(time2_curr);

    time2_count=time2_count+1;
    if(time2_curr == 0)
    {
    }
    else
    {
        if(time2_count<time2_curr)
        {
            if(HEAT_TRA!=0)
            {
                HEAT_TRA=0;
            }
        }
        else if(time2_count>=time2_curr)
        {
            if(HEAT_TRA!=1)
            {
                HEAT_TRA=1;
            }
        }
								
				if(time2_count>=time2_count_max)
				{
						if(HEAT_TRA!=0)
						{
								HEAT_TRA=0;
						}
						if(TR2!=0)
						{
								TR2 = 0;
						}
				}
    }
}

*/


void Timer_Init(void)
{
    //TMCON = TMCON|0X04;    //------100 ;Tiemr2ѡ��ʱ��Fsys
		TMCON &= 0Xfb;    //------000 ;Tiemr2ѡ��ʱ��Fsys/12

    //T2����
    T2MOD = 0x00;

    //T2CON = 0x00;	 //����Ϊ16λ���ؼĴ���
    T2CON &= 0x30;	 //����Ϊ16λ���ؼĴ���
	
		//�ɿع迪ͨʱ���
		RCAP2H = (65536-scr_open_time)/256;     //���ʱ�䣺ʱ��ΪFsys����scr_open_time*��1/(Fsys/12)��=scr_open_time*0.5us;
		RCAP2L = (65536-scr_open_time)%256;		
		
    TR2 = 0;
    ET2 = 1;//��ʱ��2����
    TR2 = 1;//�򿪶�ʱ��2

		//�رտɿع� ���ÿɿع迪ͨ���
		HEAT_TRA=0;
		scr_open_flag=0;

    EA = 1;
}

void Timer2Int_Handle()
{
    TF2 = 0;   //�������
	
		if(scr_open_flag==0)
		{
			if(HEAT_TRA!=1)
			{
					HEAT_TRA=1;
			}			
			scr_open_flag=1;			
			
			//�ɿع迪ͨʱ���֮�󣬼���ضϿɿع�Ͷ�ʱ��ʱ�䣬Ȼ�����ö�ʱ��
			RCAP2H = (65536-(scr_open_time_max-scr_open_time))/256;     //���ʱ�䣺ʱ��ΪFsys����scr_open_time*��1/(Fsys/12)��=scr_open_time*0.5us;
			RCAP2L = (65536-(scr_open_time_max-scr_open_time))%256;
		}
		else
		{
			if(HEAT_TRA!=0)
			{
					HEAT_TRA=0;
			}
			if(TR2!=0)
			{
					TR2 = 0;
			}			
			scr_open_flag=0;
		}
}
