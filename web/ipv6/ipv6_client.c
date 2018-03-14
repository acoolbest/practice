
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
int main(int argc, char *argv[])
{
//	char server_ip[] = "fe80::20c:29ff:fe4d:a02/64";
	char server_ip[] = "2001:250:4005::9:6/64";
	int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		exit(-1);
	}
	
	struct sockaddr_in6 server_addr;
	struct sockaddr_in6 client_addr;

	bzero(&server_addr, sizeof(server_addr));
	bzero(&client_addr, sizeof(client_addr));
	server_addr.sin6_family = AF_INET6;
	server_addr.sin6_port = htons(8000);
	inet_pton(AF_INET6, server_ip, &server_addr.sin6_addr);
	
	if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
	{
		perror("connect");
		close(sockfd);
		exit(-1);
	}
	char buf[1024] = "";
	while(1)
	{
		printf("send:");
		fflush(stdout);
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = '\0';
		send(sockfd, buf, strlen(buf), 0);
		bzero(buf, sizeof(buf));
		if(recv(sockfd, buf, sizeof(buf), 0) > 0)
			printf("recvmessage:%s\n", buf);
		else
		{
			printf("recv error!");
			close(sockfd);
			exit(-1);
		}
	}
	close(sockfd);
	return 0;
}