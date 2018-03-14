/* ************************************************************************
 *       Filename:  timer.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年08月08日 21时33分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#define THREAD_COUNT	10
void cleanup(void *arg)
{
	printf("clean up ptr = %s\n", (char *)arg);
	free((char *)arg);
}

void * thread_proc(void *arg)
{
	int thread_num = *((int *)arg);
	printf("thread %d\n", thread_num);
	char *ptr = (char *)malloc(1000);
	pthread_cleanup_push(cleanup, (void*)(ptr));
	bzero(ptr, 1000);
	snprintf(ptr, 1000, "memory from malloc in thread %d", thread_num);
	struct timeval tv = {3,0};
	select(0, NULL, NULL, NULL, &tv);
	
	printf("before pop in thread %d\n", thread_num);
	pthread_cleanup_pop(1);
	return NULL;
}

int main(int argc, char *argv[])
{
	printf("process start\n");
	sleep(5);
	pthread_t tid[THREAD_COUNT];
	int i_while = 1;
	while(i_while<10)
	{
		for(int i=0;i<THREAD_COUNT;i++)
		{
			//sleep(5);
			pthread_create(&tid[i], NULL, thread_proc, (void *)&i);
			//pthread_detach(tid[i]);
			usleep(100*1000);
		}
		#if 1
		usleep(2500*1000);
		printf("cancel will start!\n");
		for(int i=THREAD_COUNT-1;i>=0;i--)
		{
			//sleep(10);
			printf("cancel %d\n", i);
			#if 1
			int kill_rc = pthread_kill(tid[i],0);
			if(kill_rc == ESRCH)
				printf("the specified thread did not exists or already quit\n");
			else if(kill_rc == EINVAL)
				printf("signal is invalid\n");
			else
			{
				printf("the specified thread is alive\n");
				pthread_cancel(tid[i]);
			}
			#else
			pthread_cancel(tid[i]);
			#endif
			pthread_join(tid[i],NULL);
			//usleep(50*1000);
		}
		#endif
		sleep(10);
		printf("process end %d\n", i_while++);
	}
	sleep(20);
	return 0;
}


