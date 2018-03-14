#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define			MAX_BUFF_LEN						260

typedef struct _SINGLE_PORT_BASE_INFO_STRUCT_
{
	char board_num[MAX_BUFF_LEN];
	uint8_t dst_addr;
	char qr_code[MAX_BUFF_LEN];
	uint8_t enable_port;
}single_port_base_info_stru, *p_single_port_base_info_stru;

typedef struct _msg
{
	long mtype;
	single_port_base_info_stru mtext;
}MSG;

int main(int argc, char * argv[])
{
	int ret = -1;
	key_t key = 0x20170630;
	int msgqid;
	MSG msg;
	long mtype = 1;
	//key = ftok(".", 2012);
	#if 1
	msgqid = msgget(key, 0);
	if(msgqid>0)
	{
		msgctl(msgqid, IPC_RMID, NULL);	//从系统内核中移走消息队列。
		printf("ipcrm -Q 0x20170630\n");
	}
	#endif
	msgqid = msgget(key, IPC_CREAT|0666);
	if(msgqid == -1)
	{
		printf("msgget error:%s\n", strerror(errno));
		perror("msgget");
		exit(-1);
	}

	printf("msgqid = %d\n", msgqid);
	while(1)
	{
		msg.mtype = mtype++;
		strcpy(msg.mtext.qr_code, "12345678");
		ret = msgsnd(msgqid, &msg, sizeof(msg.mtext), 0);
		if(!ret)
			printf("send success! [hello world], ret is %d\n", ret);
		else
			printf("send failed! [hello world], ret is %d\n", ret);
	}
	
	return 0;
}