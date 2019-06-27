#include "H/Function_Init.H"

#include "Function_Init.H"
#include "MATH.H"
#include "ADC.h"

//�¶Ȳɼ�ADCת�����
volatile bit AdcFlag = 0;

void ADC_Init(uint Channel);
void ADC_Init(uint Channel);
int ADC_Convert(void);
void ADC_Interrupt_Handle(void);

int get_temp_table(uint nADValue, uchar* temp);

////////////////////////////////////////////////////////////
//AIN9: AD in ��ˮ�����¶�
//AIN8: AD out ��ˮ�����¶�

/*****************************************************
*�������ƣ�void ADC_Init(uint Channel)
*�������ܣ�ADC��ʼ��
*��ڲ�����Channel
*���ڲ�����void
*****************************************************/
void ADC_Init(uint Channel)
{
    ADCCON = 0X80|Channel;		//����ADC��ADC����Ƶ��Ϊ2M ,ѡ��ChannelλADC������
    if(Channel<8)
    {
        ADCCFG0 = 1<<Channel;   //����Channel��Ϊ������
    }
    else
    {
        ADCCFG1 = 1<<(Channel-8);   //����Channel��Ϊ������
    }
    IE |= 0X40;        //����ADC�ж�
//    EA = 1;
}

void ADC_Interrupt_Handle(void)
{
    ADCCON &= ~(0X10);  //���жϱ�־λ
    AdcFlag = 1;
}

////////////////////////////////////////////////////////////
//���ַ���� 0�� ~ 100�� ����ֵ�Ӵ�С���� ��Ӧ�¶�ֵ��С����˳��
int search(uint arry[],uint n,uint key)
{
    uint low = 0,high = n-1; // n������ -1��high���ܳ�����Ԫ�ظ��������ޣ�
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
    uint Rntc=50;//����

    //������蹫ʽ x/c=Rntc/(Rup+Rntc)
    //c:���ADC�ɼ�����ΪN bit����Ӧ������ֵΪcֵ 2^N=c������оƬ12λADC�� 2(12)=4096  5v=5v;
    //x:ADC�ɼ�ֵ x=TempAdd
    //Rup:��������ֵ 10k
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


//8 AIN8 AD-out ��ˮ�¶�
//7 AIN9 AD-in	��ˮ�¶�
/*****************************************************
*�������ƣ�int ADC_Convert(void)
*�������ܣ����ؼ���¶�ֵ
*��ڲ�����void
*���ڲ�����int
*****************************************************/
int ADC_Convert(void)
{
    uint Tad=0;
    uint MinAd=0x0fff,MaxAd=0x0000,TempAdd=0;
    uchar t=0;
    for(t=0; t<7; t++)
    {
        ADCCON |= 0x40; //��ʼADCת��
        while(!AdcFlag); //�ȴ� ADCת�����
        AdcFlag = 0;
        Tad = ((unsigned int)(ADCVH<<4))+(ADCVL>>4); //ȡ��һ��ת��ֵ

        //��һ��ֵ���ɿ���ȥ��
        if(t==0) {
            continue;
        }

        if(Tad>MaxAd) {
            MaxAd=Tad; //��õ�ǰ�����ֵ
        }
        if(Tad<MinAd) {
            MinAd=Tad; //��õ�ǰ����Сֵ
        }
        TempAdd+=Tad; //ת��ֵ�ۼ�
    }
    TempAdd-=MinAd; //ȥ����Сֵ

    TempAdd-=MaxAd; //ȥ�����ֵ

    //TempAdd=TempAdd/4; //��ƽ��ֵ
    TempAdd>>=2; //��ƽ��ֵ��������7�Σ�ȥ��3�Σ���Ϊ4��4��2�η�����������2

    return TempAdd;
}