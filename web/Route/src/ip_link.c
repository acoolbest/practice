
#include "ip_link.h"

void link_creat_ip(S_IP **p_head,S_IP *p_new)
{	
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
	printf("%s\n", p_new->ip);
}
void link_creat_mac(S_MAC **p_head,S_MAC*p_new)
{	
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
	printf("%s\n", p_new->mac);
}

void link_creat_udp(S_UDP **p_head,S_UDP *p_new)
{	
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
	printf("%d\n", p_new->udp_port);
}


void link_creat_tcp(S_TCP **p_head,S_TCP *p_new)
{	
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
	printf("%d\n", p_new->tcp_port);
}

void link_creat_key(S_KEY **p_head,S_KEY *p_new)
{	
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
	printf("%s\n", p_new->keyword);
}

void link_creat_type(S_TYPE **p_head,S_TYPE*p_new)
{	
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
	printf("%d\n", p_new->type);
}
void link_free_ip(S_IP **p_head)
{
	S_IP *pb=*p_head;
	while(pb!=NULL)
	{
		pb=(*p_head)->next;
		free(*p_head);//注意，应该是free(*p_head)
		*p_head=pb;
	}
}

void link_free_mac(S_MAC **p_head)
{
	S_MAC *pb=*p_head;
	while(pb!=NULL)
	{
		pb=(*p_head)->next;
		free(*p_head);//注意，应该是free(*p_head)
		*p_head=pb;
	}
}


void link_free_udp(S_UDP **p_head)
{
	S_UDP *pb=*p_head;
	while(pb!=NULL)
	{
		pb=(*p_head)->next;
		free(*p_head);//注意，应该是free(*p_head)
		*p_head=pb;
	}
}


void link_free_tcp(S_TCP **p_head)
{
	S_TCP *pb=*p_head;
	while(pb!=NULL)
	{
		pb=(*p_head)->next;
		free(*p_head);//注意，应该是free(*p_head)
		*p_head=pb;
	}
}



void link_free_key(S_KEY **p_head)
{
	S_KEY *pb=*p_head;
	while(pb!=NULL)
	{
		pb=(*p_head)->next;
		free(*p_head);//注意，应该是free(*p_head)
		*p_head=pb;
	}
}
void link_free_type(S_TYPE **p_head)
{
	S_TYPE *pb=*p_head;
	while(pb!=NULL)
	{
		pb=(*p_head)->next;
		free(*p_head);//注意，应该是free(*p_head)
		*p_head=pb;
	}
}

S_IP *link_search_ip(S_IP *head, char *ip)
{	
	S_IP *pb=head;
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


S_MAC *link_search_mac(S_MAC *head, char *mac)
{
	S_MAC *pb=head;
	while(pb!=NULL)	
	{
		if(strcmp(pb->mac,mac)==0)				
			return pb;				
		else
			pb=pb->next;
	}	
	return NULL;
}
S_UDP *link_search_udp(S_UDP *head, unsigned short udp_port)
{
	S_UDP *pb=head;
	while(pb!=NULL) 
	{
		if(pb->udp_port == udp_port)				
			return pb;				
		else
			pb=pb->next;
	}	
	return NULL;

}

S_TCP *link_search_tcp(S_TCP *head, unsigned short tcp_port)
{
	S_TCP *pb=head;
	while(pb!=NULL) 
	{
		if(pb->tcp_port == tcp_port)				
			return pb;				
		else
			pb=pb->next;
	}	
	return NULL;
}


S_KEY *link_search_key(S_KEY *head, unsigned char *keyword)
{
	S_KEY *pb=head;
	while(pb!=NULL) 
	{
		if(strstr(pb->keyword,keyword))				
			return pb;				
		else
			pb=pb->next;
	}	
	return NULL;

}
S_TYPE *link_search_type(S_TYPE *head, int ip_type)
{
	S_TYPE *pb=head;
	while(pb!=NULL) 
	{
		if(pb->type == ip_type)
			return pb;				
		else
			pb=pb->next;
	}	
	return NULL;
}
void link_save_ip(S_IP *head, FILE *fp)
{	
	char buf[10] = "ip:\r\n";
	fwrite(buf, 5, 1 ,fp);
	char buf2[10] = "\r\n";
	S_IP *p_mov=head;	
	if(head==NULL)		
		printf("链表为空！\n"); 
	else	
	{		
		while(p_mov!=NULL)
		{	
			fwrite(p_mov->ip, strlen(p_mov->ip), 1 ,fp);
			fwrite(buf2, 2, 1 ,fp);
			p_mov=p_mov->next;		
		}	
	}
}
void link_save_mac(S_MAC *head, FILE *fp)
{	
	char buf[10] = "mac:\r\n";
	fwrite(buf, 6, 1 ,fp);
	char buf2[10] = "\r\n";
	S_MAC *p_mov=head;	
	if(head==NULL)		
		printf("链表为空！\n"); 
	else	
	{		
		while(p_mov!=NULL)
		{	
			fwrite(p_mov->mac, strlen(p_mov->mac), 1 ,fp);
			fwrite(buf2, 2, 1 ,fp);
			p_mov=p_mov->next;		
		}	
	}
}
void link_save_tcp(S_TCP *head, FILE *fp)
{	
	char buf[20] = "tcp_port:\r\n";
	fwrite(buf, 11, 1 ,fp);
	char buf2[10] = "\r\n";
	S_TCP *p_mov=head;
	char buf3[20] ="";
	if(head==NULL)		
		printf("链表为空！\n"); 
	else	
	{		
		while(p_mov!=NULL)
		{	
			sprintf(buf3, "%d", p_mov->tcp_port);
			fwrite(buf3, strlen(buf3), 1 ,fp);
			fwrite(buf2, 2, 1 ,fp);
			p_mov=p_mov->next;		
		}	
	}
}

void link_save_udp(S_UDP *head, FILE *fp)
{	
	char buf[20] = "udp_port:\r\n";
	fwrite(buf, 11, 1 ,fp);
	char buf2[10] = "\r\n";
	S_UDP *p_mov=head;	
	char buf3[20] = "";
	if(head==NULL)		
		printf("链表为空！\n"); 
	else	
	{		
		while(p_mov!=NULL)
		{	
			sprintf(buf3, "%d", p_mov->udp_port);
			fwrite(buf3, strlen(buf3), 1 ,fp);
			fwrite(buf2, 2, 1 ,fp);
			p_mov=p_mov->next;		
		}	
	}
}
void link_save_key(S_KEY *head, FILE *fp)
{	
	char buf[10] = "keyword:\r\n";
	fwrite(buf, 10, 1 ,fp);
	char buf2[10] = "\r\n";
	S_KEY *p_mov=head;	
	if(head==NULL)		
		printf("链表为空！\n"); 
	else	
	{		
		while(p_mov!=NULL)
		{	
			fwrite(p_mov->keyword, strlen(p_mov->keyword), 1 ,fp);
			fwrite(buf2, 2, 1 ,fp);
			p_mov=p_mov->next;		
		}	
	}
}
void link_save_type(S_TYPE*head, FILE *fp)
{	
	char buf[10] = "type:\r\n";
	fwrite(buf, 7, 1 ,fp);
	char buf2[10] = "\r\n";
	S_TYPE *p_mov=head;	
	char buf3[20] = "";
	if(head==NULL)		
		printf("链表为空！\n"); 
	else	
	{		
		while(p_mov!=NULL)
		{	
			sprintf(buf3, "%d", p_mov->type);
			fwrite(buf3, strlen(buf3), 1 ,fp);
			fwrite(buf2, 2, 1 ,fp);
			p_mov=p_mov->next;		
		}	
	}
}



//从配置文件中读取数据
char * read_src_file(unsigned long int *file_length,char *src_file_name)
{
	char *src_file_buf=NULL;
	int fd = open(src_file_name, O_RDWR | O_CREAT, 0777);	//防止配置文件被删除出错
	close(fd);
	FILE *fp;
	fp=fopen(src_file_name,"rb");
	if(fp==NULL)
	{
		printf("open %s faild\n",src_file_name);
		return NULL;
	}
	fseek(fp,0,SEEK_END);
	(*file_length)=ftell(fp);
	rewind(fp);
	src_file_buf=(char *)malloc((*file_length)+1);
	fread(src_file_buf,(*file_length)+1,1,fp);
	src_file_buf[*file_length] = '\0';

	fclose(fp);// 关闭文件
	return src_file_buf ;
}

int ipconfig_strtok(char *msg_src, char *msg_done[])
{
	char str[]="\r\n";
	int num=0;
	msg_done[num]=strtok(msg_src,str);
	while(msg_done[num]!=NULL)
	{
		num++;
		msg_done[num]=strtok(NULL,str);
	}
	return num;
}

void read_ipconfig(void)
{
	char *pf;
	char *msg_done[1024];
	unsigned long int file_length=0;	//保存 源文件的长度 
	pf=read_src_file(&file_length, "ip_config");
	int line=ipconfig_strtok(pf,msg_done);
	int i = 0, mac_line = 0, tcp_port = 0, udp_port = 0, keyword = 0, type = 0;
	for(i = 1; i< line; i++)
	{
		if(strstr(msg_done[i], "mac"))
		{
			mac_line = i;
		}
		if(strstr(msg_done[i], "tcp_port"))
		{
			tcp_port = i;
		}
		if(strstr(msg_done[i], "udp_port"))
		{
			udp_port = i;
		}
		if(strstr(msg_done[i], "keyword"))
		{
			keyword = i;
		}
		if(strstr(msg_done[i], "type"))
		{
			type = i;
			break;
		}
	}
	
	S_IP *s_ip_new;
	S_MAC *s_mac_new;
	S_TCP *s_tcp_new;
	S_UDP *s_udp_new;
	S_KEY *s_key_new;
	S_TYPE *s_type_new;
	
	printf("---------ip_shield_link--------\n");
	for(i = 1; i < mac_line; i++)
	{
		s_ip_new=(S_IP*)malloc(sizeof(S_IP));
		strcpy(s_ip_new->ip, msg_done[i]);
		link_creat_ip(&s_ip_head, s_ip_new);
	}
	printf("--------mac_shield_link--------\n");
	for(i = mac_line+1; i < tcp_port; i++)
	{
		s_mac_new=(S_MAC*)malloc(sizeof(S_MAC));
		strcpy(s_mac_new->mac, msg_done[i]);
		link_creat_mac(&s_mac_head, s_mac_new);
	}
	printf("--------tcp_shield_link--------\n");
	for(i = tcp_port+1; i< udp_port; i++)
	{
		s_tcp_new=(S_TCP*)malloc(sizeof(S_TCP));
		s_tcp_new->tcp_port = atoi(msg_done[i]);
		link_creat_tcp(&s_tcp_head, s_tcp_new);
	}
	printf("--------udp_shield_link--------\n");
	for(i = udp_port+1; i< keyword; i++)
	{
		s_udp_new=(S_UDP*)malloc(sizeof(S_UDP));
		s_udp_new->udp_port = atoi(msg_done[i]);
		link_creat_udp(&s_udp_head, s_udp_new);
	}
	printf("------keyword_shield_link------\n");
	for(i = keyword+1; i< type; i++)
	{
		s_key_new=(S_KEY*)malloc(sizeof(S_KEY));
		strcpy(s_key_new->keyword, msg_done[i]);
		link_creat_key(&s_key_head, s_key_new);
	}
	printf("-------type_shield_link------\n");
	for(i = type+1; i< line; i++)
	{
		s_type_new=(S_TYPE*)malloc(sizeof(S_TYPE));
		s_type_new->type = atoi(msg_done[i]);
		link_creat_type(&s_type_head, s_type_new);
	}
}

