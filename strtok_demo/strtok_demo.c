#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
int main()
{
	char buf[]="I am a good programer, am i? hello, world.";
	//			?i am ,programer good a am i

	char * p[50],ch,*str;
	int i=0,j=0,word_num=0;
	printf("***%s***\n",buf);
	str=(char *)malloc(strlen(buf)+1);
	p[i]=strtok(buf," ");//���ո��и��ַ���
	while(p[i]!=NULL)
	{
		i++;
		p[i]=strtok(NULL," ");
		//printf("%ld, %s\n", strlen(p[i]), p[i]);
	}
	printf("i = %d\n", i);
	word_num=i;
	for(i=0; i< word_num; i++)//�����е������б����ŵĴ�����
	{
		if(ispunct(   *(p[i]+strlen(p[i])-1)     )  !=0    )//�˵��ʺ���Ǹ�������
		{
			ch=*(p[i]+strlen(p[i])-1);
			for(j=strlen(p[i]);j>1 ;j--)
			{
				*(p[i]+j-1)=*(p[i]+j-2);
			}
			*p[i]=ch;
		}
	}
	i=word_num;
	strcpy(str,p[i-1]);
	for(i=word_num-2; i>=0  ;i--)//�Ӻ���ǰ�������ַ���׷�ӽ������Կո�ֿ�
	{
		strcat(str," ");
		strcat(str,p[i]);			
	}
	
	strcpy(buf,str);
	printf("***%s***\n",buf);
	free(str);
	return 0;
}