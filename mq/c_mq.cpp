#include "CMsgQueue.h"  
#include <iostream>
using namespace std;

CMsgQueue::CMsgQueue()  
{
}  

CMsgQueue::~CMsgQueue()  
{
}  

/** 
 * 初始化消息队列 
 * type为0表示服务器 
 * type为1表示客户端 
 */  
//?????????????????????????????????????????????????  
int CMsgQueue::init(char* pathName, int type)  
{  
	key_t key = ftok(pathName, 1);  
	long ret;  
	if(-1 == key)  
	{  
		cout<<"key init error!"<<endl;  
		return -1;  
	}  
	else  
	{  
		if(0 == type)  
			ret = msgget(key, IPC_CREAT|0666);  
		else  
			ret = msgget(key, 0);  
	}  

	if(-1 == ret)  
	{  
		cout<<"msgget error!"<<endl;  
		return -1;  
	}  

	msgQueueID = ret;//消息队列ID  
	return 0;  
}  

long CMsgQueue::init(int key, int type)  
{  
	long ret;  

	if( 0 == type ) {  
		ret = msgget(key, IPC_CREAT|0666 );  
		if(ret == 0)  
		{ //0 is not ok?  
			msgctl(ret, IPC_RMID, NULL);//从系统内核中移走消息队列。  
			ret = msgget(key, IPC_CREAT|0666 );  
		}  
	}  
	else {  
		ret = msgget(key, 0 );  
	}  
	if(-1 == ret)  
	{  
		cout<<"msgget error!"<<endl;  
		return -1;  
	}  

	msgQueueID = ret;  

	return 0;  
}  

int CMsgQueue::receive(msg_t& message)
{  
	int ret = msgrcv(msgQueueID, &recvBuf, sizeof(msgbuf_t), 0, 0); 
	if(-1 == ret)  
	{  
		cout<<"msg recv error!"<<endl;  
		return -1;  
	}  
	memcpy(&message, &(recvBuf.msg), sizeof(msg_t));  
	return ret;  
}  

int CMsgQueue::send(const msg_t& message)
{  
	memcpy(&(sendBuf.msg), &message, sizeof(msg_t));  
	sendBuf.processID = msgQueueID;  
	int ret = msgsnd(msgQueueID, &sendBuf, sizeof(msgbuf_t), 0);
	if(-1 == ret)  
	{  
		cout<<"msg send error!"<<endl;  
		return -1;  
	}  
	return ret;  
}
