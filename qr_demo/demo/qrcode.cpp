//QRGenerator.h  
//#pragma once  
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <qrencode.h> 
#include <stdint.h>

using std::string;  
using std::cout;
using std::endl;

#define OUT_FILE_PIXEL_PRESCALER    8      


class QRGenerator  
{  
	public:  
		QRGenerator(void);  
		~QRGenerator(void);  

		static void generate(string& text, string& bmpSavedPath);  
};  

//QRGenerator.cpp  

//#include "QRGenerator.h"  
//#include "LibQREncode/qrencode.h"  

//#pragma comment(lib,"lib\\debug\\qrencode.lib")  

#define PIXEL_COLOR_R               0                                           // Color of bmp pixels  
#define PIXEL_COLOR_G               0  
#define PIXEL_COLOR_B               0xff  

//BMP defines  
typedef uint16_t  WORD;  
typedef uint32_t   DWORD;  
typedef int32_t     LONG;  

#define BI_RGB          0L  

typedef struct  
{  
	WORD    bfType;  
	DWORD   bfSize;  
	DWORD   bfReserved;  
	DWORD   bfOffBits;  
} BITMAPFILEHEADER;  

typedef struct  
{  
	DWORD      biSize;  
	LONG       biWidth;  
	LONG       biHeight;  
	WORD       biPlanes;  
	WORD       biBitCount;  
	DWORD      biCompression;  
	DWORD      biSizeImage;  
	LONG       biXPelsPerMeter;  
	LONG       biYPelsPerMeter;  
	DWORD      biClrUsed;  
	DWORD      biClrImportant;  
} BITMAPINFOHEADER;  

//#pragma pack(pop)  

QRGenerator::QRGenerator(void)  
{  

}  

QRGenerator::~QRGenerator(void)  
{  

}  

void QRGenerator::generate(string& text, string& bmpSavedPath)  
{  
	const char*     szSourceSring   = text.c_str();  
	const char*     szSavePathSring = bmpSavedPath.c_str();  
	unsigned int    unVersion, unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;  
	unsigned char*  pRGBData, *pSourceData, *pDestData;  
	QRcode*         pQRC;  
	FILE*           f;  

	if (pQRC = QRcode_encodeString(szSourceSring, 0, QR_ECLEVEL_H, QR_MODE_8, 1))  
	{  
		//int realwidth = (qrcode->width + margin * 2) * size;
		unWidth = pQRC->width;//矩阵的维数 
		unVersion = pQRC->version;
		unWidthAdjusted = unWidth * OUT_FILE_PIXEL_PRESCALER * 3;//每一个维度占的像素的个数（8），每个像素3个字节
		if (unWidthAdjusted % 4)  
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;  
		unDataBytes = unWidthAdjusted * unWidth * OUT_FILE_PIXEL_PRESCALER;  

		// Allocate pixels buffer  
		if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))  
		{  
			printf("Out of memory");
			exit(-1);
		}
		
		// Preset to white  
		memset(pRGBData, 0xff, unDataBytes);


		// Prepare bmp headers  
		BITMAPFILEHEADER kFileHeader;  
		kFileHeader.bfType = 0x4d42;  // "BM"  
		cout << sizeof(BITMAPFILEHEADER) << " " <<  sizeof(BITMAPINFOHEADER) << endl;
		kFileHeader.bfSize = 54 + unDataBytes;
		kFileHeader.bfReserved = 0;

		kFileHeader.bfOffBits = 54;

		BITMAPINFOHEADER kInfoHeader;  
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);  
		kInfoHeader.biWidth = unWidth * OUT_FILE_PIXEL_PRESCALER;
		kInfoHeader.biHeight = -((int)unWidth * OUT_FILE_PIXEL_PRESCALER);//负数可以控制图像上下颠倒
		kInfoHeader.biPlanes = 1;  
		kInfoHeader.biBitCount = 24;  
		kInfoHeader.biCompression = BI_RGB;  
		kInfoHeader.biSizeImage = 0;  
		kInfoHeader.biXPelsPerMeter = 0;  
		kInfoHeader.biYPelsPerMeter = 0;  
		kInfoHeader.biClrUsed = 0;  
		kInfoHeader.biClrImportant = 0;  


		// Convert QrCode bits to bmp pixels  
		pSourceData = pQRC->data;  
		for (y = 0; y < unWidth; y++)  
		{  
			pDestData = pRGBData + unWidthAdjusted * y * OUT_FILE_PIXEL_PRESCALER;  
			for (x = 0; x < unWidth; x++)  
			{  
				if (*pSourceData & 1)  
				{  
					for (l = 0; l < OUT_FILE_PIXEL_PRESCALER; l++)  
					{  
						for (n = 0; n < OUT_FILE_PIXEL_PRESCALER; n++)  
						{  
							*(pDestData + n * 3 + unWidthAdjusted * l)     = PIXEL_COLOR_B;  
							*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_G;  
							*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_R;  
						}  
					}  
				}  
				pDestData += 3 * OUT_FILE_PIXEL_PRESCALER;  
				pSourceData++;  
			}  
		}  


		// Output the bmp file  
		if (f = fopen(szSavePathSring, "wb"))
		{  
			fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, f);  
			fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);  
			fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);  
			fclose(f);  
		}  

		// Free data  
		free(pRGBData);  
		QRcode_free(pQRC);  
	}  
}  

//main.cpp  

//
//#include "QRGenerator.h"  

void qrcode_encode(string str_url, string port_number, string logo_path)
{
	string str_info = str_url + "?portnumber=" + port_number;
	string save_path = "./" + port_number + "_out.bmp";
	QRGenerator::generate(str_info, save_path);
}

int main()  
{
	qrcode_encode("http://chair.changchong.com.cn/szh5/index.html", "12462684", "./logo.bmp");
#if 0
	string info = "IP:192.168.1.103,Port:9999";  
	string savePath = "./info.bmp";  
	QRGenerator::generate(info, savePath);  
#endif
	return 0;  
}
