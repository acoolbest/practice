

#ifndef _IP_LINK_H_
#define _IP_LINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "main.h"
#include <string.h>

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
/******************************************************************
功	能:	从配置文件读取数据
*******************************************************************/
char * read_src_file(unsigned long int *file_length,char *src_file_name);

/******************************************************************
功	能:	按\r\n	切割数据
*******************************************************************/
int ipconfig_strtok(char *msg_src, char *msg_done[]);




#endif


