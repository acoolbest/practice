/* ************************************************************************
 *       Filename:  arp.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013年12月18日 19时22分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/

#include "arp.h"

LIST *head=NULL;
static int sock_raw_fd = 0;
void link_creat_head(LIST **p_head,LIST *p_new)
{
	LIST *p_mov=*p_head;
	if(*p_head==NULL)
	{
		*p_head=p_new;
		p_new->next=NULL;
	}
	else
	{
		while(p_mov->next!=NULL)
		{
			p_mov=p_mov->next;
		}
		p_mov->next=p_new;
		p_new->next=NULL;
	
	}
}
void link_creat_end(LIST **p_head,LIST *p_new)
{
//	LIST *p_mov=*p_head;
	if(*p_head==NULL)
	{
		*p_head=p_new;
		p_new->next=NULL;
	}
	else
	{
		p_new->next=*p_head;
		*p_head=p_new;	
	}
	
}
void link_print(LIST *head)
{
	LIST *p_mov=head;
	if(head==NULL)
		printf("link is empty!\n");
	else
	{
		while(p_mov!=NULL)
		{
			printf("IP: %s --- MAC: %s\n",p_mov->ip, p_mov->mac);
			p_mov=p_mov->next;
		}
	}
}
void link_save(LIST *head)
{
	LIST *p_mov=head;
	if(head==NULL)
		printf("link is empty!\n");
	else
	{
		int fd = open("ip_mac_list.txt", O_RDWR | O_CREAT, 0777);
		if(fd < 0)
		{
			perror("open");
			exit(-1);
		}
		while(p_mov!=NULL)
		{
			printf("IP: %s --- MAC: %s\n",p_mov->ip, p_mov->mac);
			write(fd, "IP: ", 4);
			write(fd, p_mov->ip, sizeof(p_mov->ip));
			write(fd, " --- MAC: ", strlen(" --- MAC: "));
			write(fd, p_mov->mac, sizeof(p_mov->mac));
			write(fd, "\n", 1);
			p_mov=p_mov->next;
		}
		close(fd);
	}
}
LIST *link_search(LIST *head,char *ip)
{
	LIST *pb=head;
	while(pb!=NULL)
	{
		static int i = 0;
		printf("----------------%d-------------\n",i++);
		if(strcmp(pb->ip,ip)==0)
			{
				printf("--------------------%s---------\n",pb->mac);
				return pb;
			}
		else
		pb=pb->next;
	}
	return NULL;
}
void *deal(void *arg)
{
	//5-----接受对方的ARP应答--------------------------------------
	unsigned char recv_msg[1024] = "";
	
	LIST *p_new=NULL;
	while(1)
	{	
		recvfrom(sock_raw_fd, recv_msg, sizeof(recv_msg), 0, NULL, NULL);
		if(recv_msg[21] == 2)					//ARP_answer
		{
			p_new=(LIST*)malloc(sizeof(LIST));
		//	char resp_mac[18] = "";				//answer_mac
		//	char resp_ip[16] = "";				//answer_ip
			sprintf(p_new->mac, "%02x:%02x:%02x:%02x:%02x:%02x",\
					recv_msg[22], recv_msg[23], recv_msg[24], recv_msg[25], recv_msg[26], recv_msg[27]);
			sprintf(p_new->ip, "%d.%d.%d.%d", recv_msg[28], recv_msg[29], recv_msg[30], recv_msg[31]);
			
			link_creat_end(&head,p_new);
			
/*			printf("IP: %d.%d.%d.%d ~ MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",\
					recv_msg[28], recv_msg[29], recv_msg[30], recv_msg[31],\
					recv_msg[22], recv_msg[23], recv_msg[24], recv_msg[25], recv_msg[26], recv_msg[27]);
*/
		}
		
	}
	return NULL;
}

void send_arp_ask(void)
{
	// 1-----创建通信用典原始套接字------------------------------
	sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock_raw_fd < 0)
	{
		perror("socket");
		exit(-1);
	}
	
	// 2-----根据各种协议首部格式构建发送数据报-------------------
	unsigned char send_msg[1024] = {
		//-------------combination mac------ 0 ~ 13 -----------
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	//det_mac: ff:ff:ff:ff:ff:ff
		0x20, 0x6a, 0x8a, 0x04, 0x8b, 0x04,	//src_mac: 00:0c:29:4d:0a:02
		0x08, 0x06,							//type: 0x0806 ARP argeement
		
		//-------------combination ARP------ 14 ~ 41 ----------
		0x00, 0x01, 0x08, 0x00,				//hardware_type: 1, agreement_type: 0x0800(IP)
		0x06, 0x04, 0x00, 0x01,				//hardware_length: 6, agreement_length: 4, option:(1:ARP_ask, 2:ARP_answer, 3:RARP_ask, 4:RARP_answer)
		0x20, 0x6a, 0x8a, 0x04, 0x8b, 0x04,	//send_src_mac: 00:0c:29:4d:0a:02
		192, 168, 1, 2,						//send_src_ip: 10.220.4.32
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//target_mac: c8:9c:dc:ba:aa:ca
		255, 255, 255, 255,						//target_ip: 10.220.4.31
	};
	
	// 3-----数据初始化---------------------------------------------
	struct sockaddr_ll sll;					//barbarism sockfd addr
	struct ifreq ethreq;					//network interface addr
	strncpy(ethreq.ifr_name, "enp4s0", IFNAMSIZ);//appointed name for network card
	
	// 4-----将网络接口赋值给原始套接字地址结构---------------------
	if(-1 == ioctl(sock_raw_fd, SIOCGIFINDEX, (char *)&ethreq))
	{
		perror("ioctl");
		close(sock_raw_fd);
		exit(-1);
	}
	bzero(&sll, sizeof(sll));
	sll.sll_ifindex = ethreq.ifr_ifindex;

	pthread_t tid;
	pthread_create(&tid, NULL, (void *)deal, NULL);
	pthread_detach(tid);

	int iTmp;
	for(iTmp = 1; iTmp < 255; iTmp++)
	{
		send_msg[41] = iTmp;
		sendto(sock_raw_fd, send_msg, 42, 0, (struct sockaddr *)&sll, sizeof(sll));
//		usleep(50*1000);
	}
	sleep(3);
	pthread_cancel(tid);
	link_print(head);
	link_save(head);
}


