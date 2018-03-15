/**
	24位深的bmp图片转换为16位深RGB565格式的bmp图片
 **/
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

using namespace std;

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short  uint16; //32bit system
typedef short  int16;
typedef unsigned int uint32;//32bit system
typedef int int32;

#pragma pack(push, 2)
//#pragma pack(2)
struct BIT_MAP_FILE_HEADER  
{   
	uint16_t bfType;       					// [0-1] bfType:必须是BM字符
	uint32_t bfSize;       					// [2-5] bfSize:总文件大小
	uint16_t bfReserved1;  					
	uint16_t bfReserved2;  					// [6-9] brReserved1,bfReserved2:保留
	uint32_t bfOffBits;    					// [10-13] bfOffBits:到图像数据的偏移
}head1;

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
}head2;

struct RGB_QUAD_MASK {
	uint32_t red_mask;						// [54-57] 红色掩码
	uint32_t green_mask;					// [58-61] 绿色掩码
	uint32_t blue_mask;						// [62-65] 蓝色掩码
	uint32_t alpha;							// [66-69] 缺省为0
};
#pragma pack(pop)

struct bmp_filehead //14B
{
	//2B 'BM'=0x424d;   
	uint16 bmtype; //'BM',2B  
	uint32 bmSize; //size,4B  
	uint32 bmReserved;//0x00,4B
	uint32 bmOffBits;//4B,... 54

};

struct bmphead //40B
{
	uint32 bmpOffBits;//4B,... 40
	uint32 bmp_wide;//4B,wide
	uint32 bmp_high;//4B,high 
	uint16 bmplans;//2B,0 or 1
	uint16 bitcount;//2B ,24
	uint32 bmpyasuo;//4B,0
	uint32 imagesize;//4B,w*h*3

	uint32 biXPelsPerMeter;
	uint32 biYPelsPerMeter;
	uint32 biClrUsed;
	uint32 biClrImportant;
};

struct BGR_data //40B
{
	uint8 B_data;
	uint8 G_data;
	uint8 R_data;
}clr_data[1024*768]; //img size must less than 1024*768 

bool bmp565_write(unsigned char *image, uint32 width, uint32 height, const char *filename)      
{
	unsigned char buffer[1024*1024] = {0};
	uint32 file_size;     
	uint32 data_size;     
	unsigned int widthAlignBytes;     
	FILE *fp;     
	
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
	
	widthAlignBytes = ((width * 16 + 31) & ~31) / 8; // 每行字节数必须为4的倍数 //int lineByte = (width * 16 / 8 + 3) / 4 * 4; 
	
	unsigned int lineBytes24bit = (head2.biWidth * 3 + 3) / 4 * 4;// 每行字节数必须为4的倍数
	unsigned int lineBytes16bit = (head2.biWidth * 2 + 3) / 4 * 4;; // 每行字节数必须为4的倍数
    data_size = lineBytes16bit * height;      // 图像数据大小     
    file_size = data_size + sizeof(header);    // 整个文件的大小
	
	printf("data_size:%d, lineBytes16bit:%d, lineBytes24bit: %d, width:%d, height:%d\n",data_size, lineBytes16bit, lineBytes24bit, width, height);
	
	*((uint32*)(header + 2)) = file_size;     
	*((uint32*)(header + 18)) = width;     
	*((uint32*)(header + 22)) = height;     
	*((uint32*)(header + 34)) = data_size;     
	
	if (!(fp = fopen(filename, "wb")))
		return false;     
	
	fwrite(header, sizeof(unsigned char), sizeof(header), fp);
	
	fwrite(image, sizeof(unsigned char), (size_t)(data_size), fp);
	
	fclose(fp);     
	return true;     
}
int main(int argc, char *argv[])
{
	unsigned char image_data[1024*1024] = {0};
	unsigned int image_index = 0;
	FILE *fp,*out_fp;
	int32 k,q;
	uint32 rec_cout,rec_cout2;
	uint16 tp_16bit;
	int8 src_file[64] = "";
	int8 dst_file[64] = "";
	
	if(argc < 2)
	{
		cout << "\nUsage:pls input src file path and dst file path\n" << endl;
		return -1;
	}
	printf("\n********************************************************\n"); 
	printf("*********24 bits bmp to 16 bits RGB565 bmp V1.0*********\n");
	printf("*********************zhzq 2017.5.17*********************\n");
	printf("********************************************************\n\n");
	strcpy(src_file, argv[1]);
	if(argc == 3)
	{
		strcpy(dst_file, argv[2]);
		cout << "dst file name : " << dst_file << endl;
	}
	else
	{
		strncpy(dst_file, src_file, strlen(src_file)-4);
		strcat(dst_file, "_rgb565.bmp");
		cout << "we set dst file default name : " << dst_file << endl;
	}
		

	if((fp=fopen(src_file,"r"))==NULL)
		cout<<"open error!"<<endl;
	else
	{
		fseek(fp, 0, 0);//fseek(fp, offset, fromwhere);
		rec_cout=fread(&head1,sizeof(BIT_MAP_FILE_HEADER),1,fp);

		fseek(fp, 14, 0);//fseek(fp, offset, fromwhere);
		rec_cout2=fread(&head2,sizeof(BIT_MAP_INFO_HEADER),1,fp);

		//fseek(fp, head1.bmOffBits, 0);//fseek(fp, offset, fromwhere);
		//rec_cout2=fread(&clr_data,3,(head2.bmp_wide*head2.bmp_high),fp);
		
		unsigned char * p_source_data = new unsigned char[head2.biSizeImage];
		fseek(fp, head1.bfOffBits, 0);//fseek(fp, offset, fromwhere);
		rec_cout2=fread(p_source_data,sizeof(unsigned char),head2.biSizeImage,fp);
		fclose(fp);

		cout<<"open ok!"<<endl;
		
		unsigned int lineBytes24bit = (head2.biWidth * 3 + 3) / 4 * 4;// 每行字节数必须为4的倍数
		unsigned int lineBytes16bit = (head2.biWidth * 2 + 3) / 4 * 4;; // 每行字节数必须为4的倍数
		
		BGR_data * p_BGR_data = NULL;
		
		if(rec_cout>=0)
		{
			cout<<"write bmp 565 start !"<<endl;
			for(k=0;k<head2.biHeight;k++)
			{
				image_index = 0;
				for(q=0;q<head2.biWidth;q++)
				{
					#if 0
					tp_16bit=((uint16)(clr_data[k*head2.bmp_wide+q].R_data>>3)<<11)//R G B
						+((uint16)(clr_data[k*head2.bmp_wide+q].G_data>>2)<<5)
						+(uint16)(clr_data[k*head2.bmp_wide+q].B_data>>3);
					image_data[lineBytes16bit*k+image_index++] = uint8(tp_16bit&0xff);
					image_data[lineBytes16bit*k+image_index++] = uint8(tp_16bit>>8);
					#endif
				
					p_BGR_data = (BGR_data *)(p_source_data + lineBytes24bit * k + sizeof(BGR_data) * q);
					tp_16bit=((uint16)(p_BGR_data->R_data>>3)<<11)//R G B
						+((uint16)(p_BGR_data->G_data>>2)<<5)
						+(uint16)(p_BGR_data->B_data>>3);
					cout << tp_16bit << endl;
					image_data[lineBytes16bit*k+image_index++] = uint8(tp_16bit&0xff);
					image_data[lineBytes16bit*k+image_index++] = uint8(tp_16bit>>8);
				}
			}
			if(bmp565_write(image_data, head2.biWidth, head2.biHeight, dst_file))
				cout<<"write bmp 565 success !"<<endl;
			else
				cout<<"write bmp 565 failed !"<<endl;
			
			#if 0
			cout<<"rt="<<rec_cout<<endl<<endl;

			printf("typ=0x%04X \n",head1.bmtype);
			printf("bmSize=0x%08X \n",head1.bmSize);
			printf("bmReserved=0x%08X \n",head1.bmReserved);
			printf("bmOffBits=0x%08X \n\n",head1.bmOffBits);


			printf("bmpOffBits=0x%08X \n",head2.bmpOffBits);
			printf("bmp_wide=0x%08X \n",head2.bmp_wide);
			printf("bmp_high=0x%08X \n",head2.bmp_high);
			printf("bitcount=0x%08X \n",head2.bitcount);
			printf("imagesize=0x%08X \n",head2.imagesize);

			for(k=(0+head2.bmp_wide*(head2.bmp_high/2));
					k<(100+head2.bmp_wide*(head2.bmp_high/2));k++)
			{
				printf("BGR=0x%02X,0x%02X,0x%02X \n",clr_data[k].B_data,
						clr_data[k].G_data,clr_data[k].R_data);
			}
			
			if((out_fp=fopen("Imag.C","w+"))==NULL)
			{
				printf("can't open\n");
			}
			else
			{
				printf("Open success!!\n");
				//fseek(out_fp, 0, 0);//fseek(fp, offset, fromwhere);
				//rec_cout=fwrite(&clr_data,100,1,out_fp);
				fprintf(out_fp,"const unsigned char gImage_Imag[%d] = { ",
						head2.bmp_wide*head2.bmp_high*2+8);

				fprintf(out_fp,
						"0X00,0X10,0X%02X,0X%02X,0X%02X,0X%02X,0X01,0X1B,\n",
						/*scan,gray,wL,wH,hL,hH,is565,rgb*/
						head2.bmp_wide&0xff,head2.bmp_wide>>8,
						head2.bmp_high&0xff,head2.bmp_high>>8);

				//for(k=0;k<head2.bmp_wide*head2.bmp_high;k++)
				for(k=(head2.bmp_high-1);k>=0;k--)
					for(q=0;q<head2.bmp_wide;q++)
					{
						tp_16bit=((uint16)(clr_data[k*head2.bmp_wide+q].R_data>>3)<<11)//R G B
							+((uint16)(clr_data[k*head2.bmp_wide+q].G_data>>2)<<5)
							+(uint16)(clr_data[k*head2.bmp_wide+q].B_data>>3);

						fprintf(out_fp, "0X%02X,0X%02X,",(tp_16bit&0xff),(tp_16bit>>8)); //L H

						if((q+1)%8==0)
							fprintf(out_fp, "\r\n");
					}
				fprintf(out_fp, "};\n");
				fclose(fp);
			}
			#endif
		}
		else
			cout<<"read error"<<endl;
		
		if(p_source_data != NULL)
			delete[] p_source_data;
		p_source_data = NULL;
	}
	return 0;
}
