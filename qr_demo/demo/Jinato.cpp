#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <qrencode.h>
#include <iostream>
#include <stdlib.h>

#pragma pack(push, 2)
//#pragma pack(2)
struct BIT_MAP_FILE_HEADER  
{   
	uint16_t bfType;       					// [0-1] bfType:必须是BM字符
	uint32_t bfSize;       					// [2-5] bfSize:总文件大小
	uint16_t bfReserved1;  					
	uint16_t bfReserved2;  					// [6-9] brReserved1,bfReserved2:保留
	uint32_t bfOffBits;    					// [10-13] bfOffBits:到图像数据的偏移
};

struct BIT_MAP_INFO_HEADER  
{   
	uint32_t biSize;   						// [14-17] biSize:本结构所占用字节数，大小为40字节
	uint32_t biWidth;                       // [18-21] biWidth:位图宽度，单位：字节
	int32_t biHeight;                       // [22-25] biHeight:位图高度，单位：字节，为负数时可上下翻转图片
	uint16_t biPlanes;                      // [26-27] biPlanes:目标设备级别，必须为1
	
	// [28-29] biBitCount:表示颜色时每个像素要用到的位数，常用的值为1(黑白二色图), 4(16色图), 8(256色), 16(嵌入式用RGB565), 24(真彩色图)
	uint16_t biBitCount;
	
	// [30-33] biCompression:位图是否压缩，其类型是 0(BI_RGB不压缩), 1(BI_RLE8压缩类型), 2(BI_RLE4压缩类型), BI_BITFIELDS=3
	uint32_t biCompression;
	
	uint32_t biSizeImage;   				// [34-37] biSizeImage:实际的位图数据占用的字节数
	uint32_t biXPelsPerMeter;   			// [38-41] biXPelsPerMeter:位图水平分辨率，每米像素数
	uint32_t biYPelsPerMeter;               // [42-45] biYPelsPerMeter:位图垂直分辨率，每米像素数
	
	// [46-49] biClrUsed:可用像素数，指定本图象实际用到的颜色数，如果该值为零，则用到的颜色数为2的biBitCount次幂个
	uint32_t biClrUsed;
	
	// [50-53] biClrImportant:指定本图象中重要的颜色数，如果该值为零，则认为所有的颜色都是重要的
	uint32_t biClrImportant;
};

struct RGB_QUAD_MASK {
	uint32_t red_mask;						// [54-57] 红色掩码
	uint32_t green_mask;					// [58-61] 绿色掩码
	uint32_t blue_mask;						// [62-65] 蓝色掩码
	uint32_t alpha;							// [66-69] 缺省为0
};
#pragma pack(pop)

enum enum_bmp_bit_count
{
	bmp_16bit_rgb565 = 16,
	bmp_24bit = 24
};

struct BGR_data
{
	uint8_t B_data;
	uint8_t G_data;
	uint8_t R_data;
};

//位图文件头
BIT_MAP_FILE_HEADER bit_map_file_header = {
	.bfType = 0x4D42,									
	.bfSize = 0,										
	.bfReserved1 = 0,
	.bfReserved2 = 0,									
	.bfOffBits = 0										
};  
							
BIT_MAP_INFO_HEADER bit_map_info_header = {
	.biSize = 40,
	.biWidth = 0,
	.biHeight = 0,
	.biPlanes = 1,
	.biBitCount = (uint16_t)bmp_24bit,
	.biCompression = 0x00,
	.biSizeImage = 0,
	#if 1
	.biXPelsPerMeter = 0,							
	.biYPelsPerMeter = 0,							
	#else
	.biXPelsPerMeter = 0x0b12,
	.biYPelsPerMeter = 0x0b12,
	#endif
	.biClrUsed = 0,
	.biClrImportant = 0
};
RGB_QUAD_MASK rgb_quad_mask = {
	.red_mask = 0xF800,
	.green_mask = 0x07E0,
	.blue_mask = 0x001F
};

bool bmp565_write(unsigned char *image, uint32_t width, uint32_t height, const char *filename)      
{
	unsigned int lineBytes16bit = (width * 2 + 3) / 4 * 4;; // 每行字节数必须为4的倍数

	BIT_MAP_FILE_HEADER bmp565_file_header = {
		.bfType = 0x4D42,									
		.bfSize = 0,										
		.bfReserved1 = 0,
		.bfReserved2 = 0,									
		.bfOffBits = sizeof(BIT_MAP_FILE_HEADER)
					+ sizeof(BIT_MAP_INFO_HEADER)
					+ sizeof(RGB_QUAD_MASK)
	};
	BIT_MAP_INFO_HEADER bmp565_info_header = {
		.biSize = 40,
		.biWidth = width,
		.biHeight = height,
		.biPlanes = 1,
		.biBitCount = 16,
		.biCompression = 0x03,
		.biSizeImage = lineBytes16bit * height,
		.biXPelsPerMeter = 0,							
		.biYPelsPerMeter = 0,							
		.biClrUsed = 0,
		.biClrImportant = 0
	};
	RGB_QUAD_MASK bmp565_rgb_quad_mask = {
		.red_mask = 0xF800,
		.green_mask = 0x07E0,
		.blue_mask = 0x001F
	};
	bmp565_file_header.bfSize = bmp565_file_header.bfOffBits + bmp565_info_header.biSizeImage;
	
	FILE * fp = NULL;
	if (!(fp = fopen(filename, "wb")))
		return false;

	fwrite(&bmp565_file_header, sizeof(BIT_MAP_FILE_HEADER), 1, fp);
	//写位图信息头进文件
	fwrite(&bmp565_info_header, sizeof(BIT_MAP_INFO_HEADER), 1, fp);

	if(bmp565_info_header.biBitCount == bmp_16bit_rgb565)
	{
		//写调色板信息进文件
		fwrite(&bmp565_rgb_quad_mask, sizeof(RGB_QUAD_MASK), 1, fp);
	}
	
	//写数据进文件  
	fwrite(image, sizeof(unsigned char), bmp565_info_header.biSizeImage, fp);

	fclose(fp);
	
	return true;
}

bool bmp_write(QRcode * qrCode, enum_bmp_bit_count biBitCount, const char *filename, const char *logo_filename = NULL, uint16_t pixel = 128)
{
	bool ret = false;
	FILE * pf = NULL;
	FILE * pf_logo = NULL;
	uint16_t magnification = 0;
	uint16_t margin = 0;
	
	
	int pixel_line_byte = (pixel * biBitCount / 8 + 3) / 4 * 4; //每line字节数必须为4的倍数
	int width = 0;
	int height = 0;
	int magnification_height = 0;
	int magnification_width = 0;
	
	unsigned char * p_bmp_data = NULL;
	unsigned char * qr_data = NULL;
	
	BIT_MAP_FILE_HEADER logo_head1;
	BIT_MAP_INFO_HEADER logo_head2;
	unsigned char * p_logo_bmp_data = NULL;
	unsigned char * p_logo_bmp_source_data = NULL;
	BGR_data * p_BGR_data = NULL;
	uint16_t tmp_16bit = 0;
	uint32_t logo_bmp_data_index = 0;
	int logo_line_byte = 0;
	uint16_t logo_margin_left = 0;
	uint16_t logo_margin_top = 0;
	
	magnification = pixel/qrCode->width;
	if(magnification == 0)
	{
		printf("ERROR: magnification is %d.\n", magnification);
		goto _BMP_WRITE_END_;
	}

	margin = (pixel - qrCode->width * magnification)/2;
	width = qrCode->width * magnification;
	height = qrCode->width * magnification;
	magnification_height = magnification;
	magnification_width = magnification;
	
	if(filename)
	{
		pf = fopen(filename, "wb");
		if (pf == NULL)
		{  
			printf("file [%s] open fail.\n", filename);
			goto _BMP_WRITE_END_;
		}
	}
	else
	{
		goto _BMP_WRITE_END_;
	}

	//-------------------------------------------------------------------
	//位图文件头
	// [10-13] bfOffBits:到图像数据的偏移
	bit_map_file_header.bfOffBits = sizeof(BIT_MAP_FILE_HEADER) 
								+ sizeof(BIT_MAP_INFO_HEADER)
								+ (biBitCount == bmp_16bit_rgb565 ? sizeof(RGB_QUAD_MASK) : 0x00);

	// [18-21] biWidth:位图宽度，单位：字节
	bit_map_info_header.biWidth = pixel;
	
	// [22-25] biHeight:位图高度，单位：字节	
	bit_map_info_header.biHeight = (pixel);

	// [28-29] biBitCount:表示颜色时每个像素要用到的位数，常用的值为1(黑白二色图), 4(16色图), 8(256色), 16(嵌入式用RGB565), 24(真彩色图)
	bit_map_info_header.biBitCount = (uint16_t)biBitCount;

	// [30-33] biCompression:位图是否压缩，其类型是 0(BI_RGB不压缩), 1(BI_RLE8压缩类型), 2(BI_RLE4压缩类型), BI_BITFIELDS=3
	bit_map_info_header.biCompression = (biBitCount == bmp_16bit_rgb565 ? 0x03 : 0x00);

	// [34-37] biSizeImage:实际的位图数据占用的字节数
	bit_map_info_header.biSizeImage = pixel_line_byte * pixel;

	// [2-5] bfSize:总文件大小
	bit_map_file_header.bfSize = bit_map_file_header.bfOffBits + bit_map_info_header.biSizeImage;
	//-------------------------------------------------------------------
	
	p_bmp_data = new unsigned char[bit_map_info_header.biSizeImage];
	memset(p_bmp_data, 255, bit_map_info_header.biSizeImage);

	qr_data = qrCode->data;
	
	for (int i = 0; i < qrCode->width; i++)  
	{  
		for (int j = 0; j < qrCode->width; j++)
		{  
			if (*(qr_data) & 1)
			{  
				#if 0
				for(int k = 0; k < ((uint16_t)biBitCount / 8); k ++)
				{
					*(p_bmp_data + line_byte * i + ((uint16_t)biBitCount / 8) * j + k) = 0;
				}
				#endif
				for(int m = 0; m < magnification_height; m++)
				{
					for(int n = 0; n < magnification_width; n++)
					{
						for(int k = 0; k < ((uint16_t)biBitCount / 8); k ++)
						{
							/*
												h 0 w 0						h 0 w 1						h 1 w 0						h 1 w 1
							tmp_height = 0			1		2			0		1		2			3		4		5			3		4		5
							tmp_width = 0 1	2	0 1 2	0 1 2		3 4 5	3 4 5	3 4 5		 0 1 2	0 1 2	0 1 2		3 4 5	3 4 5	3 4 5
							
							int tmp_height = (i * magnification + m);
							int tmp_width = (j * magnification + n);
							
							*/
							//设置rgb颜色，可自定义设置，这里设为黑色。
							*(p_bmp_data + pixel_line_byte * ((qrCode->width-1-i) * magnification + m + margin)  + (biBitCount / 8) * (j * magnification + n + margin) + k) = 0;
						}
					}
				}
			}
			qr_data++;
		}
	}
	if(logo_filename)
	{
		pf_logo = fopen(logo_filename, "rb");
		if (pf_logo == NULL)
		{  
			printf("file [%s] open fail.\n", logo_filename);
			goto _BMP_WRITE_END_;
		}
		fread(&logo_head1, sizeof(BIT_MAP_FILE_HEADER), 1, pf_logo);
		fread(&logo_head2, sizeof(BIT_MAP_INFO_HEADER), 1, pf_logo);
		p_logo_bmp_source_data = new unsigned char[logo_head2.biSizeImage];
		
		fseek(pf_logo, logo_head1.bfOffBits, 0);//fseek(fp, offset, fromwhere);
		fread(p_logo_bmp_source_data, sizeof(unsigned char), logo_head2.biSizeImage, pf_logo);
		
		logo_line_byte = (logo_head2.biWidth * biBitCount / 8 + 3) / 4 * 4;
		logo_margin_left = (pixel - logo_head2.biWidth)/2;
		logo_margin_top = (pixel - abs(logo_head2.biHeight))/2;
		
		if(logo_head2.biBitCount == bmp_24bit && biBitCount == bmp_16bit_rgb565)
		{
			p_logo_bmp_data = new unsigned char[logo_line_byte * abs(logo_head2.biHeight)];
			unsigned int lineBytes24bit = (logo_head2.biWidth * 3 + 3) / 4 * 4;// 每行字节数必须为4的倍数
			unsigned int lineBytes16bit = (logo_head2.biWidth * 2 + 3) / 4 * 4;; // 每行字节数必须为4的倍数
			for(int k=0; k<abs(logo_head2.biHeight); k++)
			{
				logo_bmp_data_index = 0;
				for(int q=0; q<logo_head2.biWidth; q++)
				{
					p_BGR_data = (BGR_data *)(p_logo_bmp_source_data + lineBytes24bit * k + sizeof(BGR_data) * q);
					tmp_16bit=((uint16_t)(p_BGR_data->R_data>>3)<<11)//R G B
						+((uint16_t)(p_BGR_data->G_data>>2)<<5)
						+(uint16_t)(p_BGR_data->B_data>>3);
					p_logo_bmp_data[lineBytes16bit*k+logo_bmp_data_index++] = uint8_t(tmp_16bit&0xff);
					p_logo_bmp_data[lineBytes16bit*k+logo_bmp_data_index++] = uint8_t(tmp_16bit>>8);
				}
			}
			bmp565_write(p_logo_bmp_data, logo_head2.biWidth, abs(logo_head2.biHeight), "./24toRGB565_logo.bmp");
		}
		else
		{
			p_logo_bmp_data = p_logo_bmp_source_data;
			p_logo_bmp_source_data = NULL;
		}
		
		
		
		for(int i = 0; i < abs(logo_head2.biHeight); i++)
		{
			memcpy(p_bmp_data + pixel_line_byte * (i + logo_margin_top) + (biBitCount / 8) * logo_margin_left,
				p_logo_bmp_data + logo_line_byte * i,
				logo_head2.biWidth * (biBitCount / 8));
		}
	}
	
	//写文件头进文件  
	fwrite(&bit_map_file_header, sizeof(BIT_MAP_FILE_HEADER), 1, pf);
	//写位图信息头进文件
	fwrite(&bit_map_info_header, sizeof(BIT_MAP_INFO_HEADER), 1, pf);

	if(biBitCount == bmp_16bit_rgb565)
	{
		//写调色板信息进文件
		fwrite(&rgb_quad_mask, sizeof(RGB_QUAD_MASK), 1, pf);
	}
	
	//写数据进文件  
	fwrite(p_bmp_data, sizeof(unsigned char), bit_map_info_header.biSizeImage, pf);

_BMP_WRITE_END_:
	if(pf != NULL)
		fclose(pf);
	pf = NULL;
	
	if(pf_logo != NULL)
		fclose(pf_logo);
	pf_logo = NULL;
	
	if(p_bmp_data != NULL)
		delete[] p_bmp_data;
	p_bmp_data = NULL;
	
	if(p_logo_bmp_data != NULL)
		delete[] p_logo_bmp_data;
	p_logo_bmp_data = NULL;
	
	if(p_logo_bmp_source_data != NULL)
		delete[] p_logo_bmp_source_data;
	p_logo_bmp_source_data = NULL;
	
	return ret;
}

int main()  
{
	const char * QRTEXT = "http://chair.changchong.com.cn/szh5/index.html?portnumber=12345678";
	QRcode * qrCode;
	int version = 0;//设置版本号，设为1，对应尺寸：21 * 21  步长为4
	QRecLevel level = QR_ECLEVEL_Q;
	QRencodeMode hint = QR_MODE_8;
	int casesensitive = 1;  
	qrCode = QRcode_encodeString(QRTEXT, version, level, hint, casesensitive);
	if (NULL == qrCode)  
	{  
		printf("QRcode create fail\n");  
		return -1;
	}  
	printf("QRcode data, version : %d, width : %d\n", qrCode->version, qrCode->width);
	//将要生成的二维码保存为BMP真彩色图片文件
	#if 1
	bmp_write(qrCode, bmp_24bit, "./out_map/24bit.bmp");
	bmp_write(qrCode, bmp_24bit, "./out_map/24bitlogo.bmp", "./logo/24logo.bmp");
	bmp_write(qrCode, bmp_16bit_rgb565, "./out_map/rgb565.bmp");
	bmp_write(qrCode, bmp_16bit_rgb565, "./out_map/rgb565logo.bmp", "./logo/24logo.bmp");
	#endif
	#if 0
	
	FILE * pf = fopen("./qrcode_demo.bmp", "wb");
	if (NULL == pf)  
	{  
		printf("file open fail.\n");
		fclose(pf);
		return -1;
	}
	
	int width = qrCode->width;  
	int height = qrCode->width;  
	int biCount = 24;//真彩色  
	int line_byte = (width * biCount / 8 + 3) / 4 * 4; //每line字节数必须为4的倍数  
	//位图文件头  
	BIT_MAP_FILE_HEADER bit_map_file_header;  
	bit_map_file_header.bfType = 0x4D42;  
	bit_map_file_header.bfSize = sizeof(BIT_MAP_FILE_HEADER) + sizeof(BIT_MAP_INFO_HEADER) + line_byte * height;  
	bit_map_file_header.bfReserved1 = 0;
	bit_map_file_header.bfReserved2 = 0;
	bit_map_file_header.bfOffBits = sizeof(BIT_MAP_FILE_HEADER) + sizeof(BIT_MAP_INFO_HEADER);  
	//位图信息头  
	BIT_MAP_INFO_HEADER bit_map_info_header;  
	bit_map_info_header.biBitCount = biCount;  
	bit_map_info_header.biClrImportant = 0;  
	bit_map_info_header.biClrUsed = 0;  
	bit_map_info_header.biCompression = 0;  
	bit_map_info_header.biHeight = height;  
	bit_map_info_header.biPlanes = 1;  
	bit_map_info_header.biSize = 40;  
	bit_map_info_header.biSizeImage = line_byte * height;  
	bit_map_info_header.biWidth = width;  
	bit_map_info_header.biXPelsPerMeter = 0;  
	bit_map_info_header.biYPelsPerMeter = 0;  
	//写文件头进文件  
	fwrite(&bit_map_file_header, sizeof(BIT_MAP_FILE_HEADER), 1, pf);  
	//写位图信息头进文件  
	fwrite(&bit_map_info_header, sizeof(BIT_MAP_INFO_HEADER), 1, pf);  
	unsigned char * pBMPData = new unsigned char[line_byte * height];  
	memset(pBMPData, 255, line_byte * height);  

	unsigned char * qr_data = qrCode->data;  
	for (int i = 0; i < height; i++)  
	{  
		for (int j = 0; j < line_byte/3; j++)
		{  
			if (*(qr_data) & 1)  
			{  
				//设置rgb颜色，可自定义设置，这里设为黑色。  
				*(pBMPData + line_byte * i + 3 * j) = 0;  
				*(pBMPData + line_byte * i + 3 * j + 1) = 0;  
				*(pBMPData + line_byte * i + 3 * j + 2) = 0;  
			}  
			qr_data++;  
		}  
	}  

	//写数据进文件  
	fwrite(pBMPData, sizeof(unsigned char), line_byte * height, pf);  
	fclose(pf);  
	delete[] pBMPData;  
	pBMPData = NULL;  
	#endif
	QRcode_free(qrCode);  
	return 0;  
}  
