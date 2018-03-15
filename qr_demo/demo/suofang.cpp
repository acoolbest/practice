由于在OPOS开发中需要对图片做缩放处理，所以就研究了一下比较简单的最邻近插值法。这种算法的原理就是依据原图像和目标图像的尺寸，计算缩放的比例，然后根据缩放比例计算目标像素所依赖的原像素。计算过程中难免会产生小数，这是就采用四舍五入，取与这个点最近的点的值。
网上也有很多算法代码可查，但是一般都是对24位的图片处理，即使有其它位深度图片的处理，多少还是存在瑕疵。以下的代码是我在项目写的一个处理函数。（4位的等会在以后更新）

bool bmpzoom(const char* szSrcBmp, const char* szDstBmp, double Rate)
{
	long i,j,k;
	long i0,j0;
	double FZOOMRATIO = Rate;

	readBmp(szSrcBmp);
	newBmpWidth = (long) (bmpWidth * FZOOMRATIO +0.5); //FZOOMRATIO 为缩放比例
	newBmpHeight = (long) (bmpHeight * FZOOMRATIO +0.5);

	int DataSizePerLine = (newBmpWidth * biBitCount + 8 * biBitCount - 1) / (8 * biBitCount); //每行实际数据量的大小
	newLineByte = DataSizePerLine % 4 ? (DataSizePerLine / 4 *4 + 4) : DataSizePerLine;

	pNewBmpBuf = new unsigned char [newLineByte * newBmpHeight];
	pNewbit = new unsigned char [newLineByte * newBmpHeight*8];

	DataSizePerLine = (bmpWidth * biBitCount + 8 * biBitCount - 1) / (8 * biBitCount); //每行实际数据量的大小
	int lineByte = DataSizePerLine % 4 ? (DataSizePerLine / 4 *4 + 4) : DataSizePerLine;
	if (biBitCount==8)
	{
		for(i = 0; i < bmpHeight/2; i++)
		{
			for(j = 0; j < bmpWidth/2; j++);
			// *(pBmpBuf+i*lineByte+j) = 255;
		}
	}

	if (biBitCount == 24)
	{
		for(i = 0;i < newBmpHeight;i++)
		{
			for(j = 0; j<newBmpWidth;j++)
			{
				for(k=0;k<3;k++) 
				{   
					i0 = (long)(i/FZOOMRATIO+0.5); 
					j0 = (long)(j/FZOOMRATIO+0.5);
					if((j0 >=0) && (j0 < bmpWidth) && (i0 >=0)&& (i0 <bmpHeight))
					{
						*(pNewBmpBuf+i*newLineByte+j*3+k) = *(pBmpBuf+i0*lineByte+j0*3+k);
					}
					else
					{
						*(pNewBmpBuf+i*newLineByte+j*3+k)=255;
					}
				}
			}
		}
	}

	if (biBitCount == 8)
	{
		for(i = 0;i < newBmpHeight;i++)
		{
			for(j = 0; j<newBmpWidth;j++)
			{
				i0 = (long)(i/FZOOMRATIO+0.5); 
				j0 = (long)(j/FZOOMRATIO+0.5);
				if((j0 >=0) && (j0 < bmpWidth) && (i0 >=0)&& (i0 <bmpHeight))
				{
					*(pNewBmpBuf+i*newLineByte+j) = *(pBmpBuf+i0*lineByte+j0);
				}
				else
				{
					*(pNewBmpBuf+i*newLineByte+j)=0;
				}
			}
		}
	}

	if (biBitCount == 1)
	{
		for(i = 0;i<newBmpHeight;i++)
		{
			for(j = 0;j<newLineByte*8;j++)
			{
				i0 = (long)(i/FZOOMRATIO+0.5); 
				j0 = (long)(j/FZOOMRATIO+0.5);
				if((j0 >=0) && (j0 < lineByte*8) && (i0 >=0)&& (i0 <bmpHeight))
				{
					*(pNewbit+i*newLineByte*8+j) = *(pbit+i0*lineByte*8+j0);
				}
				else
				{
					*(pNewbit+i*newLineByte*8+j) = 255;
				}
			}
		}

		int k = 0;
		for(int x = 0; x < newLineByte*newBmpHeight*8; x += 8)
		{
			if (k<newLineByte*newBmpHeight)
			{
				pNewBmpBuf[k] = (pNewbit[x]<<7) + (pNewbit[x+1]<<6) + (pNewbit[x+2]<<5) + (pNewbit[x+3]<<4) + (pNewbit[x+4]<<3) + (pNewbit[x+5]<<2) + (pNewbit[x+6]<<1) + pNewbit[x+7];
				k++;
			}
		}
	}

	saveBmp(szDstBmp,pNewBmpBuf,newBmpWidth,newBmpHeight,biBitCount,pColorTable);
	delete []pNewBmpBuf;
	delete []pNewbit;
	if (biBitCount == 8 || biBitCount == 1)
		delete []pColorTable;
	return 1;
}
