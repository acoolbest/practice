#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map> 
#include <pthread.h>
#include <unistd.h>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "thread_pool.h"
#include "serial_mgr/src/com_transmission.h"
#include "power_control.h"

#if _POWER_CONTROL_TURN_ON_

static bool g_thread_inited = false;
static bool g_thread_running = true;
static pthread_t g_pthread_pid;

static bool is_thread_running()
{
	return g_thread_running;
}

static void set_thread_running_status(bool b_running)
{
	g_thread_running = b_running;
}

static bool is_valid_power_control_device(uint8_t u8_board_type)
{
	bool b_is_valid = true;
	if (u8_board_type != CCKJ_BOARD_TYPE)
	{
		b_is_valid = false;
	}
	return b_is_valid;
}

static uint32_t device_power_control(uint32_t dw_control_type, uint8_t u8_board_type, char* p_board_num, uint32_t dw_port_num, char * p_device_type, uint8_t byte_power_on_time = HW_POWER_ON_TIME, uint8_t byte_power_off_time = HW_POWER_OFF_TIME)
{
	uint32_t dwSucc = CRG_FAIL;

	switch (dw_control_type)
	{
		case HW_CONTROL_POWER_ON:
			if (u8_board_type == CCKJ_BOARD_TYPE)
			{
				dwSucc = control_mobile_client(u8_board_type, p_board_num, (uint8_t)dw_port_num, connected);
				if (dwSucc == CRG_SUCCESS){
					write_log("%s device POWER ON, board type: %d, board num: %s, port: %d%s%s", p_device_type, u8_board_type, p_board_num, dw_port_num, PRINT_POINT_STR, PRINT_OK_STR);
				}else if(dwSucc == CRG_TIMEOUT){
					write_log("%s device POWER ON, board type: %d, board num: %s, port: %d%s%s", p_device_type, u8_board_type, p_board_num, dw_port_num, PRINT_POINT_STR, PRINT_TIMEOUT_STR);
				}else{
					write_log("%s device POWER ON, board type: %d, board num: %s, port: %d%s%s", p_device_type, u8_board_type, p_board_num, dw_port_num, PRINT_POINT_STR, PRINT_FAIL_STR);
				}
			}

			break;
		case HW_CONTROL_POWER_OFF:
			if (u8_board_type == CCKJ_BOARD_TYPE)
			{
				dwSucc = control_mobile_client(u8_board_type, p_board_num, (uint8_t)dw_port_num, disconnect, byte_power_on_time, byte_power_off_time);
				if (dwSucc == CRG_SUCCESS){
					write_log("%s device POWER OFF, board type: %d, board num: %s, port: %d, NEXT PARAM(POWER ON: %ds, POWER OFF: %ds)%s%s", 
							p_device_type, u8_board_type, p_board_num, dw_port_num, byte_power_on_time, byte_power_off_time, PRINT_POINT_STR, PRINT_OK_STR);
				}else if (dwSucc == CRG_TIMEOUT){
					write_log("%s device POWER OFF, board type: %d, board num: %s, port: %d, NEXT PARAM(POWER ON: %ds, POWER OFF: %ds)%s%s",
							p_device_type, u8_board_type, p_board_num, dw_port_num, byte_power_on_time, byte_power_off_time, PRINT_POINT_STR, PRINT_TIMEOUT_STR);
				}else{
					write_log("%s device POWER OFF, board type: %d, board num: %s, port: %d, NEXT PARAM(POWER ON: %ds, POWER OFF: %ds)%s%s",
							p_device_type, u8_board_type, p_board_num, dw_port_num, byte_power_on_time, byte_power_off_time, PRINT_POINT_STR, PRINT_FAIL_STR);
				}
			}
			break;
		default:
			break;
	}
	return dwSucc;
}

static uint32_t device_light_control(uint32_t dw_control_type, uint8_t u8_board_type, char* p_board_num, uint32_t dw_port_num, char * p_device_type)
{
	uint32_t dwSucc = CRG_FAIL;

	switch (dw_control_type)
	{
		case HW_CONTROL_POWER_ON:
			if (u8_board_type == CCKJ_BOARD_TYPE)
			{
				dwSucc = control_indicateor_light_client(u8_board_type, p_board_num, (uint8_t)dw_port_num, connected);
				if (dwSucc == CRG_SUCCESS)
					write_log("%s Indicator light POWER ON, board type: %d, board num: %s, port: %d%s%s", p_device_type, u8_board_type, p_board_num, dw_port_num, PRINT_POINT_STR, PRINT_OK_STR);
				else if(dwSucc == CRG_TIMEOUT)
					write_log("%s Indicator light POWER ON, board type: %d, board num: %s, port: %d%s%s", p_device_type, u8_board_type, p_board_num, dw_port_num, PRINT_POINT_STR, PRINT_TIMEOUT_STR);
				else
					write_log("%s Indicator light POWER ON, board type: %d, board num: %s, port: %d%s%s", p_device_type, u8_board_type, p_board_num, dw_port_num, PRINT_POINT_STR, PRINT_FAIL_STR);
			}

			break;
		case HW_CONTROL_POWER_OFF:
			if (u8_board_type == CCKJ_BOARD_TYPE)
			{
				dwSucc = control_indicateor_light_client(u8_board_type, p_board_num, (uint8_t)dw_port_num, disconnect);
				if (dwSucc == CRG_SUCCESS)
					write_log("%s Indicator light POWER OFF, board type: %d, board num: %s, port: %d%s%s", p_device_type, u8_board_type, p_board_num, dw_port_num, PRINT_POINT_STR, PRINT_OK_STR);
				else if (dwSucc == CRG_TIMEOUT)
					write_log("%s Indicator light POWER OFF, board type: %d, board num: %s, port: %d%s%s", p_device_type, u8_board_type, p_board_num, dw_port_num, PRINT_POINT_STR, PRINT_TIMEOUT_STR);
				else
					write_log("%s Indicator light POWER OFF, board type: %d, board num: %s, port: %d%s%s", p_device_type, u8_board_type, p_board_num, dw_port_num, PRINT_POINT_STR, PRINT_FAIL_STR);
			}
			break;
		default:
			break;
	}
	return dwSucc;
}

static void* listen_check_hub_work(void *para)
{
	write_log("listen board hub thread%s%s", PRINT_POINT_STR, PRINT_START_STR);

	operator_thread_work_count(INCREASE_TYPE);
	do
	{
		for (uint32_t i = 0; i < 5 * 60; i++)
		{
			if (!is_thread_running()) break;
			sleep(1);
		}
		check_board_hub();
	} while (is_thread_running());

	operator_thread_work_count(DECREASE_TYPE);

	write_log("listen board hub thread%s%s", PRINT_POINT_STR, PRINT_END_STR);
	return (void*)0;
}

static uint32_t create_check_hub_thread()
{
	uint32_t ret = CRG_FAIL;

	int err = pthread_create(&g_pthread_pid, NULL, listen_check_hub_work, NULL);

	if (err != 0){
		write_log("create check hub thread, error: %s%s%s", strerror(err), PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	else{
		ret = CRG_SUCCESS;
	}
	g_thread_inited = (ret == CRG_SUCCESS ? true : false);
	return ret;
}
#endif

uint32_t power_on_control(usb_dev_info_stru * p_usb_dev_info, char* p_device_type)
{
#if _POWER_CONTROL_TURN_ON_
	if (is_valid_power_control_device(p_usb_dev_info->u8_board_type))
	{
		return device_power_control(HW_CONTROL_POWER_ON, p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num, p_device_type);
	}
	else{
		write_log("%s device POWER ON, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, vid: %s, pid: %s%s%s", p_device_type,
			p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
			p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, PRINT_POINT_STR, PRINT_INVALID_BOARD_STR);
		return CRG_INVALID_PARAM;
	}
#else
	return CRG_NO_SUPPORT;
#endif
}

uint32_t power_off_control(usb_dev_info_stru * p_usb_dev_info, char* p_device_type)
{
#if _POWER_CONTROL_TURN_ON_
	if (is_valid_power_control_device(p_usb_dev_info->u8_board_type))
	{
		return device_power_control(HW_CONTROL_POWER_OFF, p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num, p_device_type);
	}
	else{
		write_log("%s device POWER OFF, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, vid: %s, pid: %s%s%s", p_device_type,
			p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
			p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, PRINT_POINT_STR, PRINT_INVALID_BOARD_STR);
		return CRG_INVALID_PARAM;
	}
#else
	return CRG_NO_SUPPORT;
#endif
}

uint32_t light_on_control(usb_dev_info_stru * p_usb_dev_info, char* p_device_type)
{
#if _POWER_CONTROL_TURN_ON_
	if (is_valid_power_control_device(p_usb_dev_info->u8_board_type))
	{
		return device_light_control(HW_CONTROL_POWER_ON, p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num, p_device_type);
	}
	else{
		write_log("%s device LIGHT ON, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, vid: %s, pid: %s%s%s", p_device_type,
			p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
			p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, PRINT_POINT_STR, PRINT_INVALID_BOARD_STR);
		return CRG_INVALID_PARAM;
	}
#else
	return CRG_NO_SUPPORT;
#endif
}

uint32_t light_off_control(usb_dev_info_stru * p_usb_dev_info, char* p_device_type)
{
#if _POWER_CONTROL_TURN_ON_
	if (is_valid_power_control_device(p_usb_dev_info->u8_board_type))
	{
		return device_light_control(HW_CONTROL_POWER_OFF, p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num, p_device_type);
	}
	else{
		write_log("%s device LIGHT OFF, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, vid: %s, pid: %s%s%s", p_device_type,
			p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
			p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, PRINT_POINT_STR, PRINT_INVALID_BOARD_STR);
		return CRG_INVALID_PARAM;
	}
#else
	return CRG_NO_SUPPORT;
#endif
}

uint32_t init_power_model()
{
#if _POWER_CONTROL_TURN_ON_
	uint32_t ret = CRG_FAIL;
	write_log("init power control model%s%s\n", PRINT_POINT_STR,PRINT_START_STR);

	ret = init_com_transmission();

	if (ret == CRG_SUCCESS){
		write_log("init power control model%s%s\n",PRINT_POINT_STR,PRINT_OK_STR);
	}else{
		write_log("init power control model%s%s\n",PRINT_POINT_STR,PRINT_FAIL_STR);
	}
	return ret;
#else
	return CRG_SUCCESS;
#endif
}

uint32_t destory_power_model()
{
#if _POWER_CONTROL_TURN_ON_
	uint32_t ret = CRG_FAIL;
	write_log("destory power model%s%s", PRINT_POINT_STR,PRINT_START_STR);

	ret = destory_com_transmission();

	if (ret){
		write_log("destory power model%s%s\n",PRINT_POINT_STR,PRINT_OK_STR);
	}else{
		write_log("destory power model%s%s\n",PRINT_POINT_STR,PRINT_FAIL_STR);
	}
	return ret;
#else
	return CRG_SUCCESS;
#endif
}

uint32_t run_power_model()
{
#if _POWER_CONTROL_TURN_ON_
	uint32_t ret = CRG_FAIL;
	write_log("run power model%s%s", PRINT_POINT_STR, PRINT_START_STR);
	ret = create_check_hub_thread();
	if (ret == CRG_SUCCESS)
		write_log("run power model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	else
		write_log("run power model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);
	return ret;
#else
	return CRG_SUCCESS;
#endif
}

void set_power_model_exit_status()
{
#if _POWER_CONTROL_TURN_ON_
	set_thread_running_status(false);
#endif
}

void wait_power_model_exit()
{
#if _POWER_CONTROL_TURN_ON_
	write_log("waiting power model stop work%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if(g_thread_inited) pthread_join(g_pthread_pid, NULL);
	g_thread_inited = false;

	set_com_transmission_exit_status();
	wait_com_transmission_exit();
	write_log("waiting power model stop work%s%s", PRINT_POINT_STR, PRINT_END_STR);
#endif
}

