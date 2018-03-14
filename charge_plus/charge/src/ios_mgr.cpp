#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <sys/shm.h>  
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "third_party/libjson/libjson.h"
#include "device_data.h"
#include "thread_pool.h"
#include "idevice_ctrl.h"
#include "ios_shm_mgr.h"
#include "power_control.h"
#include "log_mgr/src/libsendlog.h"
#include "ios_mgr.h"

static pthread_mutex_t							g_dev_mask_data_mutex;
static long 									g_found_dev_time = 0;
static bool										g_inited = false;
static bool										g_all_data_lock_inited = false;

static bool										g_thread_inited = false;
static bool										g_thread_running = true;
static pthread_t 								g_pthread_pid;

static bool is_thread_running()
{
	return g_thread_running;
}

static void set_thread_running_status(bool b_running)
{
	g_thread_running = b_running;
}

static uint32_t init_all_data_lock()
{
	uint32_t ret = CRG_FAIL;
	g_all_data_lock_inited = false;
	if (pthread_mutex_init(&g_dev_mask_data_mutex, NULL) == 0)
	{
		ret = CRG_SUCCESS;
		g_all_data_lock_inited = true;
	}
	else
		write_log("%s init mutex for ios%s%s", PRINT_IOS_STR, PRINT_POINT_STR, PRINT_FAIL_STR);

	return ret;
}

static uint32_t destory_all_data_lock()
{
	if (!g_all_data_lock_inited)
		return CRG_SUCCESS;

	uint32_t ret = CRG_FAIL;
	
	if (pthread_mutex_destroy(&g_dev_mask_data_mutex) == 0)
	{
		ret = CRG_SUCCESS;
	}
	g_all_data_lock_inited = false;

	return ret;
}

static void enter_dev_mask_data_lock()
{
	pthread_mutex_lock(&g_dev_mask_data_mutex);
}

static void leave_dev_mask_data_lock()
{
	pthread_mutex_unlock(&g_dev_mask_data_mutex);
}

static void clear_ios_dev_data(vector<ios_dev_info_stru>* p_vec_ios_dev_info)
{
	while (p_vec_ios_dev_info->size() > 0)
	{
		memset((void*)&((*p_vec_ios_dev_info)[0]), 0, sizeof(ios_dev_info_stru));
		p_vec_ios_dev_info->erase(p_vec_ios_dev_info->begin());
	}

	vector<ios_dev_info_stru>(*p_vec_ios_dev_info).swap(*p_vec_ios_dev_info);
}

void set_found_ios_dev_mask()
{
	struct timeval tv = { 0 };
	gettimeofday(&tv, NULL);

	enter_dev_mask_data_lock();
	g_found_dev_time = tv.tv_sec * 1000000 + tv.tv_usec; // us
	leave_dev_mask_data_lock();
}

static void get_found_ios_dev_mask(long & found_dev_time)
{
	enter_dev_mask_data_lock();
	found_dev_time = g_found_dev_time;
	g_found_dev_time = 0;
	leave_dev_mask_data_lock();

}

static uint32_t wait_get_ios_pid_from_remote(uint32_t & remote_pid)
{
	uint32_t ret = CRG_FAIL;
	uint32_t wait_count = 20;

	// wait 1s
	do
	{
		ret = read_ios_pid_data_from_share_mem(remote_pid);
		if (ret != CRG_EMPTY_DATA)
			break;

		usleep(50 * 1000);
		wait_count--;
	} while (wait_count > 0);

	if (ret == CRG_EMPTY_DATA)
		ret = CRG_TIMEOUT;

	return ret;
}

static uint32_t wait_ios_pid_exit_for_remote(uint32_t remote_pid)
{
	uint32_t ret = CRG_FAIL;
	uint32_t wait_count = 50;
	char path_name[MAX_PATH] = {0};

	snprintf(path_name, sizeof(path_name), "/proc/%d", remote_pid);
	// wait 5s .
	do
	{
		ret = is_exist_dir(path_name);
		if (ret == CRG_NOT_FOUND)
			break;

		usleep(100 * 1000);
		wait_count--;
	} while (wait_count > 0);

	return ret == CRG_NOT_FOUND ? CRG_SUCCESS : CRG_TIMEOUT;
}

static uint32_t enum_ios_list(vector<ios_dev_info_stru>* vec_ios_dev_info)
{
	if (vec_ios_dev_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	uint32_t remote_pid = 0;
	uint32_t json_len = SHARE_MEM_SIZE;
	char json_str[SHARE_MEM_SIZE] = { 0 };
	FILE* remote_fp = NULL;

	ret = init_ios_list_share_mem_model();
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum ios device, but can't init share memory, error: %d%s%s", PRINT_IOS_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		return ret;
	}

	// run idevice
	ret = idevice_enum_dev_list_from_ios_remote(remote_fp);
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum ios device, but can't run ios remote process, error: %d%s%s", PRINT_IOS_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		goto _ENUM_IOS_LIST_EXIT_;
	}

	// get pid
	ret = wait_get_ios_pid_from_remote(remote_pid);
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum ios device, but can't get ios remote pid, error: %d%s%s", PRINT_IOS_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		goto _ENUM_IOS_LIST_EXIT_;
	}

	// wait exit
	ret = wait_ios_pid_exit_for_remote(remote_pid);
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum ios device, but ios remote process can't exit, error: %d%s%s", PRINT_IOS_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		goto _ENUM_IOS_LIST_EXIT_;
	}

	ret = read_ios_list_data_from_share_mem(json_str, json_len);
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum ios device, but can't get any ios device, error: %d%s%s", PRINT_IOS_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		goto _ENUM_IOS_LIST_EXIT_;
	}

	if (json_len > 0)
	{
		clear_ios_dev_data(vec_ios_dev_info);
		ret = parse_json_for_ios_list(string(json_str), vec_ios_dev_info);
		if (ret != CRG_SUCCESS)
		{
			write_log("%s enum ios device, but it's invalid data from share memory%s%s", PRINT_IOS_STR, PRINT_POINT_STR, PRINT_FAIL_STR);
			write_log("%s share memory invalid data is: %s\n", PRINT_IOS_STR, json_str);
		}
	}
	else
	{
		// json_len == 0. is exist ios info in device list 
		if (is_exist_identify_dev_data_from_list_and_usb(IOS_DEVICE_TYPE) == CRG_FOUND)
		{
			write_log("%s enum ios device, but get ios device data is empty and current usb list has ios data, usbmuxd abnormal%s%s", PRINT_IOS_STR, PRINT_POINT_STR, PRINT_FAIL_STR);
			ret = CRG_EMPTY_DATA;
		}
	}

_ENUM_IOS_LIST_EXIT_:

	if (ret == CRG_TIMEOUT || ret == CRG_EMPTY_DATA)
	{
		write_log("%s kill and restart idevice, because of it was ill%s%s", PRINT_IOS_STR, PRINT_POINT_STR, PRINT_START_STR);
		kill_for_ios_remote(remote_pid);
		idevice_start();
		write_log("%s kill and restart idevice, because of it was ill%s%s", PRINT_IOS_STR, PRINT_POINT_STR, PRINT_END_STR);

	}

	if (remote_fp != NULL)
		destory_for_ios_remote(remote_fp);

	destory_ios_list_share_mem_model();

	return vec_ios_dev_info->size() > 0 ? CRG_SUCCESS : CRG_NOT_FOUND;
}

static uint32_t get_detail_info(ios_dev_info_stru* p_ios_dev_info)
{
	if (p_ios_dev_info == NULL || strlen(p_ios_dev_info->sz_unique_dev_id) == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	uint32_t i = 0;
	uint32_t cpy_len = 0;
	string outstr;
	vector<string> vec_detail_info, vec_dic_info;
	map<string, string> dic_info;

	ret = idevice_shell_get_prop(p_ios_dev_info->sz_unique_dev_id, outstr);
	if (ret != CRG_SUCCESS || outstr.length() == 0)
		return ret;

	string_split(outstr, string("\n"), &vec_detail_info);
	outstr.clear();

	if (vec_detail_info.size() <= 2) // test point
		goto _GET_IOS_DETAIL_INFO_;

	ret = CRG_FAIL;
	do
	{
		if (vec_detail_info[i].length() == 0)
			continue;

		vec_dic_info.clear();
		vector<string>(vec_dic_info).swap(vec_dic_info);
		string_split(vec_detail_info[i], string(":"), &vec_dic_info);
		if (vec_dic_info.size() < 2)
			continue;

		insert_string_map(&dic_info, vec_dic_info[0], vec_dic_info[1]);

	} while (++i && i < vec_detail_info.size());

	cpy_len = snprintf(p_ios_dev_info->sz_object, sizeof(p_ios_dev_info->sz_object), "{");

	if (is_string_exist(dic_info, string("SerialNumber"), outstr) == CRG_FOUND)
	{
		memcpy(p_ios_dev_info->sz_serial_no, outstr.c_str(), outstr.length());
		cpy_len += snprintf(p_ios_dev_info->sz_object + cpy_len, sizeof(p_ios_dev_info->sz_object) - cpy_len, " \"SerialNumber\": \"%s\",", outstr.c_str());
	}
	if (is_string_exist(dic_info, string("DeviceName"), outstr) == CRG_FOUND)
	{
		memcpy(p_ios_dev_info->sz_name, outstr.c_str(), outstr.length());
		cpy_len += snprintf(p_ios_dev_info->sz_object + cpy_len, sizeof(p_ios_dev_info->sz_object) - cpy_len, " \"DeviceName\": \"%s\",", outstr.c_str());
	}
	
	if (is_string_exist(dic_info, string("PhoneNumber"), outstr) == CRG_FOUND)
	{
		memcpy(p_ios_dev_info->sz_phone_number, outstr.c_str(), outstr.length());
	//	cpy_len += snprintf(p_ios_dev_info->sz_object + cpy_len, sizeof(p_ios_dev_info->sz_object) - cpy_len, " \"PhoneNumber\": \"%s\",", outstr.c_str());
	}
	
	if (is_string_exist(dic_info, string("ProductVersion"), outstr) == CRG_FOUND)
	{
		memcpy(p_ios_dev_info->sz_product_version, outstr.c_str(), outstr.length());
		cpy_len += snprintf(p_ios_dev_info->sz_object + cpy_len, sizeof(p_ios_dev_info->sz_object) - cpy_len, " \"ProductVersion\": \"%s\",", outstr.c_str());
	}

	if (is_string_exist(dic_info, string("ProductType"), outstr) == CRG_FOUND)
	{
		memcpy(p_ios_dev_info->sz_product_type, outstr.c_str(), outstr.length());
		cpy_len += snprintf(p_ios_dev_info->sz_object + cpy_len, sizeof(p_ios_dev_info->sz_object) - cpy_len, " \"ProductType\": \"%s\",", outstr.c_str());
	}

	if (is_string_exist(dic_info, string("IntegratedCircuitCardIdentity"), outstr) == CRG_FOUND)
	{
		memcpy(p_ios_dev_info->sz_icc_id, outstr.c_str(), outstr.length());
		cpy_len += snprintf(p_ios_dev_info->sz_object + cpy_len, sizeof(p_ios_dev_info->sz_object) - cpy_len, " \"ICCID\": \"%s\",", outstr.c_str());
	}

	if (is_string_exist(dic_info, string("InternationalMobileEquipmentIdentity"), outstr) == CRG_FOUND)
	{
		memcpy(p_ios_dev_info->sz_imei, outstr.c_str(), outstr.length());
		cpy_len += snprintf(p_ios_dev_info->sz_object + cpy_len, sizeof(p_ios_dev_info->sz_object) - cpy_len, " \"IMEI\": \"%s\",", outstr.c_str());
	}
	
	if (strcmp(p_ios_dev_info->sz_unique_dev_id, "") != 0)
	{
		cpy_len += snprintf(p_ios_dev_info->sz_object + cpy_len, sizeof(p_ios_dev_info->sz_object) - cpy_len, " \"UniqueDeviceID\": \"%s\",", p_ios_dev_info->sz_unique_dev_id);
	}

	if (p_ios_dev_info->sz_object[strlen(p_ios_dev_info->sz_object) - 1] == ',')
	{
		p_ios_dev_info->sz_object[strlen(p_ios_dev_info->sz_object) - 1] = ' ';
		p_ios_dev_info->sz_object[strlen(p_ios_dev_info->sz_object)] = '}';
	}
	else
	{
		p_ios_dev_info->sz_object[strlen(p_ios_dev_info->sz_object)] = ' ';
		p_ios_dev_info->sz_object[strlen(p_ios_dev_info->sz_object) + 1] = '}';
	}

	ret = CRG_SUCCESS;

_GET_IOS_DETAIL_INFO_:

	vec_detail_info.clear();
	vector<string>(vec_detail_info).swap(vec_detail_info);
	vec_dic_info.clear();
	vector<string>(vec_dic_info).swap(vec_dic_info);

	clear_map(dic_info);

	return ret;
}

static uint32_t is_access_device(usb_dev_info_stru* p_usb_dev_info, ios_dev_info_stru* p_ios_dev_info)
{
	if (p_ios_dev_info == NULL || p_usb_dev_info == NULL)
		return CRG_INVALID_PARAM;

	string adb_str;
	uint32_t ret = CRG_NOT_FOUND;

	if (find_dev_data_index_from_list(p_usb_dev_info) == CRG_FOUND)
	{
		if (idevice_get_state(p_ios_dev_info->sz_unique_dev_id, adb_str) == CRG_SUCCESS)
		{
			uint32_t str_len = adb_str.length() < strlen(IOS_DEVICE_STATUS) ? adb_str.length() : strlen(IOS_DEVICE_STATUS);
			p_ios_dev_info->b_access = memcmp(adb_str.c_str(), IOS_DEVICE_STATUS, str_len) == 0 ? true : false;
			if(p_ios_dev_info->b_access)
			{
				memset(p_ios_dev_info->sz_state, 0, sizeof(p_ios_dev_info->sz_state));
				memcpy(p_ios_dev_info->sz_state, IOS_DEVICE_STATUS, strlen(IOS_DEVICE_STATUS));
			}
			else
			{
				// memset(p_ios_dev_info->sz_state, 0, sizeof(p_ios_dev_info->sz_state));
				// memcpy(p_ios_dev_info->sz_state, IOS_UNAUTHORIZED_STATUS, strlen(IOS_UNAUTHORIZED_STATUS));
			}
		}
		ret = p_ios_dev_info->b_access ? CRG_SUCCESS : CRG_FAIL;
	}
	
	return ret;
}

static void waiting_access_device(usb_dev_info_stru* p_usb_dev_info, ios_dev_info_stru* p_ios_dev_info)
{
	if (p_ios_dev_info == NULL || p_usb_dev_info == NULL)
		return;

	uint32_t access = CRG_FAIL;
	time_t time_start = time(NULL);

	write_log("%s waiting for accept the trust dialog by user, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", PRINT_IOS_STR,
		p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
		p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_ios_dev_info->sz_unique_dev_id,
		p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, p_ios_dev_info->sz_state, PRINT_POINT_STR, PRINT_START_STR);
	do
	{
		access = is_access_device(p_usb_dev_info, p_ios_dev_info);
		if (access == CRG_SUCCESS
			|| access == CRG_NOT_FOUND)
		{
			break;
		}

		if((time(NULL) - time_start) >= WAIT_TIMEOUT_FOR_IOS_DEVICE)
		{
			time_start = time(NULL);
#if _POWER_CONTROL_TURN_ON_	
			write_log("%s device POWER OFF, because of can't access the device in the %d seconds, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", 
				PRINT_IOS_STR, WAIT_TIMEOUT_FOR_IOS_DEVICE, 
				p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
				p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_ios_dev_info->sz_unique_dev_id,
				p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, p_ios_dev_info->sz_state, PRINT_POINT_STR, PRINT_START_STR);
			// power off
			power_off_control(p_usb_dev_info, PRINT_IOS_STR);
#endif
			if (!is_thread_running())
				break;
		}

		usleep(100*1000); // sleep 100ms
	} while (!p_ios_dev_info->b_access);

	write_log("%s waiting for accept the trust dialog by user, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", PRINT_IOS_STR,
		p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
		p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_ios_dev_info->sz_unique_dev_id,
		p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, p_ios_dev_info->sz_state, PRINT_POINT_STR, PRINT_END_STR);
}

void  process_ios_install_work(gpointer data, gpointer u_data)
{
	uint32_t ret = CRG_NOT_FOUND;
	ios_dev_info_stru ios_dev_info = { 0 };
	dev_all_info_stru dev_all_info = { 0 };
	dev_all_info.p_identify_dev_info = (void*)&ios_dev_info;

	ret = get_new_identify_dev_data_to_list(IOS_DEVICE_TYPE, &dev_all_info);

	if (ret != CRG_FOUND) 
	{
		write_log("%s install app work thread for the new ios device, but not found idle ios from current usb device list, may be the device removed OR bug%s%s", PRINT_IOS_STR, PRINT_POINT_STR, PRINT_NOTFOUND_STR);
		return;
	}
	operator_thread_work_count(INCREASE_TYPE);

	write_log("%s install app work thread for the new ios device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s%s%s", PRINT_IOS_STR,
		dev_all_info.usb_dev_info.u8_board_type, dev_all_info.usb_dev_info.sz_board_num, dev_all_info.usb_dev_info.u8_board_port_num,
		dev_all_info.usb_dev_info.u8_bus_num, dev_all_info.usb_dev_info.u8_addr_num, ios_dev_info.sz_unique_dev_id,
		dev_all_info.usb_dev_info.sz_vid, dev_all_info.usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_START_STR);

	if (!ios_dev_info.b_access)
	{
		waiting_access_device(&dev_all_info.usb_dev_info, &ios_dev_info);
	}
	
	if (ios_dev_info.b_access)
	{
		if (get_detail_info(&ios_dev_info) == CRG_SUCCESS)
		{
			// new device was found
			write_log("%s get ios detail information, serialno: %s, vid: %s, pid: %s, name: %s, type: %s, object: %s%s%s", PRINT_IOS_STR,
				ios_dev_info.sz_unique_dev_id, ios_dev_info.sz_vid, ios_dev_info.sz_pid, ios_dev_info.sz_name, ios_dev_info.sz_product_type, ios_dev_info.sz_object, PRINT_POINT_STR, PRINT_OK_STR);

			update_identify_dev_data_to_list(IOS_DEVICE_TYPE, &ios_dev_info);

			// send log
			ret = crg_send_device_info_to_server(IOS_DEVICE_TYPE, (void*)&ios_dev_info);

#if _POWER_CONTROL_TURN_ON_	
			write_log("%s device indicator light on, because of it's a access device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s",
				PRINT_IOS_STR, 
				dev_all_info.usb_dev_info.u8_board_type, dev_all_info.usb_dev_info.sz_board_num, dev_all_info.usb_dev_info.u8_board_port_num,
				dev_all_info.usb_dev_info.u8_bus_num, dev_all_info.usb_dev_info.u8_addr_num, ios_dev_info.sz_unique_dev_id,
				dev_all_info.usb_dev_info.sz_vid, dev_all_info.usb_dev_info.sz_pid, ios_dev_info.sz_state, PRINT_POINT_STR, PRINT_START_STR);
			
			// Indicateor light 
			light_on_control(&dev_all_info.usb_dev_info, PRINT_IOS_STR);
#endif
		}
		else
			write_log("%s get ios detail information, vid: %s, pid: %s, state: %s%s%s", PRINT_IOS_STR, ios_dev_info.sz_vid, ios_dev_info.sz_pid, ios_dev_info.sz_state, PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	else
	{
		crg_send_device_info_to_server(IOS_DEVICE_TYPE, (void*)&ios_dev_info);
		/*	write_log("%s waiting access device return, thread will exit!!, vid: %s, pid: %s, sn: %s, state: %s%s%s", 
				PRINT_IOS_STR ios_dev_info.sz_vid, ios_dev_info.sz_pid, ios_dev_info.sz_unique_dev_id, IOS_UNAUTHORIZED_STATUS, PRINT_POINT_STR, PRINT_FAIL_STR);
		*/
	}

	write_log("%s install app work thread for the new ios device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", PRINT_IOS_STR,
		dev_all_info.usb_dev_info.u8_board_type, dev_all_info.usb_dev_info.sz_board_num, dev_all_info.usb_dev_info.u8_board_port_num,
		dev_all_info.usb_dev_info.u8_bus_num, dev_all_info.usb_dev_info.u8_addr_num, ios_dev_info.sz_unique_dev_id,
		dev_all_info.usb_dev_info.sz_vid, dev_all_info.usb_dev_info.sz_pid, ios_dev_info.sz_state, PRINT_POINT_STR, PRINT_END_STR);

	clear_usb_dev_data(&dev_all_info.usb_dev_info);

	operator_thread_work_count(DECREASE_TYPE);
}

static void process_ios_work(vector<ios_dev_info_stru>* p_vec_ios_dev_info)
{
	if (p_vec_ios_dev_info == NULL)
		return;

	uint32_t ret = CRG_FAIL;
	usb_dev_info_stru usb_dev_info;
	clear_usb_dev_data(&usb_dev_info);

	if (p_vec_ios_dev_info->size() > 0)
	{
		for (uint32_t i = 0; i < p_vec_ios_dev_info->size(); i++)
		{
			ret = push_identify_dev_data_to_list(IOS_DEVICE_TYPE, &(*p_vec_ios_dev_info)[i], &usb_dev_info);
			if (ret == CRG_SUCCESS)
			{
				write_log("%s new ios device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s%s%s", PRINT_IOS_STR,
					usb_dev_info.u8_board_type, usb_dev_info.sz_board_num, usb_dev_info.u8_board_port_num,
					usb_dev_info.u8_bus_num, usb_dev_info.u8_addr_num, (*p_vec_ios_dev_info)[i].sz_unique_dev_id,
					usb_dev_info.sz_vid, usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_FOUND_STR);
#if _POWER_CONTROL_TURN_ON_	
				write_log("%s device power on, because of it's a ios device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", PRINT_IOS_STR, usb_dev_info.u8_board_type, usb_dev_info.sz_board_num, usb_dev_info.u8_board_port_num, usb_dev_info.u8_bus_num, usb_dev_info.u8_addr_num, (*p_vec_ios_dev_info)[i].sz_unique_dev_id, usb_dev_info.sz_vid, usb_dev_info.sz_pid, (*p_vec_ios_dev_info)[i].sz_state, PRINT_POINT_STR, PRINT_START_STR);
			
				// power on
				power_on_control(&usb_dev_info, PRINT_IOS_STR);
#endif
				// push device to pool thread for install app
				push_new_ios_to_thread_pool((void*)0x01);
			}
			else if (ret == CRG_FOUND)
			{
				// exist
			}
			else if (ret == CRG_NOT_FOUND)
			{
				// removed
			}

		}
	}
	else
	{
		// not find ios device
	}
	clear_usb_dev_data(&usb_dev_info);
}

static void* listen_ios_work(void *arg)
{
	write_log("listen ios work thread%s%s", PRINT_POINT_STR, PRINT_START_STR);

	long time_diff = 0;
	long found_dev_time = 0;
	uint32_t ret = CRG_FAIL;
	vector<ios_dev_info_stru> vec_ios_dev_info;

	operator_thread_work_count(INCREASE_TYPE);
	do 
	{
		get_found_ios_dev_mask(found_dev_time);
		if (found_dev_time <= 0)	// not found device OR error
		{
			usleep(200 * 1000);
			continue;
		}
		struct timeval tv = { 0 };
		gettimeofday(&tv, NULL);

		time_diff = tv.tv_sec * 1000000 + tv.tv_usec - found_dev_time;
		if (time_diff < 2 * 1000 * 1000)
		{
			usleep(2 * 1000 * 1000 - time_diff);
		}

		clear_ios_dev_data(&vec_ios_dev_info);
		ret = enum_ios_list(&vec_ios_dev_info);
		if (ret == CRG_SUCCESS)
		{
			process_ios_work(&vec_ios_dev_info);
		}

	} while (is_thread_running());
	clear_ios_dev_data(&vec_ios_dev_info);

	operator_thread_work_count(DECREASE_TYPE);
	write_log("listen ios work thread%s%s", PRINT_POINT_STR, PRINT_END_STR);

	return (void*)NULL;
}

static uint32_t create_listen_ios_thread()
{
	uint32_t ret = CRG_FAIL;

	int err = pthread_create(&g_pthread_pid, NULL, listen_ios_work, NULL);

	if (err == 0)
	{
		ret = CRG_SUCCESS;
	}
	else
	{
		write_log("%s create ios listen thread, err: %s%s%s", PRINT_IOS_STR, strerror(err), PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	g_thread_inited = (ret == CRG_SUCCESS ? true : false);
	return ret;
} 

uint32_t run_ios_model()
{
	uint32_t ret = CRG_FAIL;
	write_log("run ios model%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if (create_listen_ios_thread() == CRG_SUCCESS)
	{
		ret = CRG_SUCCESS;
		write_log("run ios model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	}
	else
		write_log("run ios model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);

	return ret;
}

uint32_t init_ios_model()
{
	uint32_t ret = CRG_FAIL;
	g_inited = false;

	write_log("init ios model%s%s", PRINT_POINT_STR, PRINT_START_STR);

	if(init_all_data_lock() == CRG_SUCCESS
		&& init_ios_thread_pool(MAX_THREADPOOL_COUNT, false, NULL, process_ios_install_work) == CRG_SUCCESS
		&& idevice_start() == CRG_SUCCESS)
	{
		write_log("init ios model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
		ret = CRG_SUCCESS;
	}
	else
	{
		destory_ios_thread_pool();
		idevice_close();
		destory_all_data_lock();
		write_log("Init ios model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);
	}

	g_inited = (ret == CRG_SUCCESS ? true : false);

	return ret;
}
void set_ios_model_exit_status()
{
	set_thread_running_status(false);
}

void wait_ios_model_exit()
{
	write_log("waiting ios model stop work%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if(g_thread_inited) pthread_join(g_pthread_pid, NULL);
	g_thread_inited = false;
	destory_ios_thread_pool();
	write_log("waiting ios model stop work%s%s", PRINT_POINT_STR, PRINT_END_STR);
}

uint32_t destory_ios_model()
{
	if (!g_inited)
		return CRG_SUCCESS;
	
	write_log("destory ios model%s%s", PRINT_POINT_STR, PRINT_START_STR);

	idevice_close();
	destory_all_data_lock();

	g_inited = false;

	write_log("destory ios model%s%s", PRINT_POINT_STR, PRINT_OK_STR);

	return CRG_SUCCESS;
}
