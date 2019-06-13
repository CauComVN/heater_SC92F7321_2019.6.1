#include "H/Function_Init.H"

uint time2_count=0;
uint time2_count_max=43;// 25 200us
int time2_curr=25;

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
void Timer_Init(void)
{
    TMCON = TMCON|0X04;    //------100 ;Tiemr2ѡ��ʱ��Fsys

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

//	P01=1;

		//�������ֵ
		time2_count_max=open_max-time2_curr/4;

    time2_count=0;
    if(time2_curr==0)
    {
        P01=1;
    }
    else
    {
        P01=0;
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
            if(P01!=0)
            {
                P01=0;
            }
        }
        else if(time2_count>=time2_curr)
        {
            if(P01!=1)
            {
                P01=1;
//								TR2 = 0;
            }
        }

				/*
        if(time2_curr>0&&time2_curr<time2_count_max-18) {
            if(time2_count>=time2_count_max)
            {
                if(P01!=0)
                {
                    P01=0;
                }
                if(TR2!=0)
                {
                    TR2 = 0;
                }
            }
        }
        else
        {
            if(time2_count>=time2_count_max-18)
            {
                if(time2_count>=time2_count_max-8)//40
                {
                    if(P01!=0)
                    {
                        P01=0;
                    }
                    if(TR2!=0)
                    {
                        TR2 = 0;
                    }
                }
            }
        }
				
				*/
				
				if(time2_count>=time2_count_max)
				{
						if(P01!=0)
						{
								P01=0;
						}
						if(TR2!=0)
						{
								TR2 = 0;
						}
				}
    }
}
