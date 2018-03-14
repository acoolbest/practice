

#include "key_pthread.h"
#include "get_interface.h"
#include "arp_link.h"
#include "main.h"
#include "ip_link.h"
#include "stftp.h"
static char *help = "=============================================\n\
0:Clear the screen\n\
1:Show all the configure of this system\n\
2:Show the local of arp table\n\
3:Set shield source ip\n\
4:Set shield dest mac\n\
5:Set shield source tcp port\n\
6:Set shield dest udp port\n\
7:Set shield keyword\n\
8:Set shield type\n\
9:restart router system\n\
10:up_down_file\n\
11:print help information\n\
12:exit system\n\
=============================================\n";



S_IP *s_ip_new;
S_MAC *s_mac_new;
S_TCP *s_tcp_new;
S_UDP *s_udp_new;
S_KEY *s_key_new;
S_TYPE *s_type_new;


void clear_screen(void)
{
	system("clear");
}
void show_local_arp(void)
{
	printf("\n-----------arp_link_start--------------\n\n");
	link_print(head);
	printf("\n------------arp_link_end---------------\n");
}
void write_config(void)
{
	FILE *fp;
	fp=fopen("ip_config","w");
	if(fp==NULL)
	{
		printf("open ip_config faild\n");
		return;
	}
	link_save_ip(s_ip_head, fp);
	link_save_mac(s_mac_head, fp);
	link_save_tcp(s_tcp_head, fp);
	link_save_udp(s_udp_head, fp);
	link_save_key(s_key_head, fp);
	link_save_type(s_type_head, fp);
	fclose(fp);// 关闭文件
}
void set_shield_ip(void)
{
	s_ip_new=(S_IP*)malloc(sizeof(S_IP));
	printf("please input shield ip, for example:10.220.4.3\n");
	getchar();
	char buf[20] = "";
	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf)-1] = '\0';
	strcpy(s_ip_new->ip, buf);
	link_creat_ip(&s_ip_head, s_ip_new);
	write_config();
	printf("success!\n");
}

void set_shield_mac(void)
{
	s_mac_new=(S_MAC*)malloc(sizeof(S_MAC));
	printf("please input shield mac, for example:aa:bb:cc:dd:ee:ff\n");
	getchar();
	char buf[30] = "";
	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf)-1] = '\0';
	strcpy(s_mac_new->mac, buf);
	link_creat_mac(&s_mac_head, s_mac_new);
	write_config();
	printf("success!\n");
}

void set_shield_tcp_port(void)
{
	printf("please input shield tcp_port, for example:1234\n");
	getchar();
	s_tcp_new=(S_TCP*)malloc(sizeof(S_TCP));
	char buf[20] = "";
	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf)-1] = '\0';
	s_tcp_new->tcp_port = atoi(buf);
	link_creat_tcp(&s_tcp_head, s_tcp_new);
	write_config();
	printf("success!\n");
}
void set_shield_udp_port(void)
{
	printf("please input shield udp_port, for example:1234\n");
	getchar();
	s_udp_new=(S_UDP*)malloc(sizeof(S_UDP));
	scanf("%d", &s_udp_new->udp_port);
	link_creat_udp(&s_udp_head, s_udp_new);
	write_config();
	printf("success!\n");
}

void set_shield_keyword(void)
{
	printf("please input shield keyword, for example:aaa\n");
	getchar();
	s_key_new=(S_KEY*)malloc(sizeof(S_KEY));
	char buf[1024] = "";
	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf)-1] = '\0';
	strcpy(s_key_new->keyword, buf);
	link_creat_key(&s_key_head, s_key_new);
	write_config();
	printf("success!\n");
}
void set_shield_type(void)
{
	printf("please input shield tcp/udp, for example:17\n");
	getchar();
	s_type_new=(S_TYPE*)malloc(sizeof(S_TYPE));
	scanf("%d", &s_type_new->type);
	link_creat_type(&s_type_head, s_type_new);
	write_config();
	printf("success!\n");
}
void restart_router_system(void)
{
	//	save();
	printf("reatarting system...\n");
	pool_uninit(&pool);
	pthread_cancel(tid_recv);
	link_free(&head);
	link_free_ip(&s_ip_head);
	link_free_mac(&s_mac_head);
	link_free_tcp(&s_tcp_head);
	link_free_udp(&s_udp_head);
	link_free_key(&s_key_head);
	link_free_type(&s_type_head);
	close(sock_raw_fd);
	pthread_mutex_destroy(&mutex);
	flag = 1;
	sleep(2);
	printf("restart system success\n");
	
}

void help_print(void)
{
	printf("%s\n", help);
}

void exit_system(void)
{
	//保存配置，退出系统
	printf("saving configure...\n");
	sleep(2);
	printf("save sucess!\n");
	usleep(500*1000);
	printf("exit...\n");
	sleep(3);
	exit(0);
}

FUN fun[] = {		
	{0, clear_screen},
	{1, getinterface},
	{2, show_local_arp},
	{3, set_shield_ip}, 
	{4, set_shield_mac},
	{5, set_shield_tcp_port},
	{6, set_shield_udp_port},
	{7, set_shield_keyword},
	{8, set_shield_type},
	{9, restart_router_system},
	{10, up_down_tftp},
	{11, help_print},
	{12, exit_system}
};
void select_cmd(void)
{	
	int cmd = 0;
	help_print();
//	printf("Cmd:");
//	fflush(stdout);
	int iTmp = 0;	
	while(1)	
	{
		if(flag == 1)
		{
			break;
		}
		printf("Cmd:");
		fflush(stdout);
		scanf("%d", &cmd);		
		for(iTmp = 0; iTmp < sizeof(fun)/sizeof(FUN); iTmp++)
		{			
			if(cmd == fun[iTmp].cmd)
			{				
				fun[iTmp].fun();
			}
		}
		
	}
}
void *deal_key(void *arg)
{
	select_cmd();
	return NULL;
}

