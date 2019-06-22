//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"
#include <stdio.h>

/******************
	*ÒÔÏÂËÄÏîÊÇĞèÒª¸ù¾İÊµ¼ÊÇé¿öµ÷ÊÔµÄ
	******************/
	//15.6msMS¼ÆËãÒ»´Î ¼ÆËãÖÜÆÚ
#define pidt   0.5
//±ÈÀıÏµÊı  0.01 --- 10  ²ÉÑùÆµÂÊµÍ£¨Èç500ms£©£¬KpÒ»°ãÊÇ0.01¼¶±ğ£»²ÉÑùÆµÂÊ¸ß£¨Èç1ms£©£¬KpÒ»°ãÊÇ1¼¶±ğ
//#define  Kp   (5/10)    //Ò»¶¨²»ÄÜÕâÑùÓÃ£¬keil ²»Ö§³Ö
//»ı·ÖÊ±¼ä
#define  Ti  5000
//Î¢·ÖÊ±¼ä
#define  Td 600 

//55*10-28*10=270
#define Upper_Limit  270
//Èç¹û³öË®ÎÂ¶È³¬¹ıÔ¤ÉèÎÂ¶È£¬¿É¿Ø¹èÎŞ¹¦ÂÊÔËĞĞ
#define Lower_Limit  -1


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

bit b_start_pid=false;

////ÈÈË®Æ÷ÄÚ²¿Òì³£×´Ì¬
Enum_Ex_Flag idata Ex_Flag;

//35¶È~60¶È ×Ô¶¯µ÷½Ú  ×î¼Ñ£º40 - 50
int idata best_temp_out=37;
int  current_out_temp=28; //µ±Ç°³öË®ÎÂ¶È

int idata scr_curr_time=0;//zero_period_high_time/2;//20000;//6;



//int idata Out1=0;  //¼ÇÂ¼ÉÏ´ÎÊä³ö

//int idata ERR=0;       //µ±Ç°Îó²î
//int idata ERR1=0;      //ÉÏ´ÎÎó²î
//int  idata ERR2=0;      //ÉÏÉÏ´ÎÎó²î


//void Zero_Crossing_EXTI_Test(void);
void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();

//HEAT TRA  ¹¦ÂÊµ÷½Ú·½Ê½ flag 0:²»ÓÃµ÷½Ú 1£ºÔö¼Ó¹¦ÂÊ 2£º¼õÉÙ¹¦ÂÊ
//void Scr_Driver_power_Adjust(uint flag);

//Èí¼şÑÓÊ±
void soft_delay(uint n);
//void delay_1ms(uint x);
//void delay(uint n);

// SvÉè¶¨ÎÂ¶ÈÖµ  Pvµ±Ç°ÎÂ¶ÈÖµ
void PIDCalc(int Sv,int Pv);

int Scr_Driver_Check_Insurance();//¼ì²âÎÂ¶È±£ÏÕ
void Scr_Driver_Control_Heat_RLY(int on);//¼ÌµçÆ÷¿ØÖÆ HEAT RLY P02

//HEAT TRA PWM1 ¹¦ÂÊµ÷½Ú·½Ê½ flag 0:²»ÓÃµ÷½Ú 1£ºÔö¼Ó¹¦ÂÊ DutyÔö´ó 2£º¼õÉÙ¹¦ÂÊ Duty¼õÉÙ
//void Scr_Driver_PWM_Adjust(uint flag);

//uchar Zero_Crossing_INT1_flag = 0x00;
/*****************************************************
*º¯ÊıÃû³Æ£ºvoid EXTI_Test(void)
*º¯Êı¹¦ÄÜ£ºÍâ²¿ÖĞ¶Ï²âÊÔ
*Èë¿Ú²ÎÊı£ºvoid
*³ö¿Ú²ÎÊı£ºvoid
*****************************************************/
//void Zero_Crossing_EXTI_Test(void)
//{
//    Zero_Crossing_EX_Init();
//    while(1)
//    {
//    }
//}
/*****************************************************
*º¯ÊıÃû³Æ£ºvoid Zero_Crossing_EX_Init(void)
*º¯Êı¹¦ÄÜ£ºÍâ²¿ÖĞ¶Ï³õÊ¼»¯
*Èë¿Ú²ÎÊı£ºvoid
*³ö¿Ú²ÎÊı£ºvoid
*****************************************************/
void Zero_Crossing_EX_Init(void)
{    
    P2CON &= 0XFE;     //ÖĞ¶ÏIO¿ÚÉèÖÃÎª¸ß×èÊäÈë
    //P2PH  |= 0x01;     //ÖĞ¶ÏIO¿ÚÉèÖÃÎª¸ß×è´øÉÏÀ­
		P2PH &= 0xfe; //²»´øÉÏÀ­ Íâ²¿ÓĞÉÏÀ­µç×è

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
			//È«¹¦ÂÊ
//			HEAT_TRA=1;
			
//			//¹ıÁã¼ì²âÖĞ¶Ï£¬¿É¿Ø¹è¹Ø±Õ
			HEAT_TRA=0;
			
			//¶¨Ê±Æ÷¹Ø±Õ
			//TR1 = 0;
			
			if(scr_curr_time==0)
			{
				//È«¹¦ÂÊ
				HEAT_TRA=1;
				
				//¶¨Ê±Æ÷¹Ø±Õ
				TR1 = 0;
			}			
			else if(scr_curr_time==zero_period_high_time)
			{				
				//ÎŞ¹¦ÂÊ
				if(HEAT_TRA!=0)
					HEAT_TRA=0;
		
				//¶¨Ê±Æ÷¹Ø±Õ
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
				
				//scr_open_time = scr_open_time_max/2;//37¶È

				if(scr_curr_time<=(scr_open_time_max-zero_peroid_last_time))
				{
					if(scr_open_time != scr_curr_time)
					{
						scr_open_time=scr_curr_time;
					}
		
					Timer_Init();
				}
				else
				{
					HEAT_TRA=0;
					
					//¶¨Ê±Æ÷¹Ø±Õ
					TR1 = 0;
				}				
			}
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
	soft_delay(48000); // (1+1+£¨1+2£©*48000)*0.5us=72001us=72.001ms
}


//Èí¼şÑÓÊ± https://blog.csdn.net/nanfeibuyi/article/details/83577641 
/*

 CLRÖ¸ÁîÏûºÄ1¸ö»úÆ÷ÖÜÆÚ

 MOVÖ¸ÁîÏûºÄ1¸ö»úÆ÷ÖÜÆÚ

 INCÖ¸ÁîÏûºÄ1¸ö»úÆ÷ÖÜÆÚ                                      

 CJNEÖ¸ÁîÏûºÄ2¸ö»úÆ÷ÖÜÆÚ

¸ù¾İÑ­»·Ìõ¼ş£¬INCÖ¸ÁîºÍCJNEÖ¸Áî×Ü¹²ÒªÖ´ĞĞn´Î,¹²ÏûºÄ»úÆ÷ÖÜÆÚ£¨1+2£©*n£¬

¼ÓÉÏCLRÖ¸ÁîºÍMOVÖ¸Áî£¬Ñ­»·³ÌĞò×Ü¹²ÏûºÄ»úÆ÷ÖÜÆÚ£º1+1+£¨1+2£©*n

Èç¹ûµ¥Æ¬»úµÄ¾§ÕñÆµÂÊÎª24MHz£¬Ôò»úÆ÷ÖÜÆÚ = 12*£¨1/24£©us = 0.5us¡£

ÄÇÃ´forÑ­»·³ÌĞòºÄÊ±Îª(1+1+£¨1+2£©*n)*0.5us¡£
*/
void soft_delay(uint n)
{
	uint k;
	for(k=0;k<n;k++)
	{
		_nop_();  //´ú±íÔËĞĞÒ»¸ö»úÆ÷ÖÜÆÚ Èç¹ûÕâ¸öµ¥Æ¬»úµÄ¾§ÕñÊÇ12MµÄ£¬ÄÇÃ´Õâµ÷´úÂë»áÔËĞĞ1US
//		;
	}
}


#if 0

//https://blog.csdn.net/nanfeibuyi/article/details/83577641 
//º¯ÊıÃû£ºdelay_1ms(uint x)
//¹¦ÄÜ£ºÀûÓÃ¶¨Ê±Æ÷0¾«È·¶¨Ê±1ms
//µ÷ÓÃº¯Êı£º
//ÊäÈë²ÎÊı£ºx,1ms¼ÆÊı
//Êä³ö²ÎÊı£º
//ËµÃ÷£ºÑÓÊ±µÄÊ±¼äÎª1ms³ËÒÔx
//ÑÓÊ±1s£ºdelay_1ms(1000); //1000ms = 1s 
void delay_1ms(uint x)
{
	TMOD=0X01;//¿ª¶¨Ê±Æ÷0£¬¹¤×÷·½Ê½Îª1
	TR0=1;//Æô¶¯¶¨Ê±Æ÷0£»
	while(x--)
	{
		TH0=0Xfc;//¶¨Ê±1ms³õÖµµÄ¸ß8Î»×°ÈëTH0
		TL0=0X18;//¶¨Ê±1ms³õÖµµÄµÍ8Î»×°ÈëTL0
		while(!TF0);//µÈ´ı£¬Ö±µ½TF0Îª1
		TF0=0;	   //ÖØÖÃÒç³öÎ»±êÖ¾
	}		
	TR0=0;//Í£Ö¹¶¨Ê±Æ÷0£»
}

////https://blog.csdn.net/nanfeibuyi/article/details/83577641 
//Èç¹ûµ¥Æ¬»ú¾§ÕñÆµÂÊ24MHz£¬»úÆ÷ÖÜÆÚ=12*(1/24)us=0.5us
//Ñ­»·ºÄÊ±Îª ((1+1+600+1+2)*n+1+1)*0.5
void delay(uint n)//ÑÓÊ±º¯Êı
{
	uint i, j;
	for(i=0; i<n; i++) //Ö´ĞĞn´Î (1+1+600+1+2)*n
		for(j=0; j<200; j++) //£¨1+2£©*200=600ÖÜÆÚ
			;
}

#endif


// SvÉè¶¨ÎÂ¶ÈÖµ  Pvµ±Ç°ÎÂ¶ÈÖµ
void PIDCalc(int Sv,int Pv)
{ 		
	//int pidret=0;
	
	 //ÎÂ¶ÈÖµ³Ë10×ö´¦Àí
	int target_temp=Sv*10;
	int curr_temp=Pv*10;
	
	//int Out=target_temp-curr_temp;
			
	int DERR1 = 0;       //
	int DERR2 = 0;       //

	int Pout = 0;       //±ÈÀı½á¹û
	int Iout = 0;       //»ı·Ö½á¹û
	int Dout = 0;       //Î¢·Ö½á¹û
	int Out = 0; //×ÜÊä³ö
	
	
	
	
	static int Out1=0;  //¼ÇÂ¼ÉÏ´ÎÊä³ö

	static int ERR=0;       //µ±Ç°Îó²î
	static int ERR1=0;      //ÉÏ´ÎÎó²î
	static int ERR2=0;      //ÉÏÉÏ´ÎÎó²î

	
//	static uint Upper_Limit= 100; //PIDÊä³öÉÏÏŞ
//	static uint Lower_Limit= 0; //PIDÊä³öÏÂÏŞ
	
	
	 ERR = target_temp - curr_temp;   //Ëã³öµ±Ç°Îó²î
	DERR1 = ERR - ERR1;   //ÉÏ´Î
	
	//DERR2 = ERR - 2*ERR1 + ERR2; //ÉÏÉÏ´Î  //²»ÒªÔÚÖ÷³ÌĞòºÍÖĞ¶Ï³ÌĞòÖĞÍ¬Ê±×ö8bitÒÔÉÏµÄ³Ë³ı·¨ÔËËã£¬»á³ö´í
	DERR2= ERR  + ERR2;
	DERR2= DERR2 - ERR1;
	DERR2= DERR2 - ERR1;
	
	//ÏÈKp
	Pout = DERR1/2;//DERR1*Kp;    //Êä³öP
	Iout = 0;//(float)(ERR * ((Kp * pidt) / Ti));  //Êä³öI
	Dout = 0;//(float)(DERR2 * ((Kp * Td) / pidt));   //Êä³öD
	//Out = (int)(Out1 + Pout + Iout + Dout);
	Out = Out1+ Pout;
	Out = Out+ Iout;
	Out = Out+ Dout;
	
	if(Out >= Upper_Limit) { //Èç¹ûÊä³ö´óÓÚµÈÓÚÉÏÏŞ
		Out = Upper_Limit;
	} 
	else if(Out <= Lower_Limit) { //Èç¹ûÊä³öĞ¡ÓÚµÈÓÚÏÂÏß
		Out = Lower_Limit;
	}
	
	Out1 = Out;      //¼ÇÂ¼Õâ´ÎÊä³öµÄÖµ

	ERR2 = ERR1;    //¼ÇÂ¼Îó²î
	ERR1 = ERR;     //¼ÇÂ¼Îó²î
	
	/**/
			
	if(Out>0)
	{
		printf("111");
		
		//Æ«²î´óÓÚ2¶ÈÎªÉÏÏŞ·ùÖµÊä³ö(È«ËÙ¼ÓÈÈ)
		if(best_temp_out-current_out_temp>20)//ÎÂ¶ÈÆ«²î´óÓÚ2?
		{
			if(scr_curr_time<1)
			{
				scr_curr_time=0;
			}
		}
		else
		{
			//PIDËã·¨¿ØÖÆ
			if(b_start_pid==false)
			{
				b_start_pid=true;
				
				//È«¹¦ÂÊµ÷Õû80% ¹¦ÂÊµ÷½ÚÊÇÏà·´µÄ (100-80)/100=1/5
				scr_curr_time = zero_period_low_time/5;
			}
			else
			{
				//20000/270=74
		
				//Ò»¶¨ÒªÏà¼õ£¬ÒòÎª¹¦ÂÊµ÷½ÚÊÇÏà·´µÄ£¬scr_curr_timeÔ½Ğ¡£¬¹¦ÂÊÔ½´ó
				scr_curr_time -= Out*74;
				if(scr_curr_time<1)
				{
					scr_curr_time=0;
				}
			}
		}
	}
	else if(Out<0)
	{
		//UART_SentChar(0x57);
		
		printf("222");
		
		if(HEAT_TRA!=0)
			HEAT_TRA=0;
		
		//¶¨Ê±Æ÷¹Ø±Õ
		if(TR1!=0)
			TR1 = 0;
		
		scr_curr_time=zero_period_high_time;//scr_open_time_max-zero_peroid_last_time;
	}
	else
	{
		printf("333");
	}
}
