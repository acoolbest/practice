#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <sys/shm.h>  
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "third_party/libjson/libjson.h"
#include "device_data.h"
#include "thread_pool.h"
#include "adb_ctrl.h"
#include "android_shm_mgr.h"
#include "power_control.h"
#include "log_mgr/src/libsendlog.h"
#include "android_mgr.h"

static pthread_mutex_t							g_dev_mask_data_mutex;
static long 									g_found_dev_time = 0;
static bool										g_inited = false;
static bool										g_all_data_lock_inited = false;
static bool										g_thread_inited = false;

static bool										g_thread_running = true;
static pthread_t g_pthread_pid;


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
		write_log("%s init mutex for android%s%s", PRINT_ANDROID_STR, PRINT_POINT_STR, PRINT_FAIL_STR);

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

static void clear_android_dev_data(vector<android_dev_info_stru>* vec_android_dev_info)
{
	while (vec_android_dev_info->size() > 0)
	{
		memset((void*)&((*vec_android_dev_info)[0]), 0, sizeof(android_dev_info_stru));
		vec_android_dev_info->erase(vec_android_dev_info->begin());
	}
	vector<android_dev_info_stru>(*vec_android_dev_info).swap(*vec_android_dev_info);
}

static uint32_t read_install_android_app_json(vector<install_android_app_info_stru> & out_vec_install_android_app)
{
	uint ret = CRG_FAIL;
	string str_content;
	string str_config_path;
	install_android_app_info_stru install_android_app_info = { 0 };
	
	if (get_app_json_config_file_path(str_config_path) == CRG_NOT_FOUND)
	{
		// not exist
		write_log("%s file name: %s file %s%s", PRINT_ANDROID_STR, APP_LIST_NAME_JSON, PRINT_POINT_STR, PRINT_NOTFOUND_STR);
		return CRG_NOT_FOUND;
	}

	if (read_file(str_config_path.c_str(), str_content) != CRG_SUCCESS)
	{
		write_log("%s file name: %s file%s%s", PRINT_ANDROID_STR, str_config_path.c_str(), PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		return CRG_FAIL;
	}

	if (libjson::is_valid(str_content))
	{
		JSONNode n = libjson::parse(str_content);
		JSONNode::const_iterator i = n.begin();
		
		while (i != n.end())
		{
			string node_name = i->name();
			if (node_name == ANDROID_STR)
			{
				if (i->type() == JSON_ARRAY)
				{
					ret = CRG_EMPTY_DATA;
					JSONNode::const_iterator k = (*i).begin();
					while (k != (*i).end())
					{
						memset((void*)&install_android_app_info, 0, sizeof(install_android_app_info_stru));
						JSONNode::const_iterator j = (*k).begin();
						while (j != (*k).end())
						{
							node_name = j->name();
							if (node_name == APP_ID_KEY)
								memcpy(install_android_app_info.sz_apk_id, j->as_string().c_str(), strlen(j->as_string().c_str()));
							else if (node_name == APP_BUNDLE_ID_KEY)
								memcpy(install_android_app_info.sz_apk_package, j->as_string().c_str(), strlen(j->as_string().c_str()));
							else if (node_name == APP_NAME_STR)
								memcpy(install_android_app_info.sz_apk_name, j->as_string().c_str(), strlen(j->as_string().c_str()));
							else if (node_name == APP_PATH_STR)
								memcpy(install_android_app_info.sz_apk_path, j->as_string().c_str(), strlen(j->as_string().c_str()));
							else if (node_name == APP_VERSION_STR)
								memcpy(install_android_app_info.sz_apk_version, j->as_string().c_str(), strlen(j->as_string().c_str()));
							else if (node_name == APP_AUTO_RUN_STR)
								memcpy(install_android_app_info.sz_apk_auto_run, j->as_string().c_str(), strlen(j->as_string().c_str()));
				
							++j;
						}
						out_vec_install_android_app.push_back(install_android_app_info);
						++k;
					}
				}
			}
			++i;
		}
	}
	
	if (out_vec_install_android_app.size() == 0)
	{
		if(ret == CRG_EMPTY_DATA)
			write_log("%s don't found any data from file name: %s file%s%s", PRINT_ANDROID_STR, str_config_path.c_str(), PRINT_POINT_STR, PRINT_EMPTY_DATA_STR);
		else
			write_log("%s file name: %s file%s%s", PRINT_ANDROID_STR, str_config_path.c_str(), PRINT_POINT_STR, PRINT_ERR_FILE_STR);
	}						
	return out_vec_install_android_app.size() > 0 ? CRG_SUCCESS : CRG_FAIL;
}

void set_found_android_dev_mask()
{
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);

	enter_dev_mask_data_lock();
	g_found_dev_time = tv.tv_sec*1000000 + tv.tv_usec; // us
	leave_dev_mask_data_lock();
}

static void get_found_android_dev_mask(long & found_dev_time)
{
	enter_dev_mask_data_lock();
	found_dev_time = g_found_dev_time;
	g_found_dev_time = 0;
	leave_dev_mask_data_lock();
}

static void print_install_result(uint32_t interval, uint32_t result, string str_err)
{
	switch (result)
	{
	case CRG_SUCCESS:
		write_log("[TIME: %ds] %s", interval, PRINT_OK_STR);
		break;
	case CRG_NOT_ENOUGH_BUFF:
		write_log("%s", PRINT_NOSPACE_STR);
		break;
	case CRG_CANCELED_BY_USER:
		write_log("%s", PRINT_CANCELED_BY_USER_STR);
		break;
	case CRG_INTERNAL_ERROR_STR:
		write_log("%s", PRINT_INTERNAL_ERROR_STR);
		break;
	case CRG_VERIFICATION_TIMEOUT_BY_USER:
		write_log("%s", PRINT_VERIFICATION_TIMEOUT_BY_USER_STR);
		break;
	case CRG_VERIFICATION_FAILURE_BY_USER:
		write_log("%s", PRINT_VERIFICATION_FAILURE_BY_USER_STR);
		break;
	case CRG_DEVICE_NOT_FOUND:
		write_log("%s", PRINT_DEVICE_NOT_FOUND_STR);
		break;
	case CRG_DEVICE_OFF_LINE:
		write_log("%s", PRINT_DEVICE_OFF_LINE_STR);
		break;
	case CRG_NOT_START_SYSTEM:
		write_log("%s", PRINT_NOT_START_SYSTEM_STR);
		break;
	case CRG_DEVICE_INVALID_APK:
		write_log("%s", PRINT_INVALID_APK_STR);
		break;
	default:
		write_log("[ERR: %s] %s", str_err.c_str(), PRINT_FAIL_STR);
		break;
	}
}

static uint32_t wait_get_android_pid_from_remote(uint32_t & remote_pid)
{
	uint32_t ret = CRG_FAIL;
	uint32_t wait_count = 20;
	
	// wait 1s
	do
	{
		ret = read_android_pid_data_from_share_mem(remote_pid);
		if (ret != CRG_EMPTY_DATA)
			break;

		usleep(50 * 1000);
		wait_count--;
	} while (wait_count > 0);

	if (ret == CRG_EMPTY_DATA)
		ret = CRG_TIMEOUT;
	
	return ret;
}

static uint32_t wait_android_pid_exit_for_remote(uint32_t remote_pid)
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

static uint32_t enum_android_list(vector<android_dev_info_stru>* vec_android_dev_info)
{
	if (vec_android_dev_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	uint32_t remote_pid = 0;
	uint32_t json_len = SHARE_MEM_SIZE;
	char json_str[SHARE_MEM_SIZE] = { 0 };
	FILE* remote_fp = NULL;

	static uint8_t adb_return_null_count = 0;
	
	ret = init_android_list_share_mem_model();
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum android device, but can't init share memory, error: %d%s%s", PRINT_ANDROID_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		return ret;
	}

	// run adb
	ret = adb_enum_dev_list_from_android_remote(remote_fp);
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum android device, but can't run android remote process, error: %d%s%s", PRINT_ANDROID_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		goto _ENUM_ANDROID_LIST_EXIT_;
	}
	// get pid
	ret = wait_get_android_pid_from_remote(remote_pid);
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum android device, but can't get android remote pid, error: %d%s%s", PRINT_ANDROID_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		goto _ENUM_ANDROID_LIST_EXIT_;
	}
	
	// wait exit
	ret = wait_android_pid_exit_for_remote(remote_pid);
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum android device, but android remote process can't exit, error: %d%s%s", PRINT_ANDROID_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		goto _ENUM_ANDROID_LIST_EXIT_;
	}

	ret = read_android_list_data_from_share_mem(json_str, json_len);
	if (ret != CRG_SUCCESS)
	{
		write_log("%s enum android device, but can't get any android device, error: %d%s%s", PRINT_ANDROID_STR, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		goto _ENUM_ANDROID_LIST_EXIT_;
	}

	if (json_len > 0)
	{
		clear_android_dev_data(vec_android_dev_info);
		ret = parse_json_for_android_list(string(json_str), vec_android_dev_info);
		if (ret != CRG_SUCCESS)
		{
			write_log("%s enum android device, but it's invalid data from share memory%s%s", PRINT_ANDROID_STR, PRINT_POINT_STR, PRINT_FAIL_STR);
			write_log("%s share memory invalid data is: %s\n", PRINT_ANDROID_STR, json_str);
		}
	}
	else 
	{
		// json_len == 0. is exist android info in device list 
		if (is_exist_identify_dev_data_from_list_and_usb(ANDROID_DEVICE_TYPE) == CRG_FOUND)
		{
			write_log("%s enum android device, but get android device data is empty and current usb list has android data, adb abnormal%s%s", PRINT_ANDROID_STR, PRINT_POINT_STR, PRINT_FAIL_STR);
			ret = CRG_EMPTY_DATA;
		}
		else
		{
			adb_return_null_count++;
			write_log("adb return null, so do the usb_list!, adb_return_null_count is %d",adb_return_null_count);
			if(adb_return_null_count >= 10)
			{
				write_log("kill adb case by <adb return null>");
				ret = CRG_EMPTY_DATA;
			}	
		}
	}
	
_ENUM_ANDROID_LIST_EXIT_:

	if (ret == CRG_TIMEOUT || ret == CRG_EMPTY_DATA)
	{
		adb_return_null_count = 0;
		write_log("%s kill and restart adb, because of it was ill%s%s", PRINT_ANDROID_STR, PRINT_POINT_STR, PRINT_START_STR);
		kill_for_android_remote(remote_pid);
		adb_start();
		write_log("%s kill and restart adb, because of it was ill%s%s", PRINT_ANDROID_STR, PRINT_POINT_STR, PRINT_END_STR);
	}
	
	if (remote_fp != NULL)
		destory_for_android_remote(remote_fp);
	
	destory_android_list_share_mem_model();

	if(vec_android_dev_info->size() > 0) adb_return_null_count = 0;
	return vec_android_dev_info->size() > 0 ? CRG_SUCCESS : CRG_NOT_FOUND;
}

static uint32_t get_detail_info(android_dev_info_stru* p_android_dev_info)
{
	if (p_android_dev_info == NULL || memcmp(p_android_dev_info->sz_path, "", strlen(p_android_dev_info->sz_path) == 0))
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	uint32_t i = 0;
	string outstr;
	vector<string> vec_detail_info, vec_dic_info;
	map<string, string> dic_info;

	ret = adb_shell_get_prop(p_android_dev_info->sz_path, outstr);
	if (ret != CRG_SUCCESS || outstr.length() == 0)
		return ret;

	string_split(outstr, string("\r\n"), &vec_detail_info);
	
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

	vec_detail_info.clear();
	vector<string>(vec_detail_info).swap(vec_detail_info);
	vec_dic_info.clear();
	vector<string>(vec_dic_info).swap(vec_dic_info);

	int cpy_len = 0;
	cpy_len = snprintf(p_android_dev_info->sz_name, sizeof(p_android_dev_info->sz_name), "{");

	if (is_string_exist(dic_info, string("[ro.serialno]"), outstr) == CRG_FOUND
		&& outstr.find("[]") == string::npos)
	{
		if (strcmp(p_android_dev_info->sz_dev_id, "") == 0)
		{
			memset(p_android_dev_info->sz_dev_id, 0, sizeof(p_android_dev_info->sz_dev_id));
			memcpy(p_android_dev_info->sz_dev_id, outstr.substr(2, outstr.length() - 3).c_str(), outstr.length() - 3);
		}
		cpy_len += snprintf(p_android_dev_info->sz_name + cpy_len, sizeof(p_android_dev_info->sz_name) - cpy_len, " \"serial_id\": \"%s\",", p_android_dev_info->sz_dev_id);
	}
	if (is_string_exist(dic_info, string("[ro.product.board]"), outstr) == CRG_FOUND)
		cpy_len += snprintf(p_android_dev_info->sz_name + cpy_len, sizeof(p_android_dev_info->sz_name) - cpy_len, " \"board\": \"%s\",", outstr.substr(2, outstr.length() - 3).c_str());
	
	if (is_string_exist(dic_info, string("[ro.build.id]"), outstr) == CRG_FOUND)
		cpy_len += snprintf(p_android_dev_info->sz_name + cpy_len, sizeof(p_android_dev_info->sz_name) - cpy_len, " \"buildid\": \"%s\",", outstr.substr(2, outstr.length() - 3).c_str());
	
	if (is_string_exist(dic_info, string("[ro.product.brand]"), outstr) == CRG_FOUND)
	{
		cpy_len += snprintf(p_android_dev_info->sz_name + cpy_len, sizeof(p_android_dev_info->sz_name) - cpy_len, " \"brand\": \"%s\",", outstr.substr(2, outstr.length() - 3).c_str());
		memset(p_android_dev_info->sz_brand, 0, sizeof(p_android_dev_info->sz_brand));
		memcpy(p_android_dev_info->sz_brand, outstr.substr(2, outstr.length() - 3).c_str(), outstr.length() - 3);
	}
	if (is_string_exist(dic_info, string("[ro.product.cpu.abi]"), outstr) == CRG_FOUND)
		cpy_len += snprintf(p_android_dev_info->sz_name + cpy_len, sizeof(p_android_dev_info->sz_name) - cpy_len, " \"cpuabi\": \"%s\",", outstr.substr(2, outstr.length() - 3).c_str());

	if (is_string_exist(dic_info, string("[ro.product.model]"), outstr) == CRG_FOUND)
	{
		cpy_len += snprintf(p_android_dev_info->sz_name + cpy_len, sizeof(p_android_dev_info->sz_name) - cpy_len, " \"model\": \"%s\",", outstr.substr(2, outstr.length() - 3).c_str());
		memset(p_android_dev_info->sz_model, 0, sizeof(p_android_dev_info->sz_model));
		memcpy(p_android_dev_info->sz_model, outstr.substr(2, outstr.length() - 3).c_str(), outstr.length() - 3);
	}
	if (is_string_exist(dic_info, string("[ro.build.version.release]"), outstr) == CRG_FOUND)
	{
		cpy_len += snprintf(p_android_dev_info->sz_name + cpy_len, sizeof(p_android_dev_info->sz_name) - cpy_len, " \"androidversion\": \"%s\",", outstr.substr(2, outstr.length() - 3).c_str());
		memset(p_android_dev_info->sz_os_version, 0, sizeof(p_android_dev_info->sz_os_version));
		memcpy(p_android_dev_info->sz_os_version, outstr.substr(2, outstr.length() - 3).c_str(), outstr.length() - 3);
	}

	if (strcmp(p_android_dev_info->sz_dev_id, "") != 0)
	{
		cpy_len += snprintf(p_android_dev_info->sz_name + cpy_len, sizeof(p_android_dev_info->sz_name) - cpy_len, " \"device_id\": \"%s\",", p_android_dev_info->sz_dev_id);
	}
	if (p_android_dev_info->sz_name[strlen(p_android_dev_info->sz_name) - 1] == ',')
	{
		p_android_dev_info->sz_name[strlen(p_android_dev_info->sz_name) - 1] = ' ';
		p_android_dev_info->sz_name[strlen(p_android_dev_info->sz_name)] = '}';
	}
	else
	{
		p_android_dev_info->sz_name[strlen(p_android_dev_info->sz_name)] = ' ';
		p_android_dev_info->sz_name[strlen(p_android_dev_info->sz_name) + 1] = '}';
	}
	// snprintf(p_android_dev_info->sz_name + cpy_len, sizeof(p_android_dev_info->sz_name) - cpy_len, " }");

	clear_map(dic_info);

	return CRG_SUCCESS;
}

static uint32_t is_access_device(usb_dev_info_stru* p_usb_dev_info, android_dev_info_stru* p_android_dev_info)
{
	if (p_android_dev_info == NULL || p_usb_dev_info == NULL)
		return CRG_INVALID_PARAM;

	string adb_str;
	uint32_t ret = CRG_NOT_FOUND;

	if (find_dev_data_index_from_list(p_usb_dev_info) == CRG_FOUND)
	{
		if (adb_get_state(p_android_dev_info->sz_path, adb_str) == CRG_SUCCESS)
		{
			uint32_t str_len = adb_str.length() < strlen(ANDROID_DEVICE_STATUS) ? adb_str.length() : strlen(ANDROID_DEVICE_STATUS);
			p_android_dev_info->b_access = memcmp(adb_str.c_str(), ANDROID_DEVICE_STATUS, str_len) == 0 ? true : false;
			if(p_android_dev_info->b_access)
			{
				memset(p_android_dev_info->sz_state, 0, sizeof(p_android_dev_info->sz_state));
				memcpy(p_android_dev_info->sz_state, ANDROID_DEVICE_STATUS, strlen(ANDROID_DEVICE_STATUS));
			}
			else
			{
				// memset(p_android_dev_info->sz_state, 0, sizeof(p_android_dev_info->sz_state));
				// memcpy(p_android_dev_info->sz_state, ANDROID_UNAUTHORIZED_STATUS, strlen(ANDROID_UNAUTHORIZED_STATUS));
			}
		}
		ret = p_android_dev_info->b_access ? CRG_SUCCESS : CRG_FAIL;
	}
	
	return ret;
}

static void waiting_access_device(usb_dev_info_stru* p_usb_dev_info, android_dev_info_stru* p_android_dev_info)
{
	if (p_android_dev_info == NULL || p_usb_dev_info == NULL)
		return;

	uint32_t access = CRG_FAIL;
	time_t time_start = time(NULL);

	write_log("%s waiting for accept the trust dialog by user, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", PRINT_ANDROID_STR,
		p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
		p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_android_dev_info->sz_dev_id,
		p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, p_android_dev_info->sz_state, PRINT_POINT_STR, PRINT_START_STR);

	do
	{
		access = is_access_device(p_usb_dev_info, p_android_dev_info);
		if (access == CRG_SUCCESS
			|| access == CRG_NOT_FOUND)
		{
			break;
		}

		if ((time(NULL) - time_start) >= WAIT_TIMEOUT_FOR_ANDROID_DEVICE)
		{
			time_start = time(NULL);
#if _POWER_CONTROL_TURN_ON_	
			write_log("%s device POWER OFF, because of can't access the device in the %d seconds, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", 
				PRINT_ANDROID_STR, WAIT_TIMEOUT_FOR_ANDROID_DEVICE, 
				p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
				p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_android_dev_info->sz_dev_id,
				p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, p_android_dev_info->sz_state, PRINT_POINT_STR, PRINT_START_STR);
			// power off
			power_off_control(p_usb_dev_info, PRINT_ANDROID_STR);
#endif
			if (!is_thread_running())
				break;
		}

		usleep(100*1000); // sleep 100ms
	} while (!p_android_dev_info->b_access);

	write_log("%s waiting for accept the trust dialog by user, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", PRINT_ANDROID_STR,
		p_usb_dev_info->u8_board_type, p_usb_dev_info->sz_board_num, p_usb_dev_info->u8_board_port_num,
		p_usb_dev_info->u8_bus_num, p_usb_dev_info->u8_addr_num, p_android_dev_info->sz_dev_id,
		p_usb_dev_info->sz_vid, p_usb_dev_info->sz_pid, p_android_dev_info->sz_state, PRINT_POINT_STR, PRINT_END_STR);

}

static int get_installed_app_list(android_dev_info_stru* p_android_dev_info, vector<string> & out_vec_app_list)
{
	if (p_android_dev_info == NULL)
		return CRG_INVALID_PARAM;

	int ret = CRG_FAIL;
	string out_str;

	ret = adb_enum_installed_app_list(p_android_dev_info->sz_path, out_str);

	if (ret == CRG_SUCCESS)
	{
		string_split(out_str, string("\r\n"), &out_vec_app_list);
	}
	
	return ret;
}

static uint32_t install_one_app(android_dev_info_stru* p_android_dev_info, install_android_app_info_stru* p_install_app_info, uint32_t & out_interval, string & out_str_err)
{
	if (p_android_dev_info == NULL || p_install_app_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t  ret = CRG_FAIL;

	time_t begin_time = 0, end_time = 0;
	
	begin_time  = time(NULL);
	ret = adb_install_one_app(p_android_dev_info->sz_path, p_install_app_info->sz_apk_path, out_str_err);
	end_time = time(NULL);

	if (ret == CRG_NOT_START_SYSTEM)
	{
		sleep(10);
	}
	else if (ret != CRG_SUCCESS)
	{
	}

	out_interval = end_time - begin_time;
	
	return ret;
}

static bool is_need_auto_run(char * p_apk_auto_run)
{
	if (p_apk_auto_run == NULL)
		return false;

	return (strcasecmp(p_apk_auto_run, APP_AUTO_RUN) == 0) ? true : false;
}

static uint32_t auto_run_one_app(android_dev_info_stru* p_android_dev_info, install_android_app_info_stru* p_install_app_info)
{
	if (p_android_dev_info == NULL || p_install_app_info == NULL)
		return CRG_INVALID_PARAM;

	return adb_run_one_app(p_android_dev_info->sz_path, p_install_app_info->sz_apk_package);
}

static uint32_t install_all_app(dev_all_info_stru* p_dev_all_info)
{
	if (p_dev_all_info == NULL || p_dev_all_info->p_identify_dev_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	uint32_t i = 0;
	uint32_t interval = 0;
	vector<string> vec_installed_app_list;
	vector<install_android_app_info_stru> vec_install_android_app, vec_install_android_app_fail_list;
	android_dev_info_stru* p_android_dev_info = NULL;
	
	p_android_dev_info = (android_dev_info_stru*)p_dev_all_info->p_identify_dev_info;

	write_log("%s installing all android app for android device brand: %s, model: %s%s%s", PRINT_ANDROID_STR, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_START_STR);

	//ResetCurrentPath();
	ret = read_install_android_app_json(vec_install_android_app);
	if (ret != CRG_SUCCESS)
		goto _INSTALL_ANDROID_APP_ERR_;

	ret = get_installed_app_list(p_android_dev_info, vec_installed_app_list);
	if (ret != CRG_SUCCESS)
		write_log("%s get installed app list for android device brand: %s, model: %s%s%s", PRINT_ANDROID_STR, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_FAIL_STR);

	for (i = 0; i < vec_install_android_app.size(); i++)
	{
		if (!is_thread_running()) break;

		write_log("%s installing app name: %s for android device brand: %s, model: %s%s%s", PRINT_ANDROID_STR,
			vec_install_android_app[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_START_STR);

		if (find_string_from_vector(string(vec_install_android_app[i].sz_apk_package), &vec_installed_app_list, strlen("package:")) == CRG_FOUND)
		{
			write_log("%s installing app name: %s for android device brand: %s, model: %s%s%s", PRINT_ANDROID_STR,
				vec_install_android_app[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_EXIST_STR);
			continue; 
		}
		else
		{
			if (is_exist_file(vec_install_android_app[i].sz_apk_path) == CRG_NOT_FOUND)
			{
				// not exist
				write_log("%s file name: %s file %s%s", PRINT_ANDROID_STR, vec_install_android_app[i].sz_apk_path, PRINT_POINT_STR, PRINT_NOTFOUND_STR);
				write_log("%s installing app name: %s for android device brand: %s, model: %s%s%s", PRINT_ANDROID_STR,
					vec_install_android_app[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_FAIL_STR);
				continue;
			}
		}
		string str_err;
		ret = install_one_app(p_android_dev_info, &vec_install_android_app[i], interval, str_err);

		write_log("%s installing app name: %s for android device brand: %s, model: %s%s", PRINT_ANDROID_STR,
			vec_install_android_app[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR);
		print_install_result(interval, ret, str_err);

		if (ret == CRG_SUCCESS)
		{
			if (is_need_auto_run(vec_install_android_app[i].sz_apk_auto_run))
			{
				sleep(3);
				if (auto_run_one_app(p_android_dev_info, &vec_install_android_app[i]) == CRG_SUCCESS)
				{
					write_log("%s running app name: %s for android device brand: %s, model: %s%s%s", PRINT_ANDROID_STR,
						vec_install_android_app[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_OK_STR);
				}
				else
				{
					write_log("%s running app name: %s for android device brand: %s, model: %s%s%s", PRINT_ANDROID_STR,
						vec_install_android_app[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_FAIL_STR);
				}
			}
		}
		crg_send_install_app_result(ret, ANDROID_DEVICE_TYPE, APP_SOURCE_FROM_CC, vec_install_android_app[i].sz_apk_id, interval, (void*)p_android_dev_info);

		switch (ret)
		{
		case CRG_SUCCESS:
		case CRG_NOT_ENOUGH_BUFF:
		case CRG_DEVICE_INVALID_APK:
			break;
		case CRG_VERIFICATION_TIMEOUT_BY_USER:
		case CRG_CANCELED_BY_USER:
		case CRG_NOT_START_SYSTEM:
			vec_install_android_app_fail_list.push_back(vec_install_android_app[i]);
			break;
		case CRG_DEVICE_NOT_FOUND:
		case CRG_DEVICE_OFF_LINE:
			goto _INSTALL_ANDROID_APP_ERR_;
		default:
			vec_install_android_app_fail_list.push_back(vec_install_android_app[i]);
			break;

		}
#if _POWER_CONTROL_TURN_ON_
		if ((ret == CRG_CANCELED_BY_USER) || (ret == CRG_VERIFICATION_TIMEOUT_BY_USER))
		{
			write_log("%s android device POWER OFF and LIGHT OFF, because of canceled by user, device id: %s, vid: %s, pid: %s%s%s", PRINT_ANDROID_STR,
				p_android_dev_info->sz_dev_id, p_android_dev_info->sz_vid, p_android_dev_info->sz_pid, PRINT_POINT_STR, PRINT_START_STR);

			power_off_control(&p_dev_all_info->usb_dev_info, PRINT_ANDROID_STR);
			light_off_control(&p_dev_all_info->usb_dev_info, PRINT_ANDROID_STR);
		}
#endif
	}
	/*
	ret = CRG_SUCCESS;// retry for fail app list
	for (i = 0; i < vec_install_android_app_fail_list.size(); i++)
	{
		if (!is_thread_running()) break;
		
		write_log("%s retry installing app name: %s for device brand: %s, model: %s%s%s", PRINT_ANDROID_STR,
			vec_install_android_app_fail_list[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_START_STR);

		string str_err;
		ret = install_one_app(p_android_dev_info, &vec_install_android_app_fail_list[i], interval, str_err);
		
		write_log("%s retry installing app name: %s for device brand: %s, model: %s%s", PRINT_ANDROID_STR,
			vec_install_android_app_fail_list[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR);
		print_install_result(interval, ret, str_err);

		if (ret == CRG_SUCCESS)
		{
			if (is_need_auto_run(vec_install_android_app_fail_list[i].sz_apk_auto_run))
			{
				sleep(3);
				if (auto_run_one_app(p_android_dev_info, &vec_install_android_app_fail_list[i]) == CRG_SUCCESS)
				{
					write_log("%s running app name: %s for device brand: %s, model: %s%s%s", PRINT_ANDROID_STR,
						vec_install_android_app_fail_list[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_OK_STR);
				}
				else
				{
					write_log("%s running app name: %s for device brand: %s, model: %s%s%s", PRINT_ANDROID_STR,
						vec_install_android_app_fail_list[i].sz_apk_name, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_FAIL_STR);
				}
			}
		}
		crg_send_install_app_result(ret, ANDROID_DEVICE_TYPE, APP_SOURCE_FROM_CC, vec_install_android_app_fail_list[i].sz_apk_id, interval, (void*)p_android_dev_info);

#if _POWER_CONTROL_TURN_ON_
		if ((ret == CRG_CANCELED_BY_USER) || (ret == CRG_VERIFICATION_TIMEOUT_BY_USER))
		{
			write_log("%s android device POWER OFF and LIGHT OFF, because of canceled by user, device id: %s, vid: %s, pid: %s%s%s", PRINT_ANDROID_STR,
				p_android_dev_info->sz_dev_id, p_android_dev_info->sz_vid, p_android_dev_info->sz_pid, PRINT_POINT_STR, PRINT_START_STR);
			
			power_off_control(&p_dev_all_info->usb_dev_info, PRINT_ANDROID_STR);
			light_off_control(&p_dev_all_info->usb_dev_info, PRINT_ANDROID_STR);
		}
#endif
		if (ret != CRG_SUCCESS)
			ret = CRG_FAIL;
	}
*/
_INSTALL_ANDROID_APP_ERR_:

	vec_installed_app_list.clear();
	vector<string>(vec_installed_app_list).swap(vec_installed_app_list);
	vec_install_android_app.clear();
	vector<install_android_app_info_stru>(vec_install_android_app).swap(vec_install_android_app);
	vec_install_android_app_fail_list.clear();
	vector<install_android_app_info_stru>(vec_install_android_app_fail_list).swap(vec_install_android_app_fail_list);

	write_log("%s installing all android app for android device brand: %s, model: %s%s%s\n", PRINT_ANDROID_STR, p_android_dev_info->sz_brand, p_android_dev_info->sz_model, PRINT_POINT_STR, PRINT_END_STR);

	return ret;
}

void  process_android_install_work(gpointer data, gpointer u_data)
{
	uint32_t ret = CRG_NOT_FOUND;
	android_dev_info_stru android_dev_info = { 0 };
	dev_all_info_stru dev_all_info = { 0 };
	dev_all_info.p_identify_dev_info = (void*)&android_dev_info;

	ret = get_new_identify_dev_data_to_list(ANDROID_DEVICE_TYPE, &dev_all_info);

	if (ret != CRG_FOUND)
	{
		write_log("%s install app work thread for the new android device, but not found idle android from current usb device list, may be removed OR bug%s%s", PRINT_ANDROID_STR, PRINT_POINT_STR, PRINT_NOTFOUND_STR);
		return;
	}

	operator_thread_work_count(INCREASE_TYPE);

	write_log("%s install app work thread for the new android device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s%s%s", PRINT_ANDROID_STR,
		dev_all_info.usb_dev_info.u8_board_type, dev_all_info.usb_dev_info.sz_board_num, dev_all_info.usb_dev_info.u8_board_port_num,
		dev_all_info.usb_dev_info.u8_bus_num, dev_all_info.usb_dev_info.u8_addr_num, android_dev_info.sz_dev_id,
		dev_all_info.usb_dev_info.sz_vid, dev_all_info.usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_START_STR);

	if (!android_dev_info.b_access)
	{
		waiting_access_device(&dev_all_info.usb_dev_info, &android_dev_info);
	}

	if (android_dev_info.b_access)
	{
		if (get_detail_info(&android_dev_info) == CRG_SUCCESS)
		{
			// new device was found
			write_log("%s get android detail information, serialno: %s, vid: %s, pid: %s, brand: %s, model: %s, state: %s%s%s", PRINT_ANDROID_STR, 
				android_dev_info.sz_dev_id, android_dev_info.sz_vid, android_dev_info.sz_pid, android_dev_info.sz_brand, android_dev_info.sz_model, android_dev_info.sz_state, PRINT_POINT_STR, PRINT_OK_STR);
		
			update_identify_dev_data_to_list(ANDROID_DEVICE_TYPE, &android_dev_info);
			
			// send log
			ret = crg_send_device_info_to_server(ANDROID_DEVICE_TYPE, (void*)&android_dev_info);
				
#if _POWER_CONTROL_TURN_ON_	
			write_log("%s device indicator light on, because of it's a access device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s",
				PRINT_ANDROID_STR, 
				dev_all_info.usb_dev_info.u8_board_type, dev_all_info.usb_dev_info.sz_board_num, dev_all_info.usb_dev_info.u8_board_port_num,
				dev_all_info.usb_dev_info.u8_bus_num, dev_all_info.usb_dev_info.u8_addr_num, android_dev_info.sz_dev_id,
				dev_all_info.usb_dev_info.sz_vid, dev_all_info.usb_dev_info.sz_pid, android_dev_info.sz_state, PRINT_POINT_STR, PRINT_START_STR);
			// Indicateor light 
			light_on_control(&dev_all_info.usb_dev_info, PRINT_ANDROID_STR);
#endif
			// set install location
			if (adb_set_install_location(android_dev_info.sz_path, 0) == CRG_SUCCESS)
				write_log("%s set install location for android device, brand: %s, model: %s%s%s", PRINT_ANDROID_STR, android_dev_info.sz_brand, android_dev_info.sz_model, PRINT_POINT_STR, PRINT_OK_STR);
			else
				write_log("%s set install location for android device, brand: %s, model: %s%s%s", PRINT_ANDROID_STR, android_dev_info.sz_brand, android_dev_info.sz_model, PRINT_POINT_STR, PRINT_FAIL_STR);
	
			install_all_app(&dev_all_info);
		}
		else
			write_log("%s get android detail information, vid: %s, pid: %s, state: %s%s%s", PRINT_ANDROID_STR, android_dev_info.sz_vid, android_dev_info.sz_pid, android_dev_info.sz_state, PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	else
	{
		crg_send_device_info_to_server(ANDROID_DEVICE_TYPE, (void*)&android_dev_info);
	/*	write_log("%s waiting access device return , thread will exit!!, vid: %s, pid: %s, sn: %s, state: %s%s%s", 
			PRINT_ANDROID_STR, android_dev_info.sz_vid, android_dev_info.sz_pid, android_dev_info.sz_dev_id, ANDROID_UNAUTHORIZED_STATUS, PRINT_POINT_STR, PRINT_FAIL_STR);
	*/
	}

	write_log("%s install app work thread for the new android device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", PRINT_ANDROID_STR,
		dev_all_info.usb_dev_info.u8_board_type, dev_all_info.usb_dev_info.sz_board_num, dev_all_info.usb_dev_info.u8_board_port_num,
		dev_all_info.usb_dev_info.u8_bus_num, dev_all_info.usb_dev_info.u8_addr_num, android_dev_info.sz_dev_id,
		dev_all_info.usb_dev_info.sz_vid, dev_all_info.usb_dev_info.sz_pid, android_dev_info.sz_state, PRINT_POINT_STR, PRINT_END_STR);

	clear_usb_dev_data(&dev_all_info.usb_dev_info);

	operator_thread_work_count(DECREASE_TYPE);
}

static void process_android_work(vector<android_dev_info_stru>* p_vec_android_dev_info)
{
	if (p_vec_android_dev_info == NULL)
		return;

	uint32_t ret = CRG_FAIL;
	usb_dev_info_stru usb_dev_info;
	clear_usb_dev_data(&usb_dev_info);

	if (p_vec_android_dev_info->size() > 0)
	{
		for (uint32_t i = 0; i < p_vec_android_dev_info->size(); i++)
		{
			clear_usb_dev_data(&usb_dev_info);
			ret = push_identify_dev_data_to_list(ANDROID_DEVICE_TYPE, &(*p_vec_android_dev_info)[i], &usb_dev_info);
			if (ret == CRG_SUCCESS)
			{
				write_log("%s new android device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s%s%s", PRINT_ANDROID_STR,
					usb_dev_info.u8_board_type, usb_dev_info.sz_board_num, usb_dev_info.u8_board_port_num,
					usb_dev_info.u8_bus_num, usb_dev_info.u8_addr_num, (*p_vec_android_dev_info)[i].sz_dev_id,
					usb_dev_info.sz_vid, usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_FOUND_STR);
#if _POWER_CONTROL_TURN_ON_	
				write_log("%s device power on, because of it's a android device, board type: %d, board num: %s, port: %d, busnum: %d, addr: %d, device id: %s, vid: %s, pid: %s, state: %s%s%s", PRINT_ANDROID_STR, usb_dev_info.u8_board_type, usb_dev_info.sz_board_num, usb_dev_info.u8_board_port_num, usb_dev_info.u8_bus_num, usb_dev_info.u8_addr_num, (*p_vec_android_dev_info)[i].sz_dev_id, usb_dev_info.sz_vid, usb_dev_info.sz_pid, (*p_vec_android_dev_info)[i].sz_state, PRINT_POINT_STR, PRINT_START_STR);
				// power on
				power_on_control(&usb_dev_info, PRINT_ANDROID_STR);
#endif
				// push device to pool thread for install app
				push_new_android_to_thread_pool((void*)0x01);
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
		// not find android device
	}
	clear_usb_dev_data(&usb_dev_info);
}

static void* listen_android_work(void *arg)
{
	write_log("listen android work thread%s%s", PRINT_POINT_STR, PRINT_START_STR);
	
	long time_diff = 0;
	long found_dev_time = 0;
	uint32_t ret = CRG_FAIL;
	vector<android_dev_info_stru> vec_android_dev_info;
	
	operator_thread_work_count(INCREASE_TYPE);
	do 
	{
		get_found_android_dev_mask(found_dev_time);
		if (found_dev_time <= 0)	// not found device OR error
		{
			usleep(200*1000);
			continue;
		}
		struct timeval tv = {0};
		gettimeofday(&tv, NULL);

		time_diff = tv.tv_sec*1000000 + tv.tv_usec - found_dev_time;
		if(time_diff < 2*1000*1000)		
		{
			usleep(2*1000*1000 - time_diff);
		}

		clear_android_dev_data(&vec_android_dev_info);
		ret = enum_android_list(&vec_android_dev_info);
		if (ret == CRG_SUCCESS)
		{
			process_android_work(&vec_android_dev_info);
		}
		
	} while (is_thread_running());
	clear_android_dev_data(&vec_android_dev_info);

	operator_thread_work_count(DECREASE_TYPE);

	write_log("listen android work thread%s%s", PRINT_POINT_STR, PRINT_END_STR);

	return (void*)NULL;
}

static uint32_t create_listen_android_thread()
{
	uint32_t ret = CRG_FAIL;
	
	int err = pthread_create(&g_pthread_pid, NULL, listen_android_work, NULL);

	if (err == 0)
	{
		ret = CRG_SUCCESS;
	}
	else
	{
		write_log("%s create android listen thread, err: %s%s%s", PRINT_ANDROID_STR, strerror(err), PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	g_thread_inited = (ret == CRG_SUCCESS ? true : false);
	return ret;
} 

uint32_t run_android_model()
{
	uint32_t ret = CRG_FAIL;
	write_log("run android model%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if (create_listen_android_thread() == CRG_SUCCESS)
	{
		ret = CRG_SUCCESS;
		write_log("run android model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	}
	else
		write_log("run android model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);

	return ret;
}

uint32_t init_android_model()
{
	uint32_t ret = CRG_FAIL;
	g_inited = false;

	write_log("init android model%s%s", PRINT_POINT_STR, PRINT_START_STR);

	if(init_all_data_lock() == CRG_SUCCESS
		&& init_android_thread_pool(MAX_THREADPOOL_COUNT, false, NULL, process_android_install_work) == CRG_SUCCESS
		&& adb_start() == CRG_SUCCESS)
	{
		write_log("init android model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
		ret = CRG_SUCCESS;
	}
	else
	{
		destory_android_thread_pool();
		adb_close();
		destory_all_data_lock();
		write_log("init android model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);
	}

	g_inited = (ret == CRG_SUCCESS ? true : false);

	return ret;
}

void set_android_model_exit_status()
{
	set_thread_running_status(false);
}

void wait_android_model_exit()
{
	write_log("waiting android model stop work%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if(g_thread_inited) pthread_join(g_pthread_pid, NULL);
	g_thread_inited = false;
	destory_android_thread_pool();
	write_log("waiting android model stop work%s%s", PRINT_POINT_STR, PRINT_END_STR);
}

uint32_t destory_android_model()
{
	if (!g_inited)
		return CRG_SUCCESS;
	
	write_log("destory android model%s%s", PRINT_POINT_STR, PRINT_START_STR);

	adb_close();
	destory_all_data_lock();

	g_inited = false;
	
	write_log("destory android model%s%s", PRINT_POINT_STR, PRINT_OK_STR);

	return CRG_SUCCESS;
}
