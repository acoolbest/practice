
#ifndef _ARP_LINK_H_
#define _ARP_LINK_H_
#include "main.h"
#include <string.h>

/******************************************************************
功	能:	ARP		数据包维护链表
*******************************************************************/
void link_creat_end(LIST **p_head,LIST *p_new);

/******************************************************************
功	能:	通过ip	查找链表
*******************************************************************/
LIST *link_search(LIST *head, char *ip);

void link_print(LIST *head);
void link_free(LIST **p_head);



#endif



