#include "H/Function_Init.H"

uint time2_count=0;
uint time2_count_max=43;// 25 200us
int time2_curr=25;

void Timer_Init(void);
/*****************************************************
*函数名称：void Timer_Test(void);
*函数功能：T0/T1/T2测试
*入口参数：void
*出口参数：void
*****************************************************/
void Timer_Test(void)
{
    Timer_Init();
    while(1)
    {
    }
}
/*****************************************************
*函数名称：void Timer_Init(void)
*函数功能：T0/T1/T2初始化
*入口参数：void
*出口参数：void
*****************************************************/
void Timer_Init(void)
{
    TMCON = TMCON|0X04;    //------100 ;Tiemr2选择时钟Fsys

    //T2设置
    T2MOD = 0x00;

    //T2CON = 0x00;	 //设置为16位重载寄存器
    T2CON &= 0x30;	 //设置为16位重载寄存器

//	RCAP2H = (65536-48000)/256;     //溢出时间：时钟为Fsys，则48000*（1/Fsys）=2ms;
//	RCAP2L = (65536-48000)%256;
//	RCAP2H = (65536-24000)/256;     //溢出时间：时钟为Fsys，则24000*（1/Fsys）=1ms;
//	RCAP2L = (65536-24000)%256;
//	RCAP2H = (65536-2400)/256;     //溢出时间：时钟为Fsys，则2400*（1/Fsys）=100us;
//	RCAP2L = (65536-2400)%256;
    RCAP2H = (65536-4800)/256;     //溢出时间：时钟为Fsys，则4800*（1/Fsys）=200us;
    RCAP2L = (65536-4800)%256;

    TR2 = 0;
    ET2 = 1;//定时器2允许
    TR2 = 1;//打开定时器2

//	P01=1;

		//设置最大值
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
    TF2 = 0;   //溢出清零

    //串口打印log，调试。。。 ---->>>>> 绝对不能开启，耗时太长，一定会导致问题
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
