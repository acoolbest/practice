#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
int main(int argc, char *argv[])
{
	unsigned short port = 8000;        		// �������Ķ˿ں�
	char *server_ip = "127.0.0.1";    	// ������ip��ַ
	
	if( argc > 1 )		//�������Σ����Ը��ķ�������ip��ַ									
	{		
		server_ip = argv[1];
	}	
	if( argc > 2 )	   //�������Σ����Ը��ķ������Ķ˿ں�									
	{
		port = atoi(argv[2]);
	}

	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);// ����ͨ�Ŷ˵㣺�׽���
	if(sockfd < 0)
	{
		perror("socket");
		exit(-1);
	}
	
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr)); // ��ʼ����������ַ
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
	
	int err_log = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));      // �������ӷ�����
	if(err_log != 0)
	{
		perror("connect");
		close(sockfd);
		exit(-1);
	}
	
	char send_buf[512] = "";
	printf("send data to %s:%d\n",server_ip,port);
	while(1)
	{
		printf("send:");
		fgets(send_buf,sizeof(send_buf),stdin);
		send_buf[strlen(send_buf)-1]='\0';
		send(sockfd, send_buf, strlen(send_buf), 0);   // �������������Ϣ
	}
	
	close(sockfd);
	return 0;
}
