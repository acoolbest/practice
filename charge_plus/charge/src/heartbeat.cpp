#include <unistd.h>
#include <pthread.h>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "thread_pool.h"
#include "log_mgr/src/libsendlog.h"
#include "public_func/src/json_read.h"
#include "heartbeat.h"

#define DATA_LENGTH		64
static char sz_pc_name[DATA_LENGTH] = "";
static char sz_release_version[DATA_LENGTH] = "";
static bool g_thread_running = true;
static bool	g_thread_inited = false;
static pthread_t g_pthread_pid;

static bool is_thread_running()
{
	return g_thread_running;
}

static void set_thread_running_status(bool b_running)
{
	g_thread_running = b_running;
}

static void* heartbeat_work(void *arg)
{	
	uint32_t ret = CRG_FAIL;
	net_traffic_stru cnfg;
	char net_new[DATA_LENGTH] = "0";
	char sz_config_path[MAX_PATH] = { 0 };
	#if 1
	if((ret = get_path_of_net_traffic_sh(sz_config_path, MAX_PATH)) == CRG_FOUND)
	{
		write_log("net_traffic path is %s", sz_config_path);
		system(sz_config_path);
	}
	#endif
	write_log("heartbeat thread%s%s", PRINT_POINT_STR, PRINT_START_STR);
	operator_thread_work_count(INCREASE_TYPE);
	do
	{
		#if 1
		if(read_net_traffic(cnfg) == CRG_SUCCESS)
		{
			memset(net_new, 0, DATA_LENGTH);
			strcpy(net_new, cnfg.net_new.c_str());
		}
		else
		{
			write_log("read net_traffic.json failed, set net_new is 0");
			strcpy(net_new, "0");
		}
		#endif
		if(crg_send_heartbeat_info_to_server(sz_pc_name, sz_release_version, net_new)==CRG_SUCCESS)
		{
			#if 1
			if(strcmp(net_new, "0") != 0)
			{
				cnfg.net_new = "0";//string ∏≥÷µŒ Ã‚ 
				write_net_traffic(cnfg);
				//write_log("write net_traffic.json success, now net_new is 0");
			}
			else
			{
				//write_log("read net_traffic.json success, but net_new is 0");
			}
			#endif
		}
		for(uint32_t i=0;i<300;i++)
		{
			if (!is_thread_running())
			{
				break;
			}
			if((i % 10 == 0)&&(ret==CRG_FOUND)) system(sz_config_path);
			sleep(1);
		}
	} while (is_thread_running());
	operator_thread_work_count(DECREASE_TYPE);
	write_log("heartbeat thread%s%s", PRINT_POINT_STR, PRINT_END_STR);
	return (void*)0;
}

static uint32_t create_heartbeat_thread()
{
	uint32_t ret = CRG_FAIL;
	
	int err = pthread_create(&g_pthread_pid, NULL, heartbeat_work, NULL);
	if(err != 0){
		write_log("create heartbeat thread, error: %s%s%s", strerror(err), PRINT_POINT_STR, PRINT_FAIL_STR);
	}else{
		ret = CRG_SUCCESS;
	}
	g_thread_inited = (ret == CRG_SUCCESS ? true : false);
	return ret;
}

uint32_t run_heartbeat_model()
{
	uint32_t ret = CRG_FAIL;
	write_log("run heartbeat model%s%s", PRINT_POINT_STR, PRINT_START_STR);
	char sz_net_traffic_path[MAX_PATH] = "";
	if(get_path_of_net_traffic(sz_net_traffic_path, MAX_PATH) != CRG_FOUND)
	{
		write_log("heartbeat read net_traffic.json file failed!");
		return ret;
	}
	user_config cnfg;
	if (read_config(cnfg) == CRG_SUCCESS && cnfg.terminal.size() > 0 && strcmp(cnfg.terminal.c_str(),"null") != 0 )
	{
		strcpy(sz_pc_name, cnfg.terminal.c_str());
		strcpy(sz_release_version, cnfg.release_version.c_str());
		ret = create_heartbeat_thread();
	}else{
		write_log("heartbeat read terminal name failed!");
	}
	
	if (ret == CRG_SUCCESS)
		write_log("run heartbeat model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	else
		write_log("run heartbeat model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);
	return ret;
}

void set_heartbeat_model_exit_status()
{
	set_thread_running_status(false);
}

void wait_heartbeat_model_exit()
{
	write_log("waiting heartbeat model stop work%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if(g_thread_inited) pthread_join(g_pthread_pid, NULL);
	g_thread_inited = false;
	write_log("waiting heartbeat model stop work%s%s", PRINT_POINT_STR, PRINT_END_STR);
}


