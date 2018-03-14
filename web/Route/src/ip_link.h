

#ifndef _IP_LINK_H_
#define _IP_LINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "main.h"
#include <string.h>
#include <unistd.h>

/******************************************************************
功	能:	读取过滤配置文档信息入链表
*******************************************************************/
void read_ipconfig(void);

/******************************************************************
功	能:	创建过滤链表
*******************************************************************/
void link_creat_ip(S_IP **p_head,S_IP *p_new);
void link_creat_mac(S_MAC **p_head,S_MAC*p_new);
void link_creat_tcp(S_TCP **p_head,S_TCP *p_new);
void link_creat_udp(S_UDP **p_head,S_UDP *p_new);
void link_creat_key(S_KEY **p_head,S_KEY *p_new);
void link_creat_type(S_TYPE **p_head,S_TYPE*p_new);

/******************************************************************
功	能:	从配置文件读取数据
*******************************************************************/
char * read_src_file(unsigned long int *file_length,char *src_file_name);

/******************************************************************
功	能:	按\r\n	切割数据
*******************************************************************/
int ipconfig_strtok(char *msg_src, char *msg_done[]);

/******************************************************************
功	能:	查询链表
*******************************************************************/
S_IP *link_search_ip(S_IP *head, char *ip);
S_MAC *link_search_mac(S_MAC *head, char *mac);
S_TCP *link_search_tcp(S_TCP *head, unsigned short tcp_port);
S_UDP *link_search_udp(S_UDP *head, unsigned short udp_port);
S_KEY *link_search_key(S_KEY *head, unsigned char *keyword);
S_TYPE *link_search_type(S_TYPE *head, int ip_type);


/******************************************************************
功	能:	释放链表
*******************************************************************/
void link_free_ip(S_IP **p_head);
void link_free_mac(S_MAC **p_head);
void link_free_tcp(S_TCP **p_head);
void link_free_udp(S_UDP **p_head);
void link_free_key(S_KEY **p_head);
void link_free_type(S_TYPE **p_head);
/******************************************************************
功	能:	保存链表
*******************************************************************/

void link_save_ip(S_IP *head, FILE *fp);
void link_save_mac(S_MAC *head, FILE *fp);
void link_save_tcp(S_TCP *head, FILE *fp);
void link_save_udp(S_UDP *head, FILE *fp);
void link_save_key(S_KEY *head, FILE *fp);
void link_save_type(S_TYPE*head, FILE *fp);






#endif


