/*
   前几天碰上需要对bmp位图进行缩放的功能,
   调用API函数,虽然能实现位图缩放,但是对有放大的效果好,缩小会造成失真,图像上有花点,让人难以接受
   ,因为本人以前学易语言,易语言有一段代码,对bmp图像缩放效果非常 好,
   昨天抽空,把它翻译成c++代码了,经验证,非常 好用,帖上代码:
   */
//	这一段是对宽度进行缩放:

void __stdcall bmpsetH(unsigned char *bitmap,int h,unsigned char *bitmap2)
{
	int i,j,k,bitmapH1,bitmapH2,bitmapV1,bitmapV2,bitmapSize1,bitmapSize2,bitmapLen1,bitmapLen2;
	int startH1,startH2,R,G,B,R1,G1,B1;	
	// 	位图宽1 ＝ 取字节集数据 (位图数据, #整数型, 19)
	bitmapH1=*((int*)(bitmap+18));
	bitmapV1=*((int*)(bitmap+22));
	int redress1=bitmapH1%4;
	int bitmapNH1=bitmapH1*3+redress1;
	bitmapLen1=bitmapNH1*bitmapV1;
	bitmapSize1=bitmapLen1+54;
	int redress2=h%4;
	int bitmapNH2=h*3+redress2;
	bitmapLen2=bitmapNH2*bitmapV1;
	bitmapSize2=bitmapLen2+54;
	memcpy(bitmap2,bitmap,54);
	memcpy(bitmap2+2,&bitmapSize2,4);
	memcpy(bitmap2+18,&h,4);
	double Hval=(double)bitmapH1/h;
	double Hvala=0;
	int Hvalb=0,Hvalc=0;
	for (i=1;i<=bitmapV1;i++)
	{
		startH2=bitmapSize2-i*bitmapNH2;
		startH1=bitmapSize1-i*bitmapNH1;
		B1=bitmap[startH1];
		G1=bitmap[startH1+1];
		R1=bitmap[startH1+2];
		for(j=1;j<=h;j++)
		{
			startH2=startH2+3;
			Hvala=Hvala+Hval;
			if (Hvala<1)
			{
				Hvalb=3;
			}
			else
			{
				Hvalc=Hvala;
				Hvalb=Hvalc*3;
			}
			startH1=bitmapSize1-i*bitmapNH1+Hvalb;
			B=(bitmap[startH1-3]+B1)/2;
			G=(bitmap[startH1-2]+G1)/2;
			R=(bitmap[startH1-1]+R1)/2;
			bitmap2[startH2-3]=B;
			bitmap2[startH2-2]=G;
			bitmap2[startH2-1]=R;
			B1=bitmap[startH1-3];
			G1=bitmap[startH1-2];
			R1=bitmap[startH1-1];

		}
		Hvala=0;
	}

}
//这一段是对高度进行缩放:
void BmpSetV(unsigned char *bitmap,int v,unsigned char *bitmap2)
{	

	double Vval,Vvala=0;
	int i,j,Vvalb=0,bitH1,bitNH1,bitV1,redress,bitLen1,bitsize1,bitLen2,bitsize2;
	int start3=0,start2=0,start1=0,G1,B1,R1,R,G,B;
	bitH1=*((int*)(bitmap+18));
	bitV1=*((int*)(bitmap+22));
	redress=bitH1%4;
	bitNH1=bitH1*3+redress;
	bitLen1=bitNH1*bitV1;
	bitsize1=bitLen1+54;
	bitLen2=bitNH1*v;
	bitsize2=bitLen2+54;
	memcpy(bitmap2,bitmap,54);
	memcpy(bitmap2+2,&bitsize2,4);
	memcpy(bitmap2+22,&v,4);
	Vval=bitV1/(double)v;
	for (i=1;i<=bitH1;i++)
	{
		start3=bitsize1-bitNH1+i*3;
		B1=bitmap[start3-3];
		G1=bitmap[start3-2];
		R1=bitmap[start3-1];
		for (j=1;j<=v;j++)
		{
			Vvala=Vvala+Vval;
			if (Vvala<1)Vvalb=1;
			else Vvalb=Vvala;
			start2=bitsize2-j*bitNH1+i*3;
			start1=bitsize1-Vvalb*bitNH1+i*3;
			if (Vvalb<bitV1)
			{
				start3=bitsize1-Vvalb*bitNH1-bitNH1+i*3;
				if(i==1)
				{
					bitmap2[start2-3]=bitmap[start1-3];
					bitmap2[start2-2]=bitmap[start1-2];
					bitmap2[start2-1]=bitmap[start1-1];
				}
				else
				{
					B=(B1+bitmap[start1-3])/2;
					G=(G1+bitmap[start1-2])/2;
					R=(R1+bitmap[start1-1])/2;
					bitmap2[start2-3]=B;
					bitmap2[start2-2]=G;
					bitmap2[start2-1]=R;
					B1=bitmap[start3-3];
					G1=bitmap[start3-2];
					R1=bitmap[start3-1];
				}
			}
			else
			{
				bitmap2[start2-3]=bitmap[start1-3];
				bitmap2[start2-2]=bitmap[start1-2];
				bitmap2[start2-1]=bitmap[start1-1];
			}


		}
		Vvala=0;
	}

}
//示例:
int main(int argc, char* argv[])
{
	unsigned char *bmp,*bmp2;
	bmp=(unsigned char*)malloc(1244214);
	bmp2=(unsigned char*)malloc(1244214);
	FILE *fp=fopen("D:\\开机画面.bmp","rb");
	fread(bmp,1,304182,fp);

	fclose(fp);
	//bmpsetH(bmp,720,bmp2);
	// memcpy(bmp,bmp2,1244214);
	BmpSetV(bmp,290,bmp2);
	fp=fopen("D:\\开机.bmp","wb");
	fwrite(bmp2,1,1244214,fp);

	fclose(fp);
	free(bmp);
	free(bmp2);
	return 0;
}
//看到这确实不容易，这里脚本之家小编就为大家分享另一个代码
// suofang.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include 

struct header{
	short BM;
	unsigned int filesize;
	unsigned int recv;
	unsigned int offset;
	unsigned int bitmapheadlong;
	unsigned int bitmapwith;
	unsigned int bitmaphight;
	short bitmappageinfo;
	short bitperpixel;
	unsigned int compress;
	unsigned int bitmapsize;
	unsigned int levelresolution;
	unsigned int verticalresolution;
	unsigned int colourmap;
	unsigned int colourmapsize;
}bitmapheader;
#if 0
　　typedef struct tagBITMAPFILEHEADER
　　{
	　　WORD bfType; // 位图文件的类型，必须为BM(0-1字节)
	　　DWORD bfSize; // 位图文件的大小，以字节为单位(2-5字节)
	　　WORD bfReserved1; // 位图文件保留字，必须为0(6-7字节)
	　　WORD bfReserved2; // 位图文件保留字，必须为0(8-9字节)
	　　DWORD bfOffBits; // 位图数据的起始位置，以相对于位图(10-13字节)
	　　// 文件头的偏移量表示，以字节为单位
		　　} BITMAPFILEHEADER; 

		typedef struct tagBITMAPINFOHEADER{
			　　DWORD biSize; // 本结构所占用字节数(14-17字节)
			　　LONG biWidth; // 位图的宽度，以像素为单位(18-21字节)
			　　LONG biHeight; // 位图的高度，以像素为单位(22-25字节)
			　　WORD biPlanes; // 目标设备的级别，必须为1(26-27字节)
			　　WORD biBitCount;// 每个像素所需的位数，必须是1(双色),(28-29字节)
			　　// 4(16色)，8(256色)或24(真彩色)之一
				　　DWORD biCompression; // 位图压缩类型，必须是 0(不压缩),(30-33字节)
			　　// 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
				　　DWORD biSizeImage; // 位图的大小，以字节为单位(34-37字节)
			　　LONG biXPelsPerMeter; // 位图水平分辨率，每米像素数(38-41字节)
			　　LONG biYPelsPerMeter; // 位图垂直分辨率，每米像素数(42-45字节)
			　　DWORD biClrUsed;// 位图实际使用的颜色表中的颜色数(46-49字节)
			　　DWORD biClrImportant;// 位图显示过程中重要的颜色数(50-53字节)
			　　} BITMAPINFOHEADER;

			　typedef struct tagRGBQUAD {
				　　BYTE rgbBlue;// 蓝色的亮度(值范围为0-255)
				　　BYTE rgbGreen; // 绿色的亮度(值范围为0-255)
				　　BYTE rgbRed; // 红色的亮度(值范围为0-255)
				　　BYTE rgbReserved;// 保留，必须为0
				　　} RGBQUAD; 
#endif
void scale(int srcwith,int srcheight,int destwith,int destheight)
{

}
int _tmain(int argc, _TCHAR* argv[4000000])
{
	BITMAPFILEHEADER bmfHdr; 
	BITMAPINFOHEADER bi,bi1;  
	CFile file("d:\\pic1.bmp",CFile::modeRead);
	file.Read(&bmfHdr, sizeof(BITMAPFILEHEADER)); 
	file.Read(&bi,sizeof(BITMAPINFOHEADER));
	DWORD dwSize = (bi.biWidth*bi.biBitCount+31)/32*4*bi.biHeight; 
	PBYTE pBuf = new BYTE[dwSize]; 
	file.Read(pBuf,dwSize); 
	file.Close(); 

	/////////////////////////////////////////////////////////////////////////
	file.Open("d:\\tmp1.bmp",CFile::modeCreate|CFile::modeReadWrite); 
	memcpy(&bi1,&bi, sizeof(BITMAPINFOHEADER)); 
	bi1.biWidth = 4000; //400->200 ,718->359
	bi1.biHeight = 4000; //266->133,397->794
	DWORD dwSize1 = (bi1.biWidth * bi1.biBitCount + 31)/32*4*bi1.biHeight; 
	PBYTE pBuf1 = new BYTE[dwSize1]; 
	BYTE *pSrc,*pDest; 

	//ofstream file1("D:\\tmp.log");

#if 0
	///////////////////////////////////最邻近值采样法（速度快）////////////////////////////////////////////////
	float m_xscale,m_yscale;
	m_xscale = (float)bi.biWidth/(float)bi1.biWidth;
	m_yscale = (float)bi.biHeight/(float)bi1.biHeight;
	unsigned long k=0;
	for(int y = 0; y  {
			for(int x=0; x    {
					pSrc = pBuf+(int)(y*m_yscale)*bi.biWidth*3+(int)(x*m_xscale)*3;
					pDest = pBuf1+(int)(y*bi1.biWidth*3+x*3);
					memcpy(pDest,pSrc,3);
					//*(unsigned long*)pDest=k++;
					//*(unsigned long*)pDest=0x000000ff;
					}
					}
					////////////////////////////////////缩放结束///////////////////////////////////////////////
#endif

#if 1
					/////////////////////////////////////双线性插值法（经过优化）/////////////////////////////////////////////////////
					int sw = bi.biWidth - 1, sh = bi.biHeight - 1, dw = bi1.biWidth - 1, dh = bi1.biHeight - 1;    //源图像宽度，目标图像宽度
					int B, N, x, y;                                          //计算出的目标点对应于源图像中的浮点数横坐标N、纵坐标B,目标整数横坐标x、纵坐标y
					int nPixelSize = bi.biBitCount/8;          //像素大小
					BYTE * pLinePrev, *pLineNext;            //源图像中的行开始坐标和下一行开始坐标
					//BYTE * pDest;
					BYTE * pA, *pB, *pC, *pD;              //源图像中最邻近的四个点
					for ( int i = 0; i <= dh; ++i )      //高度递增
					{
						pDest = ( BYTE * )(pBuf1+bi1.biWidth*i*nPixelSize);
						y = i * sh / dh;
						N = dh - i * sh % dh;
						pLinePrev = ( BYTE * )(pBuf+bi.biWidth*y*nPixelSize);
						y++;
						pLineNext = ( N == dh ) ? pLinePrev : ( BYTE * )(pBuf+bi.biWidth*y*nPixelSize);
						for ( int j = 0; j <= dw; ++j )    //宽度递增
						{
							x = j * sw / dw * nPixelSize;
							B = dw - j * sw % dw;
							pA = pLinePrev + x;
							pB = pA + nPixelSize;
							pC = pLineNext + x;
							pD = pC + nPixelSize;
							if ( B == dw )
							{
								pB = pA;
								pD = pC;
							}
							for ( int k = 0; k < nPixelSize; ++k )
								*pDest++ = ( BYTE )( int )(
										( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
										  + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
										  + dw * dh / 2 ) / (double)( dw * dh )
										);
						}
					}
#endif

	file.Write(&bmfHdr,sizeof(BITMAPFILEHEADER)); 
	file.Write(&bi1,sizeof(BITMAPINFOHEADER)); 
	file.Write(pBuf1,dwSize1); 
	file.Close(); 
	//==release p memory==
	delete pBuf;
	pBuf = NULL;
	delete pBuf1;
	pBuf1 = NULL;
	//==release p memory===
	if (pDest!=NULL)
	{
		pDest = NULL;
	}
	//if (pSrc!=NULL)
	//{
	//pSrc = NULL;
	//}
	//////////////////////////////////////////////////////////////
	//MessageBox("Test successful","Info",MB_OKCANCEL);

	return 0;
}
