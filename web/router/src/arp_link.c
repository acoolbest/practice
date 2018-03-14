
#include "arp_link.h"


void link_creat_end(LIST **p_head,LIST *p_new)
{	
	if(*p_head==NULL)
	{		
		*p_head=p_new;
		p_new->next=NULL;
	}	
	else
	{	
		if(link_search(head,p_new->ip) == NULL)
		{
			p_new->next=*p_head;
			*p_head=p_new;	
		}
	}
}
LIST *link_search(LIST *head, unsigned char *ip)
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


