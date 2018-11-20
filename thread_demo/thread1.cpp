#include <pthread.h>
#include <unistd.h>
#include <iostream>

using namespace std;

void * func(void *arg)
{
	int i = 10;
	while(i--)
	{
		cout << i << endl;
		sleep(1);
	}
	return NULL;
}

int main()
{
	pthread_t pid;
	pthread_create(&pid, NULL, func, NULL);
	pthread_join(pid, NULL);
	cout << "join done!" << endl;
}
