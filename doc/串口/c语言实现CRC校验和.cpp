//最近在摄像头采集的数据清晰度上需要加强，则在每一帧传输的数据包后边加了CRC校验和。CRC校验和有16位的，也有32位的。至于CRC校验和算法原理，我是在百度上学习的，其实网上有很多这种资料。简单的说就是CRC校验和就是将一段二进制数据进行加密（乘以一个多项式），然后得到一个校验码。将这个校验码添加在这段二进制数据后边就行了。然后接收方在接收到数据之后，再对这个校验码进行解码。
//下面我就将今天的demo代码简单的注释一下：
//在发送方的buffer[22]中，前二十个数据为要发送的数据，而后两位即buffer[20]和buffer[21]中的数据就是函数int CalCrc(int crc, const char *buf, int len)产生的CRC校验和。代码如下：
//int main()
//{
//char buffer[22] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01, 0x02, 0x03, 0x04};
//int crc = CalCrc(0, buffer, 20);//crc为16位校验码
//buffer[21] = (char)crc;//取校验码低八位
//buffer[20] = (char)(crc >> 8);//取校验码高八位
//}
//在接收方，将接收到的buffer[22]中的所有数据带入函数int CalCrc(int crc, const char *buf, int len)中进行计算，若返回值为0，则说明接收的数据是正确无误的。其实在操作工程中可以使用网络抓包软件进行监控和验证。
 
//int result = CalCrc(0, buffer, 22);
//if(result == 0)
//{
//　　printf("数据传输正确");
//}
//整个demo的代码如下：
#include <stdio.h>
int CalCrc(int crc, const char *buf, int len)
{
    unsigned int byte;
    unsigned char k;
    unsigned short ACC,TOPBIT;
//    unsigned short remainder = 0x0000;
    unsigned short remainder = crc;
    TOPBIT = 0x8000;
    for (byte = 0; byte < len; ++byte)
    {
        ACC = buf[byte];
        remainder ^= (ACC <<8);
        for (k = 8; k > 0; --k)
        {
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^0x8005;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
    remainder=remainder^0x0000;
    return remainder;
}
int main(int argc, char* argv[])
{
    char buffer[22] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01, 0x02, 0x03, 0x04};
    int crc = CalCrc(0, buffer, 20);//计算得到的16位CRC校验码
    buffer[21] = (char)crc;//取校验码的低八位
    buffer[20] = (char)(crc >> 8);//取校验码的高八位
    
    char ch1=(char)(crc >> 8);//取校验码的高八位
    char ch2=(char)crc;//取校验码的低八位
	//接收方在接收到buffer中的数据时，代入CalCrc进行计算，若result的值为0，则说明数据传输过程无误
	
	char buffer2[22] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01, 0x02, 0x03, 0x04,ch1,ch2};
    int result = CalCrc(0, buffer2, 22);
    
    if(result == 0)
	{
		printf("True");
	}
	else
	{
		printf("Error");
	}

    return 0;
}
//--------------------- 
//作者：糕福社 
//来源：CSDN 
//原文：https://blog.csdn.net/cyq129445/article/details/80168258 
//版权声明：本文为博主原创文章，转载请附上博文链接！
