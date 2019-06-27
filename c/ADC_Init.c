#include "H/Function_Init.H"

#include "Function_Init.H"
#include "MATH.H"
#include "ADC.h"

//温度采集ADC转换标记
volatile bit AdcFlag = 0;

void ADC_Init(uint Channel);
void ADC_Init(uint Channel);
int ADC_Convert(void);
void ADC_Interrupt_Handle(void);

int get_temp_table(uint nADValue, uchar* temp);

////////////////////////////////////////////////////////////
//AIN9: AD in 进水采样温度
//AIN8: AD out 出水采用温度

/*****************************************************
*函数名称：void ADC_Init(uint Channel)
*函数功能：ADC初始化
*入口参数：Channel
*出口参数：void
*****************************************************/
void ADC_Init(uint Channel)
{
    ADCCON = 0X80|Channel;		//开启ADC，ADC采样频率为2M ,选择Channel位ADC采样口
    if(Channel<8)
    {
        ADCCFG0 = 1<<Channel;   //设置Channel作为采样口
    }
    else
    {
        ADCCFG1 = 1<<(Channel-8);   //设置Channel作为采样口
    }
    IE |= 0X40;        //开启ADC中断
//    EA = 1;
}

void ADC_Interrupt_Handle(void)
{
    ADCCON &= ~(0X10);  //清中断标志位
    AdcFlag = 1;
}

////////////////////////////////////////////////////////////
//二分法查表 0度 ~ 100度 电阻值从大到小逆序 对应温度值从小到大顺序
int search(uint arry[],uint n,uint key)
{
    uint low = 0,high = n-1; // n个数据 -1，high不能超数组元素个数的上限；
    uint mid,count=0;

    if( key<=arry[high] )
        return high;
    if( key>=arry[low] )
        return low;
    while(low<=high)
    {
        mid = (low+high)/2;
        if( key<=arry[mid] && key >= arry[mid+1] )
            return mid;
        if(key<arry[mid])
            low = mid + 1;
        else
            high = mid - 1;
    }
    return 0;
}

int get_temp_table(uint nADValue, uchar* temp)
{
    uint Rntc=50;//常温

    //计算电阻公式 x/c=Rntc/(Rup+Rntc)
    //c:如果ADC采集精度为N bit，对应满量程值为c值 2^N=c，主控芯片12位ADC， 2(12)=4096  5v=5v;
    //x:ADC采集值 x=TempAdd
    //Rup:上拉电阻值 10k
    //Rntc=10*nADValue/(4096-nADValue);
    //10*10*nADValue/(4096-nADValue)/10;

    Rntc=(uint)(10*nADValue/(4096-nADValue));

    if(Rntc>NTC_R_VALUE_MAX)
    {
        Rntc=NTC_R_VALUE_MAX;
    }
    if(Rntc<NTC_R_VALUE_MIN)
    {
        Rntc=NTC_R_VALUE_MIN;
    }

    *temp =search(bufTable_NTC_R, sizeof(bufTable_NTC_R)/2,	Rntc);

    return 0;
}


//8 AIN8 AD-out 出水温度
//7 AIN9 AD-in	进水温度
/*****************************************************
*函数名称：int ADC_Convert(void)
*函数功能：返回检测温度值
*入口参数：void
*出口参数：int
*****************************************************/
int ADC_Convert(void)
{
    uint Tad=0;
    uint MinAd=0x0fff,MaxAd=0x0000,TempAdd=0;
    uchar t=0;
    for(t=0; t<7; t++)
    {
        ADCCON |= 0x40; //开始ADC转换
        while(!AdcFlag); //等待 ADC转换完成
        AdcFlag = 0;
        Tad = ((unsigned int)(ADCVH<<4))+(ADCVL>>4); //取得一次转换值

        //第一次值不可靠，去掉
        if(t==0) {
            continue;
        }

        if(Tad>MaxAd) {
            MaxAd=Tad; //获得当前的最大值
        }
        if(Tad<MinAd) {
            MinAd=Tad; //获得当前的最小值
        }
        TempAdd+=Tad; //转换值累加
    }
    TempAdd-=MinAd; //去掉最小值

    TempAdd-=MaxAd; //去掉最大值

    //TempAdd=TempAdd/4; //求平均值
    TempAdd>>=2; //求平均值，如果检测7次，去掉3次，变为4，4是2次方，所以右移2

    return TempAdd;
}