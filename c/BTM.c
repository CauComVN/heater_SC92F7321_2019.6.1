#include "H/Function_Init.H"
#include <math.h>

volatile bit b_btm_int_flag=0;

void BTM_Init(void);

/*****************************************************
*�������ƣ�void BTM_Init(void)
*�������ܣ�BTM��ʼ��
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void BTM_Init(void)
{
//	BTMCON = 0x00;  //������
//	BTMCON = 0x80;  //ÿ15.6ms����һ���ж�
//	BTMCON = 0x81;	//ÿ31.3ms����һ���ж�
//	BTMCON = 0x82;	//ÿ62.5ms����һ���ж�
//	BTMCON = 0x83;	//ÿ125ms����һ���ж�
//	BTMCON = 0x84;	//ÿ0.25s����һ���ж�
    BTMCON = 0x85;	//ÿ0.5s����һ���ж�
//	BTMCON = 0x86;	//ÿ1s����һ���ж�
//	BTMCON = 0x87;	//ÿ2s����һ���ж�
    //EA = 1;           //�������ж�
    IE1 |= 0x04;       //����BTM�ж�
}

/*****************************************************
*�������ƣ�void BTM_Int(void) interrupt 9
*�������ܣ��жϺ���
*��ڲ�����void
*���ڲ�����void
*****************************************************/
void BTM_Int(void) interrupt 9
{
    if(!(BTMCON&0X40))		//�жϱ�־λ�ж�
    {
        b_btm_int_flag=1;
    }
}

