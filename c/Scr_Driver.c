//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"

//INT24 P20 ZERO

//HEAT TRA PWM1 P01

//HEAT RLY ¼ÌµçÆ÷¿ØÖÆ P02

//T/S1 ÎÂ¶È±£ÏÕ HEAT ERROR COM3/P03
//Èç¹û¼ì²âµ½ÎÂ¶È¹ı¸ß£¬T/S1»á×Ô¶¯ÌøÕ¢£¬¶Ï¿ªµçÂ· 
//¼ÌµçÆ÷Í£Ö¹¹¤×÷ ±£»¤Õû¸ö¿É¿Ø¹èÇı¶¯µçÂ·
//HEAT ERROR ÎªÊäÈë¶Ë£¬Èç¹ûµÈÓÚ¸ßµçÆ½£¬±íÃ÷ÈÈË®Æ÷ÎÂ¶È¹ı¸ß
//Èç¹ûÎªµÍµçÆ½£¬±íÃ÷ÈÈË®Æ÷ÎÂ¶ÈÔÚÕı³£·¶Î§ÄÚ

//µ±Ç°ÈÈË®Æ÷ÔËĞĞ»òÍ£Ö¹×´Ì¬ ¿ØÖÆ¼ÌµçÆ÷¶¯×÷ 0£ºÍ£Ö¹ 1£ºÔËĞĞ
bit heater_relay_on=0;

////ÈÈË®Æ÷ÄÚ²¿Òì³£×´Ì¬
Enum_Ex_Flag Ex_Flag;

//35¶È~60¶È ×Ô¶¯µ÷½Ú  ×î¼Ñ£º40 - 50
int good_temp_out_low=28;
int good_temp_out_high=60;
int best_temp_out=37;
int current_out_temp=28; //µ±Ç°³öË®ÎÂ¶È

void Zero_Crossing_EXTI_Test(void);
void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();

//HEAT TRA  ¹¦ÂÊµ÷½Ú·½Ê½ flag 0:²»ÓÃµ÷½Ú 1£ºÔö¼Ó¹¦ÂÊ 2£º¼õÉÙ¹¦ÂÊ
void Scr_Driver_Time2_Adjust(uint flag);

//Èí¼şÑÓÊ±
void soft_delay(uint n);

int Scr_Driver_Check_Insurance();//¼ì²âÎÂ¶È±£ÏÕ
void Scr_Driver_Control_Heat_RLY(int on);//¼ÌµçÆ÷¿ØÖÆ HEAT RLY P02

//HEAT TRA PWM1 ¹¦ÂÊµ÷½Ú·½Ê½ flag 0:²»ÓÃµ÷½Ú 1£ºÔö¼Ó¹¦ÂÊ DutyÔö´ó 2£º¼õÉÙ¹¦ÂÊ Duty¼õÉÙ
void Scr_Driver_PWM_Adjust(uint flag);

uchar Zero_Crossing_INT1_flag = 0x00;
/*****************************************************
*º¯ÊıÃû³Æ£ºvoid EXTI_Test(void)
*º¯Êı¹¦ÄÜ£ºÍâ²¿ÖĞ¶Ï²âÊÔ
*Èë¿Ú²ÎÊı£ºvoid
*³ö¿Ú²ÎÊı£ºvoid
*****************************************************/
void Zero_Crossing_EXTI_Test(void)
{
    Zero_Crossing_EX_Init();
    while(1)
    {
    }
}
/*****************************************************
*º¯ÊıÃû³Æ£ºvoid Zero_Crossing_EX_Init(void)
*º¯Êı¹¦ÄÜ£ºÍâ²¿ÖĞ¶Ï³õÊ¼»¯
*Èë¿Ú²ÎÊı£ºvoid
*³ö¿Ú²ÎÊı£ºvoid
*****************************************************/
void Zero_Crossing_EX_Init(void)
{    
    P2CON &= 0XFE;     //ÖĞ¶ÏIO¿ÚÉèÖÃÎª¸ß×èÊäÈë
    P2PH  |= 0x01;     //ÖĞ¶ÏIO¿ÚÉèÖÃÎª¸ß×è´øÉÏÀ­

    //ÅäÖÃÖĞ¶Ï¿ÚINT24  ÉÏÉıÑØºÍÏÂ½µÑØ¶¼Ö§³Ö
    //ÏÂ½µÑØÉèÖÃ
    INT2F |= 0x10;  //0000 xxxx  0¹Ø±Õ 1Ê¹ÄÜ ´ò¿ªÏÂ½µÑØ
		//INT2F &= 0x2F; //¹Ø±ÕÏÂ½µÑØ
    //ÉÏÉıÑØÉèÖÃ
    INT2R |= 0X10 ;    //0000 xxxx  0¹Ø±Õ 1Ê¹ÄÜ

    //Íâ²¿ÖĞ¶ÏÓÅÏÈ¼¶ÉèÖ
    IE1 |= 0x08;	//0000 x000  INT2Ê¹ÄÜ
    IP1 |= 0X00;
    EA = 1;
}

void Zero_Crossing_EX2_Handle()
{
	
    //Èç¹ûÖĞ¶Ï2ÓĞÁ½Â·ÊäÈë£¬¸ù¾İÉÏÉıÑØ»òÕßÏÂ½µÑØÀ´È·ÈÏ£¬ÉÏÉıÑØÖĞ¶Ï£¬ËùÒÔ¶Ë¿ÚµçÆ½ÊÇ1
    //if(ZERO == 1) //INT24 P20 ZERO ¹ıÁã¼ì²âµ½Áãµã
    {
        //PWM¼ÆÊıÖµÖØÖÃ
//		Scr_Driver_PWM_Init();
			
			//È«¹¦ÂÊ
//			HEAT_TRA=1;
			
//			//¹ıÁã¼ì²âÖĞ¶Ï£¬¿É¿Ø¹è¹Ø±Õ
			HEAT_TRA=0;
			
			Timer_Init();
    }
 /*   if(HALL_LLJ == 1) //INT25 P21 Ë®Á÷¼ì²â¼ÆÊı
    {

    }
	*/
}

//¼ì²âÎÂ¶È±£ÏÕ HEAT ERROR Ö±½Ó¼ì²â¶Ë¿ÚÖµ P03   ÂÖÑ¯·½Ê½
int Scr_Driver_Check_Insurance()
{	
	if(HEAT_ERROR==0)
	{
		//ÎÂ¶ÈÕı³£·¶Î§ÄÚ£¬ÎÂ¶È±£ÏÕ²»ÌøÕ¢
		return 0;
	}
	else if(HEAT_ERROR==1)
	{
		//ÎÂ¶ÈÒì³£·¶Î§ÄÚ£¬ÎÂ¶È±£ÏÕÒÑÌøÕ¢
		if(heater_relay_on==1)
		{
				heater_relay_on=0;
				Scr_Driver_Control_Heat_RLY(heater_relay_on);
		}		
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
		HEAT_RLY=1;
	}
	else
	{
		HEAT_RLY=0;
	}
	
	//Èí¼şÑÓÊ±£¬±£Ö¤heater_relay_on±äÁ¿¸üĞÂÍê³É£¬±ÜÃâÖ÷Ñ­»·Âß¼­´íÎó»òÕß»ìÂÒ
	soft_delay(48000); //48000/24=2000=2ms
}

//HEAT TRA  ¹¦ÂÊµ÷½Ú·½Ê½ flag 0:²»ÓÃµ÷½Ú 1£ºÔö¼Ó¹¦ÂÊ 2£º¼õÉÙ¹¦ÂÊ
void Scr_Driver_Time2_Adjust(uint flag)
{
	/*
	if(flag==1 || flag==2)
	{		
		if(flag==1){ //Ôö¼Ó¹¦ÂÊ
			time2_curr=time2_curr-1;
			if(time2_curr<0)
			{
				time2_curr=0;
			}
			
			//ÉèÖÃ×î´óÖµ
			time2_count_max=open_max-time2_curr/4;
		}
		else if(flag==2) ////¼õÉÙ¹¦ÂÊ
		{
			time2_curr=time2_curr+1;
			if(time2_curr>ref_max)
			{
				time2_curr=ref_max;
			}	

			//ÉèÖÃ×î´óÖµ
			time2_count_max=open_max-time2_curr/4;
		}		

		//´®¿Ú´òÓ¡log£¬µ÷ÊÔ¡£¡£¡£
		//UART_SentChar(time2_curr);
	}
	*/
	
	if(flag==1 || flag==2)
	{		
		if(flag==1){ //Ôö¼Ó¹¦ÂÊ
			scr_curr_time -= scr_adjust_step;
			if(scr_curr_time<1)
			{
				scr_curr_time=0;
			}
		}
		else if(flag==2) //¼õÉÙ¹¦ÂÊ
		{
			scr_curr_time += scr_adjust_step;
			if(scr_curr_time>=scr_open_time_max/scr_adjust_step)
			{
				scr_curr_time=scr_open_time_max;
			}
		}		

		//´®¿Ú´òÓ¡log£¬µ÷ÊÔ¡£¡£¡£
		//UART_SentChar(scr_curr_time);
	}
	
}

//Èí¼şÑÓÊ±
void soft_delay(uint n)
{
	uint k;
	for(k=0;k<n;k++)
	{
		_nop_();
	}
}

