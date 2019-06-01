#include "Function_Init.H"
#include "MATH.H"


////////////////////////////////////////////////////////
//配置中断口INT00，漏电保护输出，端口p1.0

void Leakage_EXTI_Test(void);
void Leakage_EX_Init(void);
void Leakage_EX0_Handle(void);

/*****************************************************
*函数名称：void Leakage_EXTI_Test(void)
*函数功能：外部中断测试
*入口参数：void
*出口参数：void
*****************************************************/
void Leakage_EXTI_Test(void)
{
	Leakage_EX_Init();
	while(1)
	{					
	}
}
/*****************************************************
*函数名称：void Leakage_EX_Init(void)
*函数功能：外部中断初始化
*入口参数：void
*出口参数：void
*****************************************************/
void Leakage_EX_Init(void)
{	
	//配置中断口INT00 P10
	P1CON &= 0XFE;     //中断IO口设置为高阻输入
	P1PH  |= 0x01;     //中断IO口设置为高阻带上拉
 
	//配置INT00上升沿中断
    //下降沿设置	
	INT0F &= 0x0e;//= 0X00 ;    //xxxx 0000  0关闭 1使能
    //上升沿设置	
	INT0R |= 0X01 ;    //xxxx 0000  0关闭 1使能 
	
	//外部中断优先级设置
	IE  |= 0x01;	//0000 0x0x  INT0使能
	IP  |= 0X00;
	
	EA = 1;
}

void Leakage_EX0_Handle(void)
{
	//设置INT00上升沿触发中断，如果进入中断后有漏电脉冲，P10 == 1，
	//并且大于20ms，则漏电条件成立
	if(P10 == 1)
	{
		//设置漏电检测异常标志
		Ex_Flag = Ex_Leakage;
	}
}
