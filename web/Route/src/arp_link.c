
#include "arp_link.h"



void link_creat_end(LIST **p_head,LIST *p_new)
{
	pthread_mutex_lock(&mutex);
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
	pthread_mutex_unlock(&mutex);
}
LIST *link_search(LIST *head, char *ip)
{	
	LIST *pb=head;
//	static int i = 0;
	while(pb!=NULL)	
	{
		if(strcmp(pb->ip,ip)==0)
		{				
//			printf("----ip-match-mac--%d--%s-----\n", i++, pb->mac);
			return pb;			
		}		
		else
			pb=pb->next;
	}	
	return NULL;
}
void link_print(LIST *head)
{	
	LIST *p_mov=head;	
	if(head==NULL)		
		printf("链表为空！\n");	
	else	
	{		
		while(p_mov!=NULL)
		{			
			printf("IP: %s --- MAC: %s\n",p_mov->ip, p_mov->mac);
			p_mov=p_mov->next;		
		}	
	}
}
void link_free(LIST **p_head)
{
	LIST *pb=*p_head;
	while(pb!=NULL)
	{
		pb=(*p_head)->next;
		free(*p_head);//注意，应该是free(*p_head)
		*p_head=pb;
	}
}



