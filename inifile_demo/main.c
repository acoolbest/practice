/**

 * �ļ���inirw.h

 * �汾��1.0

 *

 * ˵����ini�����ļ���д

 * 1��֧��;��#ע�ͷ��ţ�֧����βע�͡�

 * 2��֧�ִ�����'��"�ɶ�ƥ����ַ�������ȡʱ�Զ�ȥ���š������пɴ��������Ż�;#ע�ͷ���

 * 3��֧����section���section(����Ϊ��)��

 * 4��֧��10��16��8��������0x��ͷΪ16��������0��ͷΪ8���ơ�

 * 5��֧��section��key��=��ǰ����ո�

 * 6��֧��n��r��rn��nr���и�ʽ��

 * 7��������section��key��Сд����д��ʱ���´�Ϊ׼�����������Сд��

 * 8����������ʱ����section�������ڸý����һ����Ч���ݺ���ӣ��������ļ�β����ӡ�

 * 9��֧��ָ��key��������ɾ������ɾ���ü�ֵ������ע�͡�

 * 10�����Զ�������ʽ�����У��޸�ʱ��Ȼ������

 * 11���޸�ʱ����ԭע�ͣ���������ע�͡���βע��(����ǰ��ո�)��

 * 12���޸�ʱ����ԭ���С�����������Ҫ�Ǿ�������ԭ��ʽ��

 */
#include <stdio.h>
#include "inirw.h"
int main()
{

	char *sect;

	char *key;

	char value[256];

	char stadate[10];

	char enddate[10];

	//==================���������ļ�

	const char *file = "config.ini";

	//Hook_Debug_TxData("��������...");

	//printf("load config file %snn", file);

	iniFileLoad(file);

	//����IP��ַ�Ͷ˿�

	sect = "CCFL_BOARD_INFO";

	key = "board_num_5";

	iniGetString(sect, key, value, sizeof(value), "notfound!");

	printf("[%s] %s = %s\n", sect, key, value);
	
	iniFileLoad(file);
	//�����ַ�������valueΪNULL����ɾ����key�����У�����ע��
	iniSetString("CCFL_BOARD_INFO", "board_num_5", "8000345");
	iniGetString(sect, key, value, sizeof(value), "notfound!");
	printf("[%s] %s = %s\n", sect, key, value);
	//��������ֵ��baseȡֵ10��16��8���ֱ��ʾ10��16��8���ƣ�ȱʡΪ10����

	//iniSetInt(const char *section, const char *key, int value, int base);
	
	
#if 0
	key = "PORT";

	PORT = iniGetInt(sect, key, 5035);

	printf("[%11s] %11s = %dn", sect, key, PORT);

	//�����ն˱��

	sect = "OthCfg";

	key  = "PosCode";

	iniGetString(sect, key, value, sizeof(value), "notfound!");

	printf("[%11s] %11s = %sn", sect, key, value);

	CurCalc_ASCII_Proc( 2, (U08*)value, strlen(value),gSamAppInfo.PsamTID, 0 );

	//������Կ״̬

	key  = "ComKeyStat";

	OthCfg.ComKeyStat = iniGetInt(sect, key, 0);

	printf("[%11s] %11s = %dn", sect, key, OthCfg.ComKeyStat);

	//����ͨ����Կ

	key  = "ComKey";

	iniGetString(sect, key, value, sizeof(value), "notfound!");

	printf("[%11s] %11s = %sn", sect, key, value);

	CurCalc_ASCII_Proc( 2, (U08*)value, strlen(value),OthCfg.ComKey, 0 );

	//�����ļ���Ϣ

	sect = "RecCfg";

	key  = "RecName";

	iniGetString(sect, key, value, sizeof(value), "notfound!");

	strcpy(Rec_Name,value);

	printf("[%11s] %11s = %sn", sect, key, Rec_Name);

	key  = "StaDate";

	iniGetString(sect, key, value, sizeof(value), "notfound!");

	printf("[%11s] %11s = %sn", sect, key, value);

	strcpy(stadate,value);

	key  = "EndDate";

	iniGetString(sect, key, value, sizeof(value), "notfound!");

	printf("[%11s] %11s = %sn", sect, key, value);

	strcpy(enddate,value);


	Get_RecNum(Rec_Name,stadate,enddate);

#endif
	iniFileFree();
	return 0;
}