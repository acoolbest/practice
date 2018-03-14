
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <pthread.h>
char send_buf[] = "I've received your message!";
typedef struct client
{
	int sockfd;
	char client_ip[64];
	unsigned short client_port;
}CLI;
CLI client_mes;

void *client_deal(void * del)
{
	//CLI client_del = *(CLI*)del;
	char recv_buf[1024] = "";
	
	while(recv(client_mes.sockfd, recv_buf, sizeof(recv_buf), 0) != 0)
	{
		printf("[%s](%d):%s\n", client_mes.client_ip, client_mes.client_port, recv_buf);
		bzero(recv_buf, sizeof(recv_buf));
		send(client_mes.sockfd, send_buf, strlen(send_buf), 0);
	}
	
	close(client_mes.sockfd);
	return NULL;
}
int main(int argc, char *argv[])
{
	unsigned short port = 8000;
	
	if(argc > 1)
		port = atoi(argv[1]);
	
	int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		exit(-1);
	}
	struct sockaddr_in6 local_addr;
	struct sockaddr_in6 client_addr;
	socklen_t addr_len = sizeof(client_addr);
	bzero(&local_addr, sizeof(local_addr));
	bzero(&client_addr, sizeof(client_addr));
	local_addr.sin6_family = AF_INET6;
	local_addr.sin6_port = htons(port);
	local_addr.sin6_addr = in6addr_any; //Í¨ÅäµØÖ·
	
	int  on = 1; 
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&on, sizeof(on) );
	
	if(0 != bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)))
	{
		perror("bind");
		close(sockfd);
		exit(-1);
	}
	
	if(0 != listen(sockfd, 10))
	{
		perror("listen");
		close(sockfd);
		exit(-1);
	}
	int connfd = 0;
	pthread_t tid;
	
	while(1)
	{
		connfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
		if(connfd < 0)
		{
			perror("accept");
			close(sockfd);
			exit(-1);
		}
		printf("accept success!\n");
		client_mes.sockfd = connfd;
		inet_ntop(AF_INET6, &client_addr.sin6_addr, client_mes.client_ip, 64);
		client_mes.client_port = ntohs(client_addr.sin6_port);
		pthread_create(&tid, NULL, (void *)client_deal, NULL);
		//(void *)&client_mes);
		pthread_detach(tid);
	}
	close(sockfd);
	return 0;
}