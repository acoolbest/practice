#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <signal.h>
#include <iostream>
bool rev_exit_flag;
using namespace std;
class socket_helper{
	public:
		socket_helper(uint16_t port_server, uint16_t port_mobile_phone);
		~socket_helper();
		
		bool init_exit_signal();
		void deal_adb_cmd();
		int init_socket();
		bool connect_socket();
		void deal_socket_rw();
	private:
		int sockfd;
		struct sockaddr_in server_addr;
		uint16_t port_ser;
		uint16_t port_mp;
		
		string server_ip;
};

socket_helper::socket_helper(uint16_t port_server, uint16_t port_mobile_phone)
{
	port_ser = port_server;
	port_mp = port_mobile_phone;
	rev_exit_flag = false;
	server_ip = "127.0.0.1";
}

socket_helper::~socket_helper(){
	port_ser = 0;
	port_mp = 0;
	close(sockfd);
}

void socket_helper::deal_adb_cmd()
{
	char adb_cmd[3][200] = {
		//"/root/charge/application/adb_tool -s YTDIBISGSK4HDMZP shell am broadcast -a NotifyServiceStop >> /dev/null",
		"/root/charge/application/adb_tool -s usb:3-1.2 forward tcp:",
		"/root/charge/application/adb_tool -s usb:3-1.2 shell am broadcast -a NotifyServiceStart >> /dev/null"
	};
	sprintf(adb_cmd[0], "%s%d tcp:%d", adb_cmd[0],port_ser,port_mp);
	
	for(int i=0; i<2; i++) 
	{
		cout << adb_cmd[i] << endl;
		system(adb_cmd[i]) ;
	}
}

void rev_exit_signal(int signum)
{
	printf("receive signal of exit is %d\n",signum);
	rev_exit_flag = true;
}

bool init_exit_signal()
{
	if(signal(SIGPIPE,rev_exit_signal)<0)
	{
		return false;
	}
	return true;
}

int socket_helper::init_socket()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);// 创建通信端点：套接字
	if(sockfd < 0)
	{
		perror("socket");
		cout << "init socket fail"<< endl;
		return sockfd;
	}
	bzero(&server_addr,sizeof(server_addr)); // 初始化服务器地址
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_ser);
	inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);
	cout << "init socket success"<< endl;
	return sockfd;
}

bool socket_helper::connect_socket()
{
	int err_log = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));      // 主动连接服务器
	if(err_log != 0)
	{
		perror("connect");
		close(sockfd);
		sleep(1);
		return false;
	}
	cout << "connect socket success" << endl;
	return true;
}

void socket_helper::deal_socket_rw()
{
	char send_buf[512] = "";
	cout << "send data to "<< server_ip << ":" << port_ser << endl;
	char recv_buf[2048] = "";
	
	while(1)
	{
		if(rev_exit_flag)
		{
			printf("exit send\n");
			rev_exit_flag = false;
			close(sockfd);
			sleep(1);
			break;
		}
		printf("send:");
		fgets(send_buf,sizeof(send_buf),stdin);
		send_buf[strlen(send_buf)-1]='\0';
		printf("******%s*******\n", send_buf);
		int retS = send(sockfd, send_buf, strlen(send_buf), 0);   // 向服务器发送信息
		printf("\nSendret:%d\n", retS);
		if(!retS) continue;
		int ret = recv(sockfd, recv_buf, 2048, 0);
		if(ret > 0)
		{
			printf("\nrecv data:\n");
			printf("%s\n",recv_buf);
		}
		else
		{
			perror("recv");
			printf("\nrecv ERROR!, %s, %d, %d\n", recv_buf, ret, ntohl(ret));
		}
		memset(recv_buf, 0, sizeof(recv_buf));
	}
}
#if 1
int main(int argc, char *argv[])
{
	if(init_exit_signal() == false) 
		return 1;
	
	socket_helper my_socket(atoi(argv[1]), atoi(argv[2]));
	
	my_socket.deal_adb_cmd();
	
	while(1)
	{
		if(my_socket.init_socket() < 0)
			return 1;
		if(my_socket.connect_socket() == false)
			continue;
		my_socket.deal_socket_rw();
	}
	return 0;
}
#endif
#if 0
static const char cmd[2][100] = {
"adb forward tcp:12580 tcp:10086",
"adb shell am broadcast -a NotifyServiceStart"
};

void rev_exit_signal(int signum)
{
	printf("receive signal of exit is %d\n",signum);
	rev_exit_flag = true;
}

bool init_exit_signal()
{
	if(signal(SIGPIPE,rev_exit_signal)<0)
	{
		return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	system(cmd[0]);
	system(cmd[1]);
	
	unsigned short port = 12580;        		// 服务器的端口号
	char *server_ip = "127.0.0.1";    	// 服务器ip地址

	if( argc > 1 )		//函数传参，可以更改服务器的ip地址									
	{		
		server_ip = argv[1];
	}	
	if( argc > 2 )	   //函数传参，可以更改服务器的端口号									
	{
		port = atoi(argv[2]);
	}
	
	init_exit_signal();
	
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);// 创建通信端点：套接字
	if(sockfd < 0)
	{
		perror("socket");
		exit(-1);
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr)); // 初始化服务器地址
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
	
	for(;;)
	{
		int err_log = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));      // 主动连接服务器
		if(err_log != 0)
		{
			perror("connect");
			//close(sockfd);
			//exit(-1);
			sleep(1);
			continue;
		}
		
		char send_buf[512] = {0x01};
		printf("send data to %s:%d\n",server_ip,port);
		char recv_buf[2048] = "";
		
		while(1)
		{
			if(rev_exit_flag)
			{
				printf("exit send\n");
				rev_exit_flag = false;
				//close(sockfd);
				break;
			}
			printf("send:");
			fgets(send_buf,sizeof(send_buf),stdin);
			send_buf[strlen(send_buf)-1]='\0';
			printf("******%s*******\n", send_buf);
			int retS = send(sockfd, send_buf, strlen(send_buf), 0);   // 向服务器发送信息
			printf("\nretS:%d\n", retS);
			if(!retS) continue;
			int ret = recv(sockfd, recv_buf, 2048, 0);
			if(ret > 0)
			{
				printf("\nrecv data:\n");
				printf("%s\n",recv_buf);
			}
			else
			{
				perror("recv");
				printf("\nrecv ERROR!, %s, %d, %d\n", recv_buf, ret, ntohl(ret));
			}
			memset(recv_buf, 0, sizeof(recv_buf));
		}
	}
	close(sockfd);
	return 0;
}
#endif