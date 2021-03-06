//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"
#include <stdio.h>
#include <math.h>

//INT24 P20 ZERO

//HEAT TRA PWM1 P01

//HEAT RLY 继电器控制 P02

//T/S1 温度保险 HEAT ERROR COM3/P03
//如果检测到温度过高，T/S1会自动跳闸，断开电路
//继电器停止工作 保护整个可控硅驱动电路
//HEAT ERROR 为输入端，如果等于高电平，表明热水器温度过高
//如果为低电平，表明热水器温度在正常范围内

//抗饱和积分
//8位单片机不支持浮点运算，提高控制精度，乘10处理
int idata umax=500;//=50;//理想最大温度值
int idata umin=290;//=29;//理想最小温度值

int idata Out1=0;  		//记录上次输出
int idata ERR=0;       //当前误差
int idata ERR1=0;      //上次误差
int idata ERR2=0;      //上上次误差
int idata integral=0;		//积分分离

// 0:无功率 1：全功率
volatile uchar heater_power_status=0;
//当前热水器运行或停止状态 控制继电器动作 0：停止 1：运行
volatile bit heater_relay_on=0;
//开始PID算法标记
volatile bit b_start_pid=0;
////热水器内部异常状态
Enum_Ex_Flag idata ex_flag=Ex_Normal;
//35度~60度 自动调节  最佳：40 - 50
//8位单片机不支持浮点运算，提高控制精度，乘10处理
int idata best_temp_out=380;//38;
//当前出水温度
//8位单片机不支持浮点运算，提高控制精度，乘10处理
volatile char  current_out_temp=290;//29;
//可控硅触发时间最大值
uint idata scr_open_time_max=zero_period_low_time;
//实时计算的可控硅触发时间
volatile int  scr_curr_time=0;
//实时计算的可控硅触发时间副本，用于解决主循环和过零检测中断内全局变量scr_curr_time问题
volatile int  scr_tune_time=0;

void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();

//软件延时
void soft_delay(uint n);

// Sv设定温度值  Pv当前温度值
void PIDCalc(int Sv,int Pv);

int Scr_Driver_Check_Insurance();//检测温度保险
void Scr_Driver_Control_Heat_RLY(int on);//继电器控制 HEAT RLY P02

/*****************************************************
*函数名称：void Zero_Crossing_EX_Init(void)
*函数功能：外部中断初始化
*入口参数：void
*出口参数：void
*****************************************************/
void Zero_Crossing_EX_Init(void)
{
    P2CON &= 0XFE;     //中断IO口设置为高阻输入
    //P2PH  |= 0x01;     //中断IO口设置为高阻带上拉
    P2PH &= 0xfe; //不带上拉 外部有上拉电阻

    //配置中断口INT24  上升沿和下降沿都支持
    //下降沿设置
    INT2F |= 0x10;  //0000 xxxx  0关闭 1使能 打开下降沿
    //INT2F &= 0x2F; //关闭下降沿
    //上升沿设置
    INT2R |= 0X10 ;    //0000 xxxx  0关闭 1使能

    //外部中断优先级设�
    IE1 |= 0x08;	//0000 x000  INT2使能
    IP1 |= 0X00;
//    EA = 1;
}

void Zero_Crossing_EX2_Handle()
{
    if(heater_power_status==1)
    {
        //全功率
        if(HEAT_TRA!=1)
            HEAT_TRA=1;

        //定时器关闭
        if(TR1!=0)
            TR1 = 0;
    }
    else if(heater_power_status==0)
    {
        //无功率
        if(HEAT_TRA!=0)
            HEAT_TRA=0;

        //定时器关闭
        if(TR1!=0)
            TR1 = 0;
    }
    else
    {
        if(ZERO==1)
        {
            scr_open_time_max=zero_period_high_time;
        }
        else
        {
            scr_open_time_max=zero_period_low_time;
        }

        if(scr_tune_time>0 && scr_tune_time<=(scr_open_time_max-zero_peroid_last_time))
        {
            if(HEAT_TRA!=0)
            {
                HEAT_TRA=0;
            }

            if(scr_open_time != scr_tune_time)
            {
                scr_open_time=scr_tune_time;
            }

            Timer_Init();
        }
        else
        {
            if(HEAT_TRA!=0)
                HEAT_TRA=0;

            //定时器关闭
            if(TR1!=0)
                TR1 = 0;
        }
    }
}

//检测温度保险 HEAT ERROR 直接检测端口值 P03   轮询方式
int Scr_Driver_Check_Insurance()
{
    if(HEAT_ERROR==0)
    {
        //温度正常范围内，温度保险不跳闸
        return 0;
    }
    else if(HEAT_ERROR==1)
    {
        //设置温度保险已跳闸标记
        ex_flag = Ex_Thermal_Switch_Error;
        return -1;
    }
    return 0;
}

//继电器控制 HEAT RLY P02
void Scr_Driver_Control_Heat_RLY(int on)
{
    P0VO = P0VO&0xfb; //P02端口设置成普通I/O口  1111 1011
    if(on == 1)
    {
        HEAT_RLY=1;
    }
    else
    {
        HEAT_RLY=0;
    }

    //软件延时，保证heater_relay_on变量更新完成，避免主循环逻辑错误或者混乱
    soft_delay(48000); // (1+1+（1+2）*48000)*0.5us=72001us=72.001ms
}


//软件延时 https://blog.csdn.net/nanfeibuyi/article/details/83577641
/*
根据循环条件，INC指令和CJNE指令总共要执行n次,共消耗机器周期（1+2）*n，
加上CLR指令和MOV指令，循环程序总共消耗机器周期：1+1+（1+2）*n
如果单片机的晶振频率为24MHz，则机器周期 = 12*（1/24）us = 0.5us。
那么for循环程序耗时为(1+1+（1+2）*n)*0.5us。
*/
void soft_delay(uint n)
{
    uint k;
    for(k=0; k<n; k++)
    {
        _nop_();  //代表运行一个机器周期 如果这个单片机的晶振是12M的，那么这调代码会运行1US
//		;
    }
}

// Sv设定温度值  Pv当前温度值
void PIDCalc(int Sv,int Pv)
{
    int idata index=0;

    int idata DERR1 = 0;
    int idata DERR2 = 0;

    int idata Pout = 0;       //比例结果
    int idata Iout = 0;       //积分结果
    int idata Dout = 0;       //微分结果
    int idata Out = 0; 				//总输出

    ERR=Sv-Pv; //算出当前误差
    DERR1 = ERR - ERR1;   //上次

    //DERR2 = ERR - 2*ERR1 + ERR2; //上上次  //不要在主程序和中断程序中同时做8bit以上的乘除法运算，会出错
    DERR2= ERR  + ERR2;
    DERR2= DERR2 - ERR1;
    DERR2= DERR2 - ERR1;

    //Kp
    Pout = Kp*ERR;

    //Ki 积分分离过程 抗积分饱和
    if(Pv>umax) { //灰色底色表示抗积分饱和的实现

        if(abs(ERR)>1) {    //蓝色标注为积分分离过程
            index=0;
        } else {
            index=1;
            if(ERR<0) {
                integral+=ERR;
            }
        }
    } else if(Pv<umin) {
        if(abs(ERR)>1) {    //积分分离过程
            index=0;
        } else {
            index=1;
            if(ERR>0) {
                integral+=ERR;
            }
        }
    } else {
        if(abs(ERR)>1) {                  //积分分离过程
            index=0;
        } else {
            index=1;
            integral+=ERR;
        }
    }
    Iout = index*Ti*integral;

    //Kd
    Dout = DERR1*Td;

    Out = Out1+ Pout;
    Out = Out+ Iout;
    Out = Out+ Dout;

    Out1 = Out;      //记录这次输出的值

    ERR2 = ERR1;    //记录误差
    ERR1 = ERR;     //记录误差

    //printf("%d\n",Out);

    //PID算法控制
    if(heater_power_status!=2)
        heater_power_status=2;

    //一定要相减，因为功率调节是相反的，scr_curr_time越小，功率越大
    scr_curr_time = scr_curr_time - Out;  //Out=50 Out*74=3700
    if(scr_curr_time<1)
    {
        if(heater_power_status!=1)
            heater_power_status=1;
    }
    else if(scr_curr_time>zero_period_low_time-zero_peroid_last_time)
    {
        if(heater_power_status!=0)
            heater_power_status=0;
    }
    else
    {
        //scr_curr_time复制给副本scr_tune_time，避开主循环和过零中断共享全局变量导致的严重问题，这是用操作系统的方法
        //关闭过零中断会同时关闭水流量计中断，因为都是INT2中断，导致更大的问题，此种方法在这里不可行
        //https://blog.csdn.net/dijindeng/article/details/50426028
        do {
            scr_tune_time=scr_curr_time;
        } while(scr_tune_time != scr_curr_time);
    }

    //printf("%d\n",scr_curr_time);
}
