/**

 * 文件：inirw.h

 * 版本：1.0

 *

 * 说明：ini配置文件读写

 * 1、支持;和#注释符号，支持行尾注释。

 * 2、支持带引号'或"成对匹配的字符串，提取时自动去引号。引号中可带其它引号或;#注释符。

 * 3、支持无section或空section(名称为空)。

 * 4、支持10、16、8进制数，0x开头为16进制数，0开头为8进制。

 * 5、支持section、key或=号前后带空格。

 * 6、支持n、r、rn或nr换行格式。

 * 7、不区分section、key大小写，但写入时以新串为准，并保持其大小写。

 * 8、新增数据时，若section存在则在该节最后一个有效数据后添加，否则在文件尾部添加。

 * 9、支持指定key所在整行删除，即删除该键值，包括注释。

 * 10、可自动跳过格式错误行，修改时仍然保留。

 * 11、修改时保留原注释：包括整行注释、行尾注释(包括前面空格)。

 * 12、修改时保留原空行。以上三点主要是尽量保留原格式。

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

	//==================加载配置文件

	const char *file = "config.ini";

	//Hook_Debug_TxData("加载配置...");

	//printf("load config file %snn", file);

	iniFileLoad(file);

	//加载IP地址和端口

	sect = "CCFL_BOARD_INFO";

	key = "board_num_5";

	iniGetString(sect, key, value, sizeof(value), "notfound!");

	printf("[%s] %s = %s\n", sect, key, value);
	
	iniFileLoad(file);
	//设置字符串：若value为NULL，则删除该key所在行，包括注释
	iniSetString("CCFL_BOARD_INFO", "board_num_5", "8000345");
	iniGetString(sect, key, value, sizeof(value), "notfound!");
	printf("[%s] %s = %s\n", sect, key, value);
	//设置整数值：base取值10、16、8，分别表示10、16、8进制，缺省为10进制

	//iniSetInt(const char *section, const char *key, int value, int base);
	
	
#if 0
	key = "PORT";

	PORT = iniGetInt(sect, key, 5035);

	printf("[%11s] %11s = %dn", sect, key, PORT);

	//加载终端编号

	sect = "OthCfg";

	key  = "PosCode";

	iniGetString(sect, key, value, sizeof(value), "notfound!");

	printf("[%11s] %11s = %sn", sect, key, value);

	CurCalc_ASCII_Proc( 2, (U08*)value, strlen(value),gSamAppInfo.PsamTID, 0 );

	//加载密钥状态

	key  = "ComKeyStat";

	OthCfg.ComKeyStat = iniGetInt(sect, key, 0);

	printf("[%11s] %11s = %dn", sect, key, OthCfg.ComKeyStat);

	//加载通信密钥

	key  = "ComKey";

	iniGetString(sect, key, value, sizeof(value), "notfound!");

	printf("[%11s] %11s = %sn", sect, key, value);

	CurCalc_ASCII_Proc( 2, (U08*)value, strlen(value),OthCfg.ComKey, 0 );

	//加载文件信息

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