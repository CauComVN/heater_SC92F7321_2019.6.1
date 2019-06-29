#include "Function_Init.H"

void ADC_Interrupt(void) interrupt 6
{
    ADC_Interrupt_Handle();
}

void EX0_Interrupt() interrupt	0
{
    if(OS == 1)
    {
//        Leakage_EX0_Handle();
    }

    if(RX1==0 || TX1==0)
    {
    }
}

void EX1_Interrupt() interrupt	2
{
}

void EX2_Interrupt() interrupt	10
{
    //需要判断和区分
    if(!(P2CON&0x02) && (HALL_LLJ==1) ) {
        //INT25 端口P21 上升沿中断 HALL_LLJ P21=1
        Water_Detection_EX2_Handle();
    }
    else if(!(P2CON&0x01))
    {
        //INT24 端口P20  ZERO 上升沿和下降沿中断都支持
        Zero_Crossing_EX2_Handle();
    }
}

void Timer0_Interrupt() interrupt 1
{
    Water_Detection_Timer0_Handle();
}

void timer1_Interrupt() interrupt 3
{
    Timer1Int_Handle();
}

void Timer2Int(void) interrupt 5
{
}

void PWMInt(void) interrupt 8
{
}

void UartInt(void) interrupt 4
{
    UartInt_Handle();
}

