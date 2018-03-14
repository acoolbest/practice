
//#include "CTimer.h"

#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <iostream>
#include <map>

using namespace std;


struct my_timer_stru{
	pthread_t pthread_pid;
	string time_id;
	struct timeval select_time;
	string other_info;
};

static pthread_mutex_t									g_timer_info_mutex;
static map<string, my_timer_stru>						g_map_my_timer;

static void update_timer_info_to_map(my_timer_stru stru_my_timer)
{
	pthread_mutex_lock(&g_timer_info_mutex);
	g_map_my_timer[stru_my_timer.time_id] = stru_my_timer;
	pthread_mutex_unlock(&g_timer_info_mutex);
}

static bool is_timer_info_exist(string time_id)
{
	bool ret = false;
	pthread_mutex_lock(&g_timer_info_mutex);
	map<string, my_timer_stru>::iterator iter;
	iter = g_map_my_timer.find(time_id);
	if (iter != g_map_my_timer.end())
	{
		ret = true;
	}
	pthread_mutex_unlock(&g_timer_info_mutex);
	return ret;
}

static bool get_timer_info_from_map(string time_id, my_timer_stru & out_val)
{
	uint32_t ret = false;
	pthread_mutex_lock(&g_timer_info_mutex);
	map<string, my_timer_stru>::iterator iter;
	iter = g_map_my_timer.find(time_id);
	if (iter != g_map_my_timer.end())
	{
		out_val = iter->second;
		ret = true;
	}
	pthread_mutex_unlock(&g_timer_info_mutex);
	return ret;
}

static bool erase_time_info_from_map(string time_id)
{
	bool ret = false;
	pthread_mutex_lock(&g_timer_info_mutex);
	
	map<string, my_timer_stru>::iterator iter;
	iter = g_map_my_timer.find(time_id);
	if (iter != g_map_my_timer.end())
	{
		g_map_my_timer.erase(iter);
		map<string, my_timer_stru>(g_map_my_timer).swap(g_map_my_timer);
		ret = true;
	}
	pthread_mutex_unlock(&g_timer_info_mutex);
	return ret;
}

void kill_timer(string time_id)
{
	my_timer_stru my_timer = {0};
	if(get_timer_info_from_map(time_id, my_timer))
	{
		erase_time_info_from_map(time_id);
        pthread_cancel(my_timer.pthread_pid);
        pthread_join(my_timer.pthread_pid, NULL); 
	}	
}

static void * timer_thread(void * arg)
{
	my_timer_stru my_timer = *((my_timer_stru *)arg);
	delete (my_timer_stru *)arg;
	update_timer_info_to_map(my_timer);
	struct timeval tempval = {0};
	while (true){
		tempval = my_timer.select_time;
		select(0, NULL, NULL, NULL, &tempval);
		cout << my_timer.other_info << endl;
		pthread_testcancel();
	}
	return (void *)0;
}

bool set_timer(string time_id, struct timeval stru_time, string other_info)
{
	if(is_timer_info_exist(time_id))
	{
		kill_timer(time_id);
	}

	my_timer_stru * my_timer = new my_timer_stru;
	my_timer->time_id = time_id;
	my_timer->select_time = stru_time;
	my_timer->other_info = other_info;
	if(!pthread_create(&my_timer->pthread_pid, NULL, timer_thread, (void*)my_timer))
		return true;
	else
		return false;
}

int main()
{
	pthread_mutex_init(&g_timer_info_mutex, NULL);
	
	my_timer_stru my_timer = {
		.pthread_pid = 0,
		.time_id = string("12450035"),
		.select_time = {10,0},
		.other_info = string("test")
	};
	
	while(1)
	{
		cin >> my_timer.time_id;
		cin >> my_timer.other_info;
		cout << "set done " << my_timer.time_id << " " << my_timer.other_info << endl;
		if(my_timer.time_id == string("exit"))
			break;
		set_timer(my_timer.time_id, my_timer.select_time, my_timer.other_info);
		printf("press enter\n");
		sleep(5);
		kill_timer(my_timer.time_id);
		printf("kill done\n");
	}
	pthread_mutex_destroy(&g_timer_info_mutex);
	return 0;
}

#if 0
int main(int argc, char **argv)
{
	CTimer timer;
	uint32_t event_num = 0, fun = 0;
	pthread_mutex_init(&g_timer_info_mutex, NULL);
	while(1)
	{
		cout << "pls input [event_num] and [fun]" << endl;
		cin >> event_num;
		cin >> fun;
		cout << event_num << " , " << fun << endl;//定时器非常方便，定时器业务完成后可以销毁定时器
		if(fun == 0)
			timer.KillTimer(event_num);        //注销100号业务，现在只有50号业务在运行
		else
			timer.SetTimer(10,0,event_num);
	}
	pthread_mutex_destroy(&g_timer_info_mutex);
	return 0;
}
#endif




