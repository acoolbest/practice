
#ifndef _ARP_LINK_H_
#define _ARP_LINK_H_
#include "main.h"
#include <string.h>

/******************************************************************
��	��:	ARP		���ݰ�ά������
*******************************************************************/
void link_creat_end(LIST **p_head,LIST *p_new);

/******************************************************************
��	��:	ͨ��ip	��������
*******************************************************************/
LIST *link_search(LIST *head, char *ip);

void link_print(LIST *head);
void link_free(LIST **p_head);



#endif



