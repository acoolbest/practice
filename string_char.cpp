/* ************************************************************************
 *       Filename:  string_char.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2016年11月18日 18时47分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <strings.h>
#include <stdint.h>
using namespace std;
// 文件头     
unsigned char header[66] = {     
	// BITMAPFILEINFO     
	'B', 'M',               // [0-1] bfType:必须是BM字符     
	0, 0, 0, 0,             // [2-5] bfSize:总文件大小      
	0, 0, 0, 0,             // [6-9] brReserved1,bfReserved2:保留     
	sizeof(header), 0, 0, 0,// [10-13] bfOffBits:到图像数据的偏移     
	// BITMAPFILEHEADER     
	0x28, 0, 0, 0,          // [14-17] biSize:BITMAPINFOHEADER大小40字节     
	0, 0, 0, 0,             // [18-21] biWidth:图片宽度     
	0, 0, 0, 0,             // [22-25] biHeight:图片高度     
	0x01, 0,                // [26-27] biPlanes:必须为1     
	0x10, 0,                // [28-29] biBitCount:16位     
	0x03, 0, 0, 0,          // [30-33] biCompression:BI_BITFIELDS=3     
	0, 0, 0, 0,             // [34-37] biSizeImage:图片大小     
	0x12, 0x0B, 0, 0,       // [38-41] biXPelsPerMeter:单位长度内的像素数     
	0x12, 0x0B, 0, 0,       // [42-45] biYPelsPerMeter:单位长度内的像素数     
	0, 0, 0, 0,             // [46-49] biClrUsed:可用像素数，设为0即可     
	0, 0, 0, 0,             // [50-53] biClrImportant:重要颜色数，设为0即可     
	// RGBQUAD MASK     
	0x0, 0xF8, 0, 0,        // [54-57] 红色掩码     
	0xE0, 0x07, 0, 0,       // [58-61] 绿色掩码     
	0x1F, 0, 0, 0           // [62-65] 蓝色掩码     
};

uint32_t is_bmp_correct(string bmp_content)
{
	if(bmp_content.c_str()[0] == 'B' 
		&& bmp_content.c_str()[1] == 'M')
	{
		uint32_t bit_size = *((uint32_t *)&bmp_content.c_str()[34]);
		uint32_t bit_offset = *((uint32_t *)&bmp_content.c_str()[10]);
		uint32_t file_size = *((uint32_t *)&bmp_content.c_str()[2]);
		if(file_size == bmp_content.length()
			&& bit_size+bit_offset == file_size)
		{
			return 1;
		}
	}
	return 0;
}

static uint32_t read_area_file(const char * file_name, string & str_content)
{
	if (file_name == NULL || strlen(file_name) == 0)
		return 0;

	FILE *fp = NULL;
	long file_len = 0;
		
	fp = fopen(file_name, "rb");
	if (fp == NULL)
		return 0;

	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	str_content.resize(file_len);
	fread((void*)str_content.c_str(), 1, file_len, fp);
	fclose(fp);
	
	return str_content.length() ? 1 : 0;
}

int main(int argc, char *argv[])
{
	string qr_code_content;
	//12347044_err.bmp
	//12449957.bmp
	if(read_area_file("./12347044_err.bmp",qr_code_content) && is_bmp_correct(qr_code_content))
	{
		cout << "success" << endl;
	}
	else
	{
		cout << "error" << endl;
	}
	#if 0
	char a[30] = "hello world\0string_char.cpp";
	qr_code_content.resize(30);
	memcpy((char *)(qr_code_content.c_str()), a, 30);
	
	char *p_data = NULL;
	p_data = (char *)malloc(qr_code_content.length());
	qr_code_content.copy((char *)p_data, qr_code_content.length(), 0);
	
	for(int i=0;i<qr_code_content.length();i++)
		cout << i << " "<< p_data[i] << endl;
	#endif
	#if 0
	string s1 = "123\r\02343405_d*d%$#@!##*&(*$KJLKFJDLKFJ";
	char *data;
	int len = s1.length();
	data = (char *)malloc(len);
//	s1.copy(data, len, 0);
	memcpy(data, s1.c_str(), len);
	for(int i=0;i<len;i++)
	{
		cout << "~~~~~~~~~"<< *(data + i) <<"~~~~~~~~" << endl;
		printf("~~~~~ %c ~~~~~\n", *(data+i));
	}
	return 0;
	#endif
}

