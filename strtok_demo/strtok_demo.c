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
	p[i]=strtok(buf," ");//按空格切割字符串
	while(p[i]!=NULL)
	{
		i++;
		p[i]=strtok(NULL," ");
		//printf("%ld, %s\n", strlen(p[i]), p[i]);
	}
	printf("i = %d\n", i);
	word_num=i;
	for(i=0; i< word_num; i++)//把所有单词中有标点符号的处理完
	{
		if(ispunct(   *(p[i]+strlen(p[i])-1)     )  !=0    )//此单词后边是个标点符号
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
	for(i=word_num-2; i>=0  ;i--)//从后向前把所有字符串追加进来，以空格分开
	{
		strcat(str," ");
		strcat(str,p[i]);			
	}
	
	strcpy(buf,str);
	printf("***%s***\n",buf);
	free(str);
	return 0;
}