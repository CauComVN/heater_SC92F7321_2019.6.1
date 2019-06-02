#include "Function_Init.H"

void Serial_Key_Init(void);
void Serial_Key_Test(void);
void Serial_Key_EX0_Handle();

void Serial_Key_Test(void)
{
		Serial_Key_Init();
    while(1)
    {        
    }
}

void Serial_Key_Init(void)
{
		//�ô���P12 P13ģ����ƹ��� �½��ش���
		////////////////////////////////////////////////////////////
		//�����жϿ�INT02 INT03 P12 P13 �½���
		P1CON &= 0XF3;     //�ж�IO������Ϊ��������
		P1PH  |= 0x0c;     //�ж�IO������Ϊ���������	 
		
			//�½�������	
		INT0F |= 0x0c;   //xxxx 0000  0�ر� 1ʹ��
			//����������	
		INT0R &= 0Xf3 ;    //xxxx 0000  0�ر� 1ʹ�� 
		
		//�ⲿ�ж����ȼ�����
		IE  |= 0x01;	//0000 0x0x  INT0ʹ��
		IP  |= 0X00;
		
		EA = 1;
		//////////////////////////////////////////////////////////		
}

void Serial_Key_EX0_Handle()
{	
	P00 = ~P00;
	if(P12==0) //���ӹ���
	{
		
	}
	if(P13==0) //���͹���
	{
	}
}