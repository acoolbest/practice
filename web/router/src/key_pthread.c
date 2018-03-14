

#include "key_pthread.h"
#include "get_interface.h"
static char *help = "=============================================\n\
0:Clear the screen\n\
1:Show all the configure of this system\n\
2:Show the local of arp table\n\
3:Set shield ip\n\
4:Set shield mac\n\
5:Set shield port\n\
6:Set shield keyword\n\
7:restart router system\n\
8:print help information\n\
=============================================\n";


void clear_screen(void)
{
	system("clear");
}
void show_local_arp(void)
{

}

void set_shield_ip(void)
{

}

void set_shield_mac(void)
{

}

void set_shield_port(void)
{

}

void set_shield_keyword(void)
{

}

void restart_router_system(void)
{

}

void help_print(void)
{
	printf("%s\n", help);
}

void exit_system(void)
{
	//保存配置，退出系统
	printf("saving configure...");
	printf("save sucess!");
	printf("exit...");
	exit(0);
}


FUN fun[] = {		
	{0, clear_screen},
	{1, getinterface},
	{2, show_local_arp},
	{3, set_shield_ip}, 
	{4, set_shield_mac},
	{5, set_shield_port},
	{6, set_shield_keyword},
	{7, restart_router_system},
	{8, help_print},
	{9, exit_system}
};
void select_cmd(void)
{	
	int cmd = 0;
	help_print();
	printf("Cmd:");
	fflush(stdout);
	int iTmp = 0;	
	while(1)	
	{		
		scanf("%d", &cmd);		
		for(iTmp = 0; iTmp < sizeof(fun)/sizeof(FUN); iTmp++)
		{			
			if(cmd == fun[iTmp].cmd)
			{				
				fun[iTmp].fun();
			}
		}
		printf("Cmd:");
		fflush(stdout);
	}
}
void *deal_key(void *arg)
{
	select_cmd();
	return NULL;
}

