//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"

//INT24 P20

//HEAT TRA PWM1 P01

//HEAT RLY ¼ÌµçÆ÷¿ØÖÆ P02

//T/S1 ÎÂ¶È±£ÏÕ HEAT ERROR COM3/P03
//Èç¹û¼ì²âµ½ÎÂ¶È¹ý¸ß£¬T/S1»á×Ô¶¯ÌøÕ¢£¬¶Ï¿ªµçÂ· 
//¼ÌµçÆ÷Í£Ö¹¹¤×÷ ±£»¤Õû¸ö¿É¿Ø¹èÇý¶¯µçÂ·
//HEAT ERROR ÎªÊäÈë¶Ë£¬Èç¹ûµÈÓÚ¸ßµçÆ½£¬±íÃ÷ÈÈË®Æ÷ÎÂ¶È¹ý¸ß
//Èç¹ûÎªµÍµçÆ½£¬±íÃ÷ÈÈË®Æ÷ÎÂ¶ÈÔÚÕý³£·¶Î§ÄÚ

//µ±Ç°ÈÈË®Æ÷ÔËÐÐ»òÍ£Ö¹×´Ì¬ ¿ØÖÆ¼ÌµçÆ÷¶¯×÷ 0£ºÍ£Ö¹ 1£ºÔËÐÐ
bit heater_relay_on=0;

////ÈÈË®Æ÷ÄÚ²¿Òì³£×´Ì¬
Enum_Ex_Flag Ex_Flag;

//35¶È~60¶È ×Ô¶¯µ÷½Ú  ×î¼Ñ£º40 - 55
uint good_temperature_out_low=40;
uint good_temperature_out_high=50;

void Zero_Crossing_EXTI_Test(void);
void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();


int Scr_Driver_Check_Heat_Error();//¼ì²âÎÂ¶È±£ÏÕ
void Scr_Driver_Control_Heat_RLY(int on);//¼ÌµçÆ÷¿ØÖÆ HEAT RLY P02

//HEAT TRA PWM1 ¹¦ÂÊµ÷½Ú·½Ê½ flag 0:²»ÓÃµ÷½Ú 1£ºÔö¼Ó¹¦ÂÊ DutyÔö´ó 2£º¼õÉÙ¹¦ÂÊ Duty¼õÉÙ
void Scr_Driver_PWM_Adjust(uint flag);

uchar Zero_Crossing_INT1_flag = 0x00;
/*****************************************************
*º¯ÊýÃû³Æ£ºvoid EXTI_Test(void)
*º¯Êý¹¦ÄÜ£ºÍâ²¿ÖÐ¶Ï²âÊÔ
*Èë¿Ú²ÎÊý£ºvoid
*³ö¿Ú²ÎÊý£ºvoid
*****************************************************/
void Zero_Crossing_EXTI_Test(void)
{
    Zero_Crossing_EX_Init();
    while(1)
    {
    }
}
/*****************************************************
*º¯ÊýÃû³Æ£ºvoid EX_Init(void)
*º¯Êý¹¦ÄÜ£ºÍâ²¿ÖÐ¶Ï³õÊ¼»¯
*Èë¿Ú²ÎÊý£ºvoid
*³ö¿Ú²ÎÊý£ºvoid
*****************************************************/
void Zero_Crossing_EX_Init(void)
{
    //ÅäÖÃÖÐ¶Ï¿ÚINT24
    P2CON &= 0XFE;     //ÖÐ¶ÏIO¿ÚÉèÖÃÎª¸ß×èÊäÈë
    P2PH  |= 0x01;     //ÖÐ¶ÏIO¿ÚÉèÖÃÎª¸ß×è´øÉÏÀ­

    //INT24ÉÏÉýÖÐ¶Ï
    //ÏÂ½µÑØÉèÖÃ
    INT2F &= 0x2F; //= 0X00 ;    //0000 xxxx  0¹Ø±Õ 1Ê¹ÄÜ
    //ÉÏÉýÑØÉèÖÃ
    INT2R |= 0X10 ;    //0000 xxxx  0¹Ø±Õ 1Ê¹ÄÜ

    //Íâ²¿ÖÐ¶ÏÓÅÏÈ¼¶ÉèÖ
    IE1 |= 0x08;	//0000 x000  INT2Ê¹ÄÜ
    IP1 |= 0X00;
    EA = 1;
}

void Zero_Crossing_EX2_Handle()
{
	
    //Èç¹ûÖÐ¶Ï2ÓÐÁ½Â·ÊäÈë£¬¸ù¾ÝÉÏÉýÑØ»òÕßÏÂ½µÑØÀ´È·ÈÏ£¬ÉÏÉýÑØÖÐ¶Ï£¬ËùÒÔ¶Ë¿ÚµçÆ½ÊÇ1
    //if(P20 == 1) //INT24 P20 ¹ýÁã¼ì²âµ½Áãµã
    {
        //PWM¼ÆÊýÖµÖØÖÃ
        PWM_Init();
    }
 /*   if(P21 == 1) //INT25 P21 Ë®Á÷¼ì²â¼ÆÊý
    {

    }
	*/
}


//HEAT TRA PWM1 ¹¦ÂÊµ÷½Ú·½Ê½ flag 0:²»ÓÃµ÷½Ú 1£ºÔö¼Ó¹¦ÂÊ DutyÔö´ó 2£º¼õÉÙ¹¦ÂÊ Duty¼õÉÙ
void Scr_Driver_PWM_Adjust(uint flag)
{
	if(flag==1 || flag==2)
	{
		EA=0;
		IE1 &= 0xfd;        //¹Ø±ÕPWMÖÐ¶Ï		
		
		if(flag==1){ //Ôö¼Ó¹¦ÂÊ
			PWMDTY1=PWMDTY1+3;
			if(PWMDTY1>59)
			{
				PWMDTY1=59;
			}
		}
		else if(flag==2)
		{
			PWMDTY1=PWMDTY1-3;
			if(PWMDTY1<1)
			{				
				PWMDTY1=1;
			}
		}			
		
		IE1 |= 0x02;        //¿ªÆôPWMÖÐ¶Ï
		EA=1;
	}
}


//¼ì²âÎÂ¶È±£ÏÕ HEAT ERROR Ö±½Ó¼ì²â¶Ë¿ÚÖµ P03   ÂÖÑ¯·½Ê½
int Scr_Driver_Check_Heat_Error()
{	
	if(P03==0)
	{
		//ÎÂ¶ÈÕý³£·¶Î§ÄÚ£¬ÎÂ¶È±£ÏÕ²»ÌøÕ¢
		return 0;
	}
	else if(P03==1)
	{
		//ÎÂ¶ÈÒì³£·¶Î§ÄÚ£¬ÎÂ¶È±£ÏÕÒÑÌøÕ¢
		return -1;
	}
	return -1;
}

//¼ÌµçÆ÷¿ØÖÆ HEAT RLY P02
void Scr_Driver_Control_Heat_RLY(int on)
{
	P0VO = P0VO&0xfb; //P02¶Ë¿ÚÉèÖÃ³ÉÆÕÍ¨I/O¿Ú  1111 1011
	if(on == 1)
	{
		P02=1;
	}
	else
	{
		P02=0;
	}
}
