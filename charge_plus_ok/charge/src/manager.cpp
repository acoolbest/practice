#include <string>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "device_data.h"
#include "thread_pool.h"
#include "ios_shm_mgr.h"
#include "android_shm_mgr.h"
#include "ios_mgr.h"
#include "android_mgr.h"
#include "listen_usb.h"
#include "manager.h"
#include "power_control.h"
#include "heartbeat.h"
#include "log_mgr/src/libsendlog.h"
#include "serial_mgr/src/com_transmission.h"
#include "log_mgr/src/log_mgr.h"



using std::vector;

#define		SIGNAL_EXIT									41
#define		SIGNAL_RESET_BOARD							42

#define		INIT_FUNC_COUNT								8
#define		RUN_FUNC_COUNT								5
#define		DESTORY_FUNC_COUNT							6

typedef		uint32_t (*LP_FUNC_START_ROUTINE)();

static uint32_t init_exit_signal();
static uint32_t init_reset_board_signal();

static LP_FUNC_START_ROUTINE init_func[INIT_FUNC_COUNT] = {init_exit_signal,
													init_reset_board_signal,
													crg_init_log_model_for_charge, 
													init_dev_data_model,
													init_power_model, 
													init_android_model, 
													init_ios_model,
													init_usb_model};
static LP_FUNC_START_ROUTINE run_func[RUN_FUNC_COUNT] = {run_power_model, 
													run_android_model, 
													run_ios_model,
													run_usb_model, 
													run_heartbeat_model};
static LP_FUNC_START_ROUTINE destory_func[DESTORY_FUNC_COUNT] = {destory_usb_model, 
														destory_android_model, 
														destory_ios_model,
														destory_power_model, 
														destory_dev_data_model,
														crg_destory_log_model};

static bool rev_signal_flag = false;

static void rev_signal(int signum)
{
	write_log("receive signal of exit is %d\n",signum);
	rev_signal_flag = true;
}

static uint32_t init_exit_signal()
{
	write_log("init exit signal model%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if(signal(SIGNAL_EXIT,rev_signal)<0)
	{
		write_log("init exit signal model, errno: %d%s%s", errno, PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}
	write_log("init exit signal model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	return CRG_SUCCESS;
}

static void rev_reset_board_signal(int signum)
{
	static bool flag = true;
	if(flag)
	{
		write_log("receive signal of reset board is %d\n",signum);
		flag = false;
		set_board_status(RESET_BOARD_FUNC);
		flag = true;
	}
	else
	{
		write_log("receive signal of reset board is %d, but now charge is busy, please Wait a moment!\n",signum);
	}
}

static uint32_t init_reset_board_signal()
{
	write_log("init reset_board signal model%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if(signal(SIGNAL_RESET_BOARD,rev_reset_board_signal)<0)
	{
		write_log("init reset_board signal model, errno: %d%s%s", errno, PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}
	write_log("init reset_board signal model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	return CRG_SUCCESS;
}


static void waiting_for_exit_signal()
{
	write_log("waiting for exit signal%s%s", PRINT_POINT_STR, PRINT_START_STR);
	while (1)
	{
		if (rev_signal_flag)
		{
			break;
		}
		sleep(5);
	}
	write_log("received EXIT signal%s%s", PRINT_POINT_STR, PRINT_OK_STR);
}

static void waiting_for_all_model_exit()
{
	set_usb_model_exit_status();
	wait_usb_model_exit();

	set_android_model_exit_status();
	set_ios_model_exit_status();
	wait_android_model_exit();
	wait_ios_model_exit();
	
	set_power_model_exit_status();
	set_heartbeat_model_exit_status();
	wait_power_model_exit();
	wait_heartbeat_model_exit();
}

uint32_t start_work()
{
	uint32_t i = 0;
	uint32_t ret = CRG_FAIL;
	write_log("charge process starting...\n");
	for (i = 0; i < INIT_FUNC_COUNT; i++)
	{
		if (init_func[i]() != CRG_SUCCESS)
		{
			goto _START_WORK_ERROR_;
		}
	}
	write_log("init all model%s%s", PRINT_POINT_STR, PRINT_OK_STR);

	for (i = 0; i < RUN_FUNC_COUNT; i++)
	{
		if (run_func[i]() != CRG_SUCCESS)
		{
			goto _START_WORK_ERROR_;
		}
	}
	write_log("run all model%s%s", PRINT_POINT_STR, PRINT_OK_STR);

	waiting_for_exit_signal();
	ret = CRG_SUCCESS;

_START_WORK_ERROR_:

	waiting_for_all_model_exit();
	
	i = get_thread_work_count();
	if (i == 0)
		write_log("all run model exit%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	else
		write_log("all run model NOT exit, %d thread count still running%s%s", i, PRINT_POINT_STR, PRINT_BUG_STR);

	for (i = 0; i < DESTORY_FUNC_COUNT; i++)
	{
		destory_func[i]();
	}
	write_log("destory all model%s%s", PRINT_POINT_STR, PRINT_OK_STR);	

	write_log("charge process exit success...\n\n\n");
	
	return ret;
}

