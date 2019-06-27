//���������ͷ�ɼ�����������������Ҫ��ǿ������ÿһ֡��������ݰ���߼���CRCУ��͡�CRCУ�����16λ�ģ�Ҳ��32λ�ġ�����CRCУ����㷨ԭ�������ڰٶ���ѧϰ�ģ���ʵ�����кܶ��������ϡ��򵥵�˵����CRCУ��;��ǽ�һ�ζ��������ݽ��м��ܣ�����һ������ʽ����Ȼ��õ�һ��У���롣�����У�����������ζ��������ݺ�߾����ˡ�Ȼ����շ��ڽ��յ�����֮���ٶ����У������н��롣
//�����Ҿͽ������demo����򵥵�ע��һ�£�
//�ڷ��ͷ���buffer[22]�У�ǰ��ʮ������ΪҪ���͵����ݣ�������λ��buffer[20]��buffer[21]�е����ݾ��Ǻ���int CalCrc(int crc, const char *buf, int len)������CRCУ��͡��������£�
//int main()
//{
//char buffer[22] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01, 0x02, 0x03, 0x04};
//int crc = CalCrc(0, buffer, 20);//crcΪ16λУ����
//buffer[21] = (char)crc;//ȡУ����Ͱ�λ
//buffer[20] = (char)(crc >> 8);//ȡУ����߰�λ
//}
//�ڽ��շ��������յ���buffer[22]�е��������ݴ��뺯��int CalCrc(int crc, const char *buf, int len)�н��м��㣬������ֵΪ0����˵�����յ���������ȷ����ġ���ʵ�ڲ��������п���ʹ������ץ��������м�غ���֤��
 
//int result = CalCrc(0, buffer, 22);
//if(result == 0)
//{
//����printf("���ݴ�����ȷ");
//}
//����demo�Ĵ������£�
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
    int crc = CalCrc(0, buffer, 20);//����õ���16λCRCУ����
    buffer[21] = (char)crc;//ȡУ����ĵͰ�λ
    buffer[20] = (char)(crc >> 8);//ȡУ����ĸ߰�λ
    
    char ch1=(char)(crc >> 8);//ȡУ����ĸ߰�λ
    char ch2=(char)crc;//ȡУ����ĵͰ�λ
	//���շ��ڽ��յ�buffer�е�����ʱ������CalCrc���м��㣬��result��ֵΪ0����˵�����ݴ����������
	
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
//���ߣ��⸣�� 
//��Դ��CSDN 
//ԭ�ģ�https://blog.csdn.net/cyq129445/article/details/80168258 
//��Ȩ����������Ϊ����ԭ�����£�ת���븽�ϲ������ӣ�
