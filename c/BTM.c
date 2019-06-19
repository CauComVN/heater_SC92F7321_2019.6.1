#include "H/Function_Init.H"
#include <math.h>

uchar idata b_btm_int_flag=false;
//uint count=0;

void BTM_Init(void);
//void BTM_Test(void);



/*****************************************************
*函数名称：void BTM_Test(void)
*函数功能：BTM测试
*入口参数：void
*出口参数：void
*****************************************************/
//void BTM_Test(void)
//{
//	BTM_Init();
//	while(1)
//	{
//	}
//}
/*****************************************************
*函数名称：void BTM_Init(void)
*函数功能：BTM初始化
*入口参数：void
*出口参数：void
*****************************************************/
void BTM_Init(void)
{
//	BTMCON = 0x00;  //不开启
	BTMCON = 0x80;  //每15.6ms产生一个中断
//	BTMCON = 0x81;	//每31.3ms产生一个中断
//	BTMCON = 0x82;	//每62.5ms产生一个中断
//	BTMCON = 0x83;	//每125ms产生一个中断
//	BTMCON = 0x84;	//每0.25s产生一个中断
//	BTMCON = 0x85;	//每0.5s产生一个中断
//	BTMCON = 0x86;	//每1s产生一个中断
	BTMCON = 0x87;	//每2s产生一个中断
	EA = 1;           //开启总中断
	IE1 |= 0x04;       //开启BTM中断
}

/*****************************************************
*函数名称：void BTM_Int(void) interrupt 9
*函数功能：中断函数
*入口参数：void
*出口参数：void
*****************************************************/
void BTM_Int(void) interrupt 9
{
	if(!(BTMCON&0X40))		//中断标志位判断
	{
//		uint ADCTempValue=0;
//		int ret=0;
//		uint pidret=0;
		
		b_btm_int_flag=true;
		
		
//		count++;
//		if(count>=20)
//		b_btm_int_flag=true;
		
//		//出水温度
//			ADC_Init(AIN8);
//			ADCTempValue=ADC_Convert(); //启动ADC转换，获得转换值
//			ret = get_temp_table(ADCTempValue,&current_out_temp);
//			
//			//UART_SentChar(current_out_temp);
//		
//			if(ret==-1) { //通知检测温度异常，超过最低温度，发送主板BEEP报警
//					ex_flag=Ex_Out_Water_Temp_Low;
//					
//			}
//			else if(ret==-2) { //通知检测温度异常，超过最高温度发送主板BEEP报警
//					ex_flag=Ex_Out_Water_Temp_High;
//			}
//			else {
//				//调节温度到一个合适的范围内
//				//HEAT TRA  功率调节方式 flag 0:不用调节 1：增加功率 2：减少功率
//				
//				//0 --- 20000  42度-28度=14度 20000/14=1428.57
//								
//				//设定值大于实际值否？
//				//偏差大于2为上限幅值输出(全速加热)
//				if(best_temp_out-current_out_temp>2) ////偏差大于2?
//				{
//					scr_open_time=0;//8600;//17200;//20000;//5;//低电平 8.6ms 17200---0  高电平 10ms  20000---0
//					scr_curr_time=0;
//				}
//				else{
//					pidret=PIDCalc(best_temp_out,current_out_temp);
//					
//					pidret=1400*pidret;
//					
//					if(current_out_temp<best_temp_out)
//					{
//						//Scr_Driver_power_Adjust(1);
//						
//						scr_curr_time -= abs(pidret);
//						if(scr_curr_time<1)
//						{
//							scr_curr_time=0;
//						}
//					}
//					if(current_out_temp>best_temp_out)
//					{
//						//Scr_Driver_power_Adjust(2);
//						
//						scr_curr_time += abs(pidret);
//						if(scr_curr_time>=(scr_open_time_max-zero_peroid_last_time))
//						{
//							scr_curr_time=(scr_open_time_max-zero_peroid_last_time);
//						}
//					}
//				}				
//			}
	}
}

