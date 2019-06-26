#include "H/Function_Init.H"

uint  idata  scr_open_time_max=zero_period_high_time;
uint  scr_open_time=0;//17200;//20000;//5;//�͵�ƽ 8.6ms 17200---0  �ߵ�ƽ 10ms  20000---0
bit scr_open_flag=0;//�ɿع迪ͨ��־ ���ڹض϶�ʱ�� �ضϿɿع�

void Timer_Init(void);

/*****************************************************
*�������ƣ�void Timer_Init(void)
*�������ܣ�T0/T1/T2��ʼ��
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Timer_Init(void)
{
    TMCON &= 0xfd;   //ʱ��Fsys/12
    TMOD &=0x9f;
    TMOD |=0x10;


    //�رտɿع� ���ÿɿع迪ͨ���
    HEAT_TRA=0;
    scr_open_flag=0;

    TL1 = (65536 - scr_open_time)%256;     //���ʱ�䣺ʱ��ΪFsys/12����scr_open_time*��1/(Fsys/12)��=scr_open_time*0.5us;
    TH1 = (65536 - scr_open_time)/256;
    TR1 = 0;
    ET1 = 1;//��ʱ��0����
    TR1 = 1;//�򿪶�ʱ��0

//     EA = 1;
}

void Timer1Int_Handle()
{
    if(scr_open_flag==0)
    {

        scr_open_flag=1;

        if(HEAT_TRA!=1)
        {
            HEAT_TRA=1;
        }

        //�ɿع迪ͨʱ���֮�󣬼���ضϿɿع�Ͷ�ʱ��ʱ�䣬Ȼ�����ö�ʱ��
        //400us
        TL1 = 224;// (65536 - 800)%256;     //���ʱ�䣺ʱ��ΪFsys/12����scr_open_time*��1/(Fsys/12)��=scr_open_time*0.5us;
        TH1 = 252;//(65536 - 800)/256;
    }
    else
    {
        if(HEAT_TRA!=0)
        {
            HEAT_TRA=0;
        }
        if(TR1!=0)
        {
            //ET1 = 0;
            TR1 = 0;
        }
        scr_open_flag=0;
    }
}

