/* ************************************************************************
 *       Filename:  webserver.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013年12月16日 11时21分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
/*
char head[]="HTTP/1.1 200 OK\r\n"\
	"Content-Type: text/html\r\n"\
	"\r\n";	
	
char err[]=	"HTTP/1.1 404 Not Found\r\n"		\
			"Content-Type: text/html\r\n"		\
			"\r\n"								\
			"<HTML><BODY>File not found</BODY></HTML>";
*/			
char head[]="HTTP/1.1 200 OK\r\n\
	Content-Type: text/html\r\n\
	\r\n";
char err[]=	"HTTP/1.1 404 Not Found\r\n				\
			Content-Type: text/html\r\n				\
			\r\n									\
			<HTML><BODY>File not found</BODY></HTML>";
typedef struct client
{
	int sockfd;
	char client_ip[16];//INET_ADDRSTRLEN
	short client_port;
}CLI;

void *client_deal(void * del)
{
	CLI client_del = *(CLI*)del;
	char recv_buf[1024] = "";
	char filename[512] = "";

	recv(client_del.sockfd, recv_buf, sizeof(recv_buf), 0);
	sscanf(recv_buf, "GET /%[^ ]", filename);
	printf("___filename == %s\n", filename);
	
/******************************************************************************	
*功能：用系统调用open()打开文件的数据
*******************************************************************************/		
	int fd = 0;
	fd = open(filename, O_RDONLY);
	if((fd = open(filename, O_RDONLY)) < 0)
	{
		perror("open");
		send(client_del.sockfd, err, strlen(err), 0);
		close(client_del.sockfd);
		return NULL;
	}
/******************************************************************************	
*功能：用“文件流”fopen()打开文件
*******************************************************************************/		
/*	
	FILE *fp;
	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		perror("fopen");
		send(client_del.sockfd, err, strlen(err), 0);
//			printf("err == %s\n", err);
		close(client_del.sockfd);
		return NULL;
	}
*/
/******************************************************************************	
*功能：如果网页文件打开成功，则webserver服务器会在传送网页文件数据前给浏览器
*		（客户端）发送一条“请求成功”报文
*******************************************************************************/	
	printf("file exist!\n");
	send(client_del.sockfd, head, strlen(head), 0);
//	printf("[%s](%d):%s\n", client_del.client_ip, client_del.client_port, recv_buf);
//	printf("head == %s\n", head);
/******************************************************************************	
*功能：通过read()循环读取文件内容并发送给客户端
*******************************************************************************/
	char buf[1024] = "";
	int len = 0;
	while( (len = read(fd, buf, sizeof(buf))) > 0)
	{
		send(client_del.sockfd, buf, len, 0);
	}
	close(fd);
/******************************************************************************	
*功能：通过fgetc(),逐个“字符”读取文件，然后逐个发送给客户端
*******************************************************************************/	
/*	
	FILE *fp;
	char ch;		
	while((ch = fgetc(fp)) != EOF)
	{
		printf("--%c--", ch);
		int lenc = send(client_del.sockfd, &ch, sizeof(ch), 0);
	//	int lenc = write(client_del.sockfd, ch, sizeof(ch));
		if(lenc <= 0)
		{	
			printf("***********\n");	//判断是否发送到客户端（即浏览器）
		}
	}
	printf("\n");
	fclose(fp);
*/
/******************************************************************************	
*功能：通过fgets(),逐个“字符串”读取文件，然后逐个发送给客户端
*******************************************************************************/
/*	
	FILE *fp;
	char buf[1024*1024] = "";
	while(1)
	{
		printf("before fgets!\n");
		fgets(buf, sizeof(buf), fp);
		printf("buf = %s", buf);
		int lens = send(client_del.sockfd, buf, strlen(buf), 0);
		//int lens = write(client_del.sockfd, buf, sizeof(buf));
		if(lens <= 0)
		{
			printf("**************\n");
		}
		if(strlen(buf) == 0)
			break;
		bzero(buf, sizeof(buf));
	}
	fclose(fp);
*/
/******************************************************************************	
*功能：通过fread(),逐个“数据块”读取文件，然后逐个发送给客户端
*******************************************************************************/
/*	
	FILE *fp;
	char buf[1024] = "";
	static int i = 0;
	while(1)
	{
		int lenf = fread(buf, sizeof(buf), 1, fp);
		int lens = send(client_del.sockfd, buf, sizeof(buf), 0);
		fflush(fp);
		fflush(stdout);
		printf("\n");
		//write(client_del.sockfd, buf, sizeof(buf));
		if(lens <= 0)
		{
			printf("**************\n");
		}
		
		i++;
		printf("___线程循环次数__%d\n", i);
		printf("%s",buf);
		if(lenf == 0)
			break;
		bzero(buf, sizeof(buf));
	}
	i = 0;
	fclose(fp);
*/
	close(client_del.sockfd);
	return NULL;
}

int main(int argc, char *argv[])
{
	unsigned short port = 8000;
	struct sockaddr_in my_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int  on = 1; 
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&on, sizeof(on) );
	int connfd = 0;
	if(argc > 1)
	{
		port = atoi(argv[1]);
	}
	
	if(sockfd < 0)
	{
		perror("socket");
		exit(-1);
	}

	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr)) != 0)
	{
		perror("bind");
		close(sockfd);
		exit(-1);
	}
	
	if(listen(sockfd, 10) != 0)
	{
		perror("listen");
		close(sockfd);
		exit(-1);
	}
	
	struct sockaddr_in client_addr;
	bzero(&client_addr, sizeof(client_addr));
	socklen_t addr_len = sizeof(client_addr);
	
	CLI client_mes;
	pthread_t tid;
	static int j = 0;
	while(1)
	{
		printf("please input like this:“10.220.4.32:8000/html/index.html”\n");
		connfd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
		if(connfd < 0)
		{
			perror("accept");
			close(sockfd);
			exit(-1);
		}
		printf("accept success!\n");
		
		client_mes.sockfd = connfd;
		inet_ntop(AF_INET, &client_addr.sin_addr, client_mes.client_ip, 16);
		client_mes.client_port = ntohs(client_addr.sin_port);
		
		pthread_create(&tid, NULL, (void *)client_deal, (void *)&client_mes);
		pthread_detach(tid);
	//	pthread_join(tid);
		j++;
		printf("_____外层while循环次数____%d\n", j);
	}
	close(sockfd);
	return 0;
}


