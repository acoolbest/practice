/**
*	g++ bounded_blocking_queue.cpp -lpthread -g -Wall -std=c++11
**/
#include <thread>
#include <stdint.h>
#include <unistd.h>
#include <algorithm>
#include "bounded_blocking_queue.h"

static bool gb_push_runing = true;
static bool gb_pop_runing = true;


bounded_blocking_queue<client_recv_stru> q(1024*1024);

void foo1() 
{
	client_recv_stru stru_tmp;
	bool was_empty = false;
	while(gb_pop_runing || !was_empty)
	{
		sleep(3);
		if(q.pop(stru_tmp, was_empty, 10))
			cout << BOLDWHITE << get_current_time() << stru_tmp.recv_buf << stru_tmp.client_data << RESET << endl;
		else
			cout << BOLDWHITE << get_current_time() << "pop timeout ~~~~~~~~~" << RESET << endl;
		sleep(3);
	}
}

void foo2() 
{
  client_recv_stru stru_tmp;
  client_recv_stru stru_tmp1;
  uint32_t client_data = 0xFFFFFF;
  while(gb_push_runing)
  {
	client_data--;
	for(int i=0;i<2;i++)
	{
		stru_tmp.client_data = client_data;
		stru_tmp.recv_buf = "123, ";
		q.push(stru_tmp, 15);
	}
	sleep(1);
	stru_tmp.client_data++;
	stru_tmp1.client_data = stru_tmp.client_data;
	q.erase(stru_tmp1);
  }
  gb_pop_runing = false;
}

int main(int argc, char *argv[])
{
	std::thread first (foo1);
	std::thread second (foo2);
	sleep(30);
	gb_push_runing = false;
	first.join();
	second.join();
	return 0;
}