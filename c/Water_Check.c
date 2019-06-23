#include "Function_Init.H"
#include "MATH.H"

//Water Detection

//����ˮ��������->�ⲿ�жϼ������ڲ���ʱ����ʱ
//INT25 P21 HALL_LLJ

///40ms

//��¼ˮ�����崥���жϸ���

volatile uint idata  numberPulse = 0;   


//ˮ��״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
volatile int idata water_flow_flag=0;

void Water_Detection_EX_Init(void);
//void Water_Detection_EXTI_Test(void);
void Water_Detection_Timer_Init(void);
//void Water_Detection_Timer_Test(void);
void Water_Detection_EX2_Handle();
void Water_Detection_Timer0_Handle();

/*****************************************************
*�������ƣ�void Water_Detection_EXTI_Test(void)
*�������ܣ��ⲿ�жϲ���
*��ڲ�����void
*���ڲ�����void
*****************************************************/
//void Water_Detection_EXTI_Test(void)
//{
//    Water_Detection_EX_Init();
//    while(1)
//    {
//    }
//}
/*****************************************************
*�������ƣ�void Water_Detection_EX_Init(void)
*�������ܣ��ⲿ�жϳ�ʼ��
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Water_Detection_EX_Init(void)
{
	
    //�����жϿ�25
    P2CON &= 0xfd;     //�ж�IO������Ϊ��������
    P2PH  |= 0x02;     //�ж�IO������Ϊ���������

    //����INT25�������ж�
    //�½�������
    INT2F &= 0x1f ;    //0000 xxxx  0�ر� 1ʹ��
    //����������
    INT2R |= 0X20 ;    //0000 xxxx  0�ر� 1ʹ��

    //�ⲿ�ж����ȼ�����
    IE1 |= 0x08;	//0000 x000  INT2ʹ��
    IP1 |= 0X00;
//    EA = 1;
	
	/**/
	
	
}

void Water_Detection_EX2_Handle()
{
    /**/
    //IE1 &= 0xf7;	//0000 x000  INT2ʹ�ر� �رջ���ˮ��������->�ⲿ�ж�

    //�ж�2����·���룬��Ҫ���� ������
    numberPulse=numberPulse+1; //����ˮ��������->�ⲿ�жϼ���

    //����ж�2����·���룬���������ػ����½�����ȷ�ϣ��������½���
//    if(ZERO == 1)
//    {
//    }
    if(HALL_LLJ == 1) //INT25 P21 ˮ��������
    {
    }

    //IE1 |= 0x08;	//0000 x000  INT2ʹ��

}

//////////////////////////////////////////////////////////////
void Water_Detection_Timer_Init(void);
/*****************************************************
*�������ƣ�void Water_Detection_Timer_Test(void);
*�������ܣ�T0/T1/T2����
*��ڲ�����void
*���ڲ�����void
*****************************************************/
//void Water_Detection_Timer_Test(void)
//{
//    Water_Detection_Timer_Init();
//    while(1)
//    {
//    }
//}
/*****************************************************
*�������ƣ�void Water_Detection_Timer_Init(void)
*�������ܣ�T0/T1/T2��ʼ��
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void Water_Detection_Timer_Init(void)
{
    //TMCON |= 0X01;    //------001 ;Timer0ѡ��ʱ��Fsys
		TMCON &=0xfe; //Fsys/12

    //T0����
    TMOD |= 0x01;                 //0000 0001;Timer0���ù�����ʽ1
		TMOD &=0xfb;

		TL0 = (65536 - 65000)%256;    //���ʱ�䣺ʱ��ΪFsys/12����40000*(1/(Fsys/12))=20ms;
		TH0 = (65536 - 65000)/256;

    TR0 = 0;
    ET0 = 1;//��ʱ��0����
    TR0 = 1;//�򿪶�ʱ��0

//    EA = 1;
}

void Water_Detection_Timer0_Handle()
{		
		TL0 = (65536 - 65000)%256;    //���ʱ�䣺ʱ��ΪFsys/12����40000*(1/(Fsys/12))=20ms;
		TH0 = (65536 - 65000)/256;
	
		//��ʱ�����ر��жϣ�ͳ�ƻ���ˮ��������->�ⲿ�жϼ���������ˮ��
    
    IE1 &= 0xf7;	//0000 x000  INT2ʹ�ر� �رջ���ˮ��������->�ⲿ�ж�
//    TR0=0; //�رն�ʱ��0

		if(numberPulse == 0){
			water_flow_flag=0; ////ˮ��״̬��� 0����ˮ�� 1����ˮ�� 2����ˮ��������
			
			ex_flag = Ex_Water_No_Flow;			
		}
		else if(numberPulse>0 && numberPulse <= waterThreshold){
			water_flow_flag=1;
			
			ex_flag = Ex_Water_Little_Flow;
		}
		else{
			water_flow_flag=2;
		}

    //����ˮ��������->�ⲿ�жϼ�������
    numberPulse=0;
		
		//��������ˮ���������жϣ��򿪶�ʱ��
		IE1 |= 0x08;	//0000 x000  INT2ʹ��
//		TR0 = 1;//�򿪶�ʱ��0	
}
