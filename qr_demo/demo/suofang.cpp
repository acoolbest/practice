������OPOS��������Ҫ��ͼƬ�����Ŵ������Ծ��о���һ�±Ƚϼ򵥵����ڽ���ֵ���������㷨��ԭ���������ԭͼ���Ŀ��ͼ��ĳߴ磬�������ŵı�����Ȼ��������ű�������Ŀ��������������ԭ���ء������������������С�������ǾͲ����������룬ȡ�����������ĵ��ֵ��
����Ҳ�кܶ��㷨����ɲ飬����һ�㶼�Ƕ�24λ��ͼƬ������ʹ������λ���ͼƬ�Ĵ������ٻ��Ǵ���覴á����µĴ�����������Ŀд��һ������������4λ�ĵȻ����Ժ���£�

bool bmpzoom(const char* szSrcBmp, const char* szDstBmp, double Rate)
{
	long i,j,k;
	long i0,j0;
	double FZOOMRATIO = Rate;

	readBmp(szSrcBmp);
	newBmpWidth = (long) (bmpWidth * FZOOMRATIO +0.5); //FZOOMRATIO Ϊ���ű���
	newBmpHeight = (long) (bmpHeight * FZOOMRATIO +0.5);

	int DataSizePerLine = (newBmpWidth * biBitCount + 8 * biBitCount - 1) / (8 * biBitCount); //ÿ��ʵ���������Ĵ�С
	newLineByte = DataSizePerLine % 4 ? (DataSizePerLine / 4 *4 + 4) : DataSizePerLine;

	pNewBmpBuf = new unsigned char [newLineByte * newBmpHeight];
	pNewbit = new unsigned char [newLineByte * newBmpHeight*8];

	DataSizePerLine = (bmpWidth * biBitCount + 8 * biBitCount - 1) / (8 * biBitCount); //ÿ��ʵ���������Ĵ�С
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
