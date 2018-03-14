#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#define			MAX_BUFF_LEN						260
#define			MAXEPOLLSIZE						10
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
	
	//key = ftok(".", 2012);
	msgqid = msgget(key, 0);
	if(msgqid == -1)
	{
		printf("msgget error:%s\n", strerror(errno));
		perror("msgget");
		printf("msgqid = %d\n", msgqid);
		exit(-1);
	}
	
	printf("msgqid = %d\n", msgqid);
	
	#if 0
	uint16_t rev_data_len = 0;
	int epollfd, nfds, n;
	struct epoll_event ev;
	struct epoll_event events[MAXEPOLLSIZE];
	epollfd = epoll_create(MAXEPOLLSIZE);
	ev.events = EPOLLIN;
    ev.data.fd = msgqid;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, msgqid, &ev) < 0) { 
        fprintf(stderr, "epoll set insertion error: fd=%d\n", msgqid); 
        return -1; 
	}// else printf("put msgqid into epoll listening OK!\n"); 
	while(1)
	{
		nfds = epoll_wait(epollfd, events, MAXEPOLLSIZE, 1000);
		if (nfds != -1) { 
			for (n = 0; n < nfds; ++n) {
				if(events[n].events & EPOLLIN){
					ioctl(msgqid, FIONREAD, &rev_data_len);//chage by zhzq @ 2016-7-23 13:39:39
					printf("rev_data_len is %d\n", rev_data_len);
					ret = msgrcv(msgqid, &msg, sizeof(msg.mtext), 0, 0);
					if(ret == sizeof(msg.mtext))
						printf("[success, ret is %d] msg.mtype = %ld, msg.mtext.qr_code = %s\n", ret, msg.mtype, msg.mtext.qr_code);
					else
						printf("[failed, ret is %d] msg.mtype = %ld, msg.mtext.qr_code = %s\n", ret, msg.mtype, msg.mtext.qr_code);
					getchar();
	            }
	        }
        }
	}
	#endif
	
	#if 1
	while(1)
	{
		ret = msgrcv(msgqid, &msg, sizeof(msg.mtext), 0, 0);
		if(ret == sizeof(msg.mtext))
			printf("[success, ret is %d] msg.mtype = %ld, msg.mtext.qr_code = %s\n", ret, msg.mtype, msg.mtext.qr_code);
		else
			printf("[failed, ret is %d] msg.mtype = %ld, msg.mtext.qr_code = %s\n", ret, msg.mtype, msg.mtext.qr_code);
		//getchar();
	}
	#endif
	
	msgctl(msgqid, IPC_RMID, NULL);
	return 0;
}