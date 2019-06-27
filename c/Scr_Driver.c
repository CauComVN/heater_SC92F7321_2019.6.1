//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"
#include <stdio.h>

//55*10-28*10=270
#define Upper_Limit  30
//Èç¹û³öË®ÎÂ¶È³¬¹ıÔ¤ÉèÎÂ¶È£¬¿É¿Ø¹èÎŞ¹¦ÂÊÔËĞĞ
#define Lower_Limit  -30


//INT24 P20 ZERO

//HEAT TRA PWM1 P01

//HEAT RLY ¼ÌµçÆ÷¿ØÖÆ P02

//T/S1 ÎÂ¶È±£ÏÕ HEAT ERROR COM3/P03
//Èç¹û¼ì²âµ½ÎÂ¶È¹ı¸ß£¬T/S1»á×Ô¶¯ÌøÕ¢£¬¶Ï¿ªµçÂ·
//¼ÌµçÆ÷Í£Ö¹¹¤×÷ ±£»¤Õû¸ö¿É¿Ø¹èÇı¶¯µçÂ·
//HEAT ERROR ÎªÊäÈë¶Ë£¬Èç¹ûµÈÓÚ¸ßµçÆ½£¬±íÃ÷ÈÈË®Æ÷ÎÂ¶È¹ı¸ß
//Èç¹ûÎªµÍµçÆ½£¬±íÃ÷ÈÈË®Æ÷ÎÂ¶ÈÔÚÕı³£·¶Î§ÄÚ

// 0:ÎŞ¹¦ÂÊ 1£ºÈ«¹¦ÂÊ
volatile uchar heater_power_status=0; 
//µ±Ç°ÈÈË®Æ÷ÔËĞĞ»òÍ£Ö¹×´Ì¬ ¿ØÖÆ¼ÌµçÆ÷¶¯×÷ 0£ºÍ£Ö¹ 1£ºÔËĞĞ
volatile bit heater_relay_on=0;
//¿ªÊ¼PIDËã·¨±ê¼Ç
volatile bit b_start_pid=0;
////ÈÈË®Æ÷ÄÚ²¿Òì³£×´Ì¬
Enum_Ex_Flag idata ex_flag=Ex_Normal;
//35¶È~60¶È ×Ô¶¯µ÷½Ú  ×î¼Ñ£º40 - 50
int idata best_temp_out=38;
//µ±Ç°³öË®ÎÂ¶È
volatile uchar  current_out_temp=29; 
//¿É¿Ø¹è´¥·¢Ê±¼ä×î´óÖµ 
uint idata scr_open_time_max=zero_period_low_time;
//ÊµÊ±¼ÆËãµÄ¿É¿Ø¹è´¥·¢Ê±¼ä
volatile int  scr_curr_time=0;
//ÊµÊ±¼ÆËãµÄ¿É¿Ø¹è´¥·¢Ê±¼ä¸±±¾£¬ÓÃÓÚ½â¾öÖ÷Ñ­»·ºÍ¹ıÁã¼ì²âÖĞ¶ÏÄÚÈ«¾Ö±äÁ¿scr_curr_timeÎÊÌâ
volatile int  scr_tune_time=0;

void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();

//Èí¼şÑÓÊ±
void soft_delay(uint n);

// SvÉè¶¨ÎÂ¶ÈÖµ  Pvµ±Ç°ÎÂ¶ÈÖµ
void PIDCalc(int Sv,int Pv);

int Scr_Driver_Check_Insurance();//¼ì²âÎÂ¶È±£ÏÕ
void Scr_Driver_Control_Heat_RLY(int on);//¼ÌµçÆ÷¿ØÖÆ HEAT RLY P02

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
//    EA = 1;
}

void Zero_Crossing_EX2_Handle()
{
    if(heater_power_status==1)
    {
        //È«¹¦ÂÊ
        if(HEAT_TRA!=1)
            HEAT_TRA=1;

        //¶¨Ê±Æ÷¹Ø±Õ
        if(TR1!=0)
            TR1 = 0;
    }
    else if(heater_power_status==0)
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

            //¶¨Ê±Æ÷¹Ø±Õ
            if(TR1!=0)
                TR1 = 0;
        }
    }
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
¸ù¾İÑ­»·Ìõ¼ş£¬INCÖ¸ÁîºÍCJNEÖ¸Áî×Ü¹²ÒªÖ´ĞĞn´Î,¹²ÏûºÄ»úÆ÷ÖÜÆÚ£¨1+2£©*n£¬
¼ÓÉÏCLRÖ¸ÁîºÍMOVÖ¸Áî£¬Ñ­»·³ÌĞò×Ü¹²ÏûºÄ»úÆ÷ÖÜÆÚ£º1+1+£¨1+2£©*n
Èç¹ûµ¥Æ¬»úµÄ¾§ÕñÆµÂÊÎª24MHz£¬Ôò»úÆ÷ÖÜÆÚ = 12*£¨1/24£©us = 0.5us¡£
ÄÇÃ´forÑ­»·³ÌĞòºÄÊ±Îª(1+1+£¨1+2£©*n)*0.5us¡£
*/
void soft_delay(uint n)
{
    uint k;
    for(k=0; k<n; k++)
    {
        _nop_();  //´ú±íÔËĞĞÒ»¸ö»úÆ÷ÖÜÆÚ Èç¹ûÕâ¸öµ¥Æ¬»úµÄ¾§ÕñÊÇ12MµÄ£¬ÄÇÃ´Õâµ÷´úÂë»áÔËĞĞ1US
//		;
    }
}

// SvÉè¶¨ÎÂ¶ÈÖµ  Pvµ±Ç°ÎÂ¶ÈÖµ
void PIDCalc(int Sv,int Pv)
{
    int DERR1 = 0;
    int DERR2 = 0;

    int Pout = 0;       //±ÈÀı½á¹û
    int Iout = 0;       //»ı·Ö½á¹û
    int Dout = 0;       //Î¢·Ö½á¹û
    int Out = 0; 				//×ÜÊä³ö

    static int Out1=0;  //¼ÇÂ¼ÉÏ´ÎÊä³ö
    static int ERR=0;       //µ±Ç°Îó²î
    static int ERR1=0;      //ÉÏ´ÎÎó²î
    static int ERR2=0;      //ÉÏÉÏ´ÎÎó²î

    ERR=Sv-Pv; //Ëã³öµ±Ç°Îó²î
    DERR1 = ERR - ERR1;   //ÉÏ´Î

    //DERR2 = ERR - 2*ERR1 + ERR2; //ÉÏÉÏ´Î  //²»ÒªÔÚÖ÷³ÌĞòºÍÖĞ¶Ï³ÌĞòÖĞÍ¬Ê±×ö8bitÒÔÉÏµÄ³Ë³ı·¨ÔËËã£¬»á³ö´í
    DERR2= ERR  + ERR2;
    DERR2= DERR2 - ERR1;
    DERR2= DERR2 - ERR1;

    //ÏÈKp
    Pout = DERR1*Kp;//0;//DERR1*Kp;    //Êä³öP
    Iout = ERR * Ti;//0;
    Dout = DERR2*Td;//DERR2 * Td;//0;
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

    printf("%d\n",Out);

    //if(Out>=0)
    {
        //printf("111\n");

        if(ERR>2)
        {
            if(heater_power_status!=1)
                heater_power_status=1;
        }
        else
        {
            if(heater_power_status!=2)
                heater_power_status=2;

            //PIDËã·¨¿ØÖÆ
            if(b_start_pid==0)
            {
                b_start_pid=1;

                //È«¹¦ÂÊµ÷Õû90% ¹¦ÂÊµ÷½ÚÊÇÏà·´µÄ (100-90)/100=1/10
                scr_curr_time = 5000;

                do {
                    scr_tune_time=scr_curr_time;
                } while(scr_tune_time != scr_curr_time);
            }
            else
            {
                //Ò»¶¨ÒªÏà¼õ£¬ÒòÎª¹¦ÂÊµ÷½ÚÊÇÏà·´µÄ£¬scr_curr_timeÔ½Ğ¡£¬¹¦ÂÊÔ½´ó
                scr_curr_time = scr_curr_time - Out;  //Out=50 Out*74=3700
                if(scr_curr_time<1)
                {
                    if(heater_power_status!=1)
                        heater_power_status=1;
                }
                else
                {
                    //scr_curr_time¸´ÖÆ¸ø¸±±¾scr_tune_time£¬±Ü¿ªÖ÷Ñ­»·ºÍ¹ıÁãÖĞ¶Ï¹²ÏíÈ«¾Ö±äÁ¿µ¼ÖÂµÄÑÏÖØÎÊÌâ£¬ÕâÊÇÓÃ²Ù×÷ÏµÍ³µÄ·½·¨
                    //¹Ø±Õ¹ıÁãÖĞ¶Ï»áÍ¬Ê±¹Ø±ÕË®Á÷Á¿¼ÆÖĞ¶Ï£¬ÒòÎª¶¼ÊÇINT2ÖĞ¶Ï£¬µ¼ÖÂ¸ü´óµÄÎÊÌâ£¬´ËÖÖ·½·¨ÔÚÕâÀï²»¿ÉĞĞ
                    //https://blog.csdn.net/dijindeng/article/details/50426028
                    do {
                        scr_tune_time=scr_curr_time;
                    } while(scr_tune_time != scr_curr_time);
                }
            }
        }
        //printf("%d\n",scr_curr_time);
    }
//	else if(Out<0)
//	{
//		//printf("222\n");
//
//		if(heater_power_status!=0)
//			heater_power_status=0;
//	}
}
