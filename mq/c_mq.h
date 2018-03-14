#ifndef _CMSGQUEUE_H_  
#define _CMSGQUEUE_H_  
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>  
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/msg.h>  

//��Ϣ�ṹ��  
typedef struct _msg {  
	int len;  
	char mText[1024];  
}msg_t;  

typedef struct _msgbuf_t  
{  
	int processID;  
	msg_t msg;  
} msgbuf_t;  


class CMsgQueue  
{  
	public:  
		CMsgQueue();  
		virtual ~CMsgQueue();  

		int init(char* pathName, int type);  
		long init(int key, int type);  
		int receive(msg_t& message);  
		int send(const msg_t& message);  

	private:  
		/** 
		 * ��Ϣ����ID 
		 */  
		long msgQueueID;  
		/** 
		 * ���ջ���ṹ�� 
		 */  
		msgbuf_t recvBuf;  
		/** 
		 * ���ͻ���ṹ�� 
		 */  
		msgbuf_t sendBuf;  
};  

#endif /* _CMSGQUEUE_H_ */  