#include <pthread.h> 
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "device_data.h"

static pthread_mutex_t							g_data_mutex;
static vector<dev_all_info_stru>				g_vec_dev_all_info;
static bool										g_inited = false;

static uint32_t init_dev_data_lock()
{
	return pthread_mutex_init(&g_data_mutex, NULL) == 0 ? CRG_SUCCESS : CRG_FAIL;
}

static uint32_t destory_dev_data_lock()
{
	return pthread_mutex_destroy(&g_data_mutex) == 0 ? CRG_SUCCESS : CRG_FAIL;
}

static void enter_dev_data_lock()
{
	pthread_mutex_lock(&g_data_mutex);
}

static void leave_dev_data_lock()
{
	pthread_mutex_unlock(&g_data_mutex);
}

static uint32_t clear_identify_dev_data(void* &identify_dev_info)
{
	if (identify_dev_info == NULL)
		return CRG_INVALID_PARAM;

	free(identify_dev_info);
	identify_dev_info = NULL;

	return CRG_SUCCESS;
}

static uint32_t clear_dev_data_for_list()
{
	enter_dev_data_lock();
	while (g_vec_dev_all_info.size() > 0)
	{
		clear_dev_data(&g_vec_dev_all_info[0]);
		g_vec_dev_all_info.erase(g_vec_dev_all_info.begin());
	}
	vector<dev_all_info_stru>(g_vec_dev_all_info).swap(g_vec_dev_all_info);
	leave_dev_data_lock();
	return CRG_SUCCESS;
}

uint32_t init_dev_data_model()
{
	uint32_t ret = CRG_FAIL;
	write_log("init device data model%s%s", PRINT_POINT_STR, PRINT_START_STR);
	ret = init_dev_data_lock();
	if (ret == CRG_SUCCESS)
		write_log("init device data model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	else
		write_log("init device data model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);

	g_inited = (ret == CRG_SUCCESS ? true : false);
	return ret;
}

uint32_t destory_dev_data_model()
{
	if (!g_inited)
		return CRG_SUCCESS;

	write_log("destory device data model%s%s", PRINT_POINT_STR, PRINT_START_STR);

	uint32_t ret = CRG_FAIL;
	
	clear_dev_data_for_list();
	ret = destory_dev_data_lock();
	// free g_vec_dev_all_info
	if (ret == CRG_SUCCESS)
		write_log("destory device data model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	else
		write_log("destory device data model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);
	return ret;
}

uint32_t clear_usb_dev_data(usb_dev_info_stru* usb_dev_info)
{
	if (usb_dev_info == NULL)
		return CRG_INVALID_PARAM;
	while( usb_dev_info->vec_dev_id.size())
	{
		usb_dev_info->vec_dev_id.pop_back();
		usb_dev_info->vec_object.pop_back();
	}
	
	vector<string>(usb_dev_info->vec_dev_id).swap(usb_dev_info->vec_dev_id);
	vector<string>(usb_dev_info->vec_object).swap(usb_dev_info->vec_object);
	memset(usb_dev_info, 0, sizeof(usb_dev_info_stru)-sizeof(vector<string>) - sizeof(vector<string>));

	return CRG_SUCCESS;
}

uint32_t clear_dev_data(dev_all_info_stru* dev_all_info)
{
	if (dev_all_info == NULL)
		return CRG_INVALID_PARAM;

	dev_all_info->dw_dev_type = UNKNOWN_DEVICE_TYPE;
	clear_usb_dev_data(&dev_all_info->usb_dev_info);
	clear_identify_dev_data(dev_all_info->p_identify_dev_info);

	return CRG_SUCCESS;
}

uint32_t clear_identify_dev_data_from_list(uint32_t data_index)
{
	uint32_t ret = CRG_FAIL;
	enter_dev_data_lock();
	if (data_index <= g_vec_dev_all_info.size())
	{
		ret = clear_identify_dev_data(g_vec_dev_all_info[data_index].p_identify_dev_info);
	}
	leave_dev_data_lock();
	return ret;
}

uint32_t get_new_identify_dev_data_to_list(uint32_t dev_type, dev_all_info_stru* p_out_dev_all_info)
{
	if ((dev_type != ANDROID_DEVICE_TYPE && dev_type != IOS_DEVICE_TYPE) || p_out_dev_all_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t i = 0;
	uint32_t ret = CRG_NOT_FOUND;

	enter_dev_data_lock();
	
	for (i = 0; i < g_vec_dev_all_info.size(); i++)
	{
		if (g_vec_dev_all_info[i].dw_dev_type == dev_type
			&& g_vec_dev_all_info[i].p_identify_dev_info != NULL)
		{
			if (dev_type == ANDROID_DEVICE_TYPE)
			{
				if (((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->dw_proc_state == PROC_IDLE_STATE)
				{
					p_out_dev_all_info->dw_dev_type = g_vec_dev_all_info[i].dw_dev_type;
					memcpy(&p_out_dev_all_info->usb_dev_info, &g_vec_dev_all_info[i].usb_dev_info, sizeof(usb_dev_info_stru) -sizeof(vector<string>) - sizeof(vector<string>));
					p_out_dev_all_info->usb_dev_info.vec_dev_id = g_vec_dev_all_info[i].usb_dev_info.vec_dev_id;
					p_out_dev_all_info->usb_dev_info.vec_object = g_vec_dev_all_info[i].usb_dev_info.vec_object;
					memcpy(p_out_dev_all_info->p_identify_dev_info, g_vec_dev_all_info[i].p_identify_dev_info, sizeof(android_dev_info_stru));
					((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->dw_proc_state = PROC_BUSY_STATE;
					ret = CRG_FOUND;
					break;
				}
			}
			else
			{
				if (((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->dw_proc_state == PROC_IDLE_STATE)
				{
					p_out_dev_all_info->dw_dev_type = g_vec_dev_all_info[i].dw_dev_type;
					memcpy(&p_out_dev_all_info->usb_dev_info, &g_vec_dev_all_info[i].usb_dev_info, sizeof(usb_dev_info_stru) -sizeof(vector<string>) - sizeof(vector<string>));
					p_out_dev_all_info->usb_dev_info.vec_dev_id = g_vec_dev_all_info[i].usb_dev_info.vec_dev_id;
					p_out_dev_all_info->usb_dev_info.vec_object = g_vec_dev_all_info[i].usb_dev_info.vec_object;
					memcpy(p_out_dev_all_info->p_identify_dev_info, g_vec_dev_all_info[i].p_identify_dev_info, sizeof(ios_dev_info_stru));
					((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->dw_proc_state = PROC_BUSY_STATE;
					ret = CRG_FOUND;
					break;
				}
			}
		}
	}

	leave_dev_data_lock();

	return ret;
}

uint32_t find_num_from_string(string src_str, string & out_str)
{
	if (src_str.length() == 0)
		return CRG_INVALID_PARAM;

	for (uint32_t i = 0; i < src_str.length(); i++)
	{
		if (*(src_str.c_str() + i) >= '0' && *(src_str.c_str() + i) <= '9')
			out_str += *(src_str.c_str() + i);
	}
	return out_str.length() > 0 ? CRG_FOUND : CRG_NOT_FOUND;
}

bool check_usb_position_compare(string src_dev_path, uint8_t dest_bus_num, char* dest_port_num, uint32_t dest_port_num_len)
{
	if (src_dev_path.length() < 6 || dest_port_num == NULL || dest_port_num_len == 0 || dest_port_num_len > MAX_PORT_PATH_LEN)
		return false;

	bool ret = false;
	char bus_num[4] = { 0 };
	// char port_num[MAX_PORT_PATH_LEN] = { 0 };
	string str_num_path;

	find_num_from_string(src_dev_path, str_num_path);
	
	if (str_num_path.length() > 0)
	{
		snprintf(bus_num, sizeof(bus_num), "%d", dest_bus_num);

	// 	convert_uint_to_string(port_num, sizeof(port_num), dest_port_num, dest_port_num_len);

		if ((str_num_path.c_str())[0] == bus_num[0] 
			&& strcasecmp(str_num_path.c_str() + 1, dest_port_num) == 0)
		{
			ret = true;
		}
	}

	return ret;
}

uint32_t update_identify_dev_data_to_list(uint32_t dev_type, void* identify_dev_info)
{
	if (dev_type != ANDROID_DEVICE_TYPE && dev_type != IOS_DEVICE_TYPE)
		return CRG_INVALID_PARAM;

	uint32_t i = 0;
	uint32_t ret = CRG_NOT_FOUND;
	bool bfind = false;
	char vid[MAX_BUFF_LEN] = { 0 };
	char dev_id[MAX_BUFF_LEN] = { 0 };

	enter_dev_data_lock();
	if (dev_type == ANDROID_DEVICE_TYPE)
	{
		memcpy(dev_id, ((android_dev_info_stru*)identify_dev_info)->sz_dev_id, sizeof(((android_dev_info_stru*)identify_dev_info)->sz_dev_id));

		for (i = 0; i < g_vec_dev_all_info.size(); i++)
		{
			bool compare = check_usb_position_compare(((android_dev_info_stru*)identify_dev_info)->sz_path,
				g_vec_dev_all_info[i].usb_dev_info.u8_bus_num,
				g_vec_dev_all_info[i].usb_dev_info.sz_port_num, strlen(g_vec_dev_all_info[i].usb_dev_info.sz_port_num));

			if (!compare) // test point!
				continue;

			for (uint32_t j = 0; j < g_vec_dev_all_info[i].usb_dev_info.vec_dev_id.size(); j++)
			{
				if (strcasecmp(dev_id, g_vec_dev_all_info[i].usb_dev_info.vec_dev_id[j].c_str()) == 0)
				{
					bfind = true;
					break;
				}
			}
			if (bfind)
				break;
		}
		if (bfind)
		{
			if (g_vec_dev_all_info[i].p_identify_dev_info != NULL)
			{
				memcpy(((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_name, ((android_dev_info_stru*)identify_dev_info)->sz_name, sizeof(((android_dev_info_stru*)identify_dev_info)->sz_name));
				memcpy(((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_brand, ((android_dev_info_stru*)identify_dev_info)->sz_brand, sizeof(((android_dev_info_stru*)identify_dev_info)->sz_brand));
				memcpy(((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_model, ((android_dev_info_stru*)identify_dev_info)->sz_model, sizeof(((android_dev_info_stru*)identify_dev_info)->sz_model));
				memcpy(((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_os_version, ((android_dev_info_stru*)identify_dev_info)->sz_os_version, sizeof(((android_dev_info_stru*)identify_dev_info)->sz_os_version));
				memcpy(((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_state, ((android_dev_info_stru*)identify_dev_info)->sz_state, sizeof(((android_dev_info_stru*)identify_dev_info)->sz_state));
				((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->b_access = ((android_dev_info_stru*)identify_dev_info)->b_access;
				ret = CRG_SUCCESS;
			}
			else
				ret = CRG_NOT_FOUND;
		}
	}
	else
	{
		memcpy(vid, ((ios_dev_info_stru*)identify_dev_info)->sz_vid, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_vid));
		memcpy(dev_id, ((ios_dev_info_stru*)identify_dev_info)->sz_unique_dev_id, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_unique_dev_id));

		for (i = 0; i < g_vec_dev_all_info.size(); i++)
		{
			if (strcasecmp(vid, g_vec_dev_all_info[i].usb_dev_info.sz_vid) != 0)
				continue;

			for (uint32_t j = 0; j < g_vec_dev_all_info[i].usb_dev_info.vec_dev_id.size(); j++)
			{
				if (strcasecmp(dev_id, g_vec_dev_all_info[i].usb_dev_info.vec_dev_id[j].c_str()) == 0)
				{
					bfind = true;
					break;
				}
			}
			if (bfind)
				break;
		}
		if (bfind)
		{
			if (g_vec_dev_all_info[i].p_identify_dev_info != NULL)
			{
				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_name, ((ios_dev_info_stru*)identify_dev_info)->sz_name, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_name));
				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_serial_no, ((ios_dev_info_stru*)identify_dev_info)->sz_serial_no, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_serial_no));
				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_product_type, ((ios_dev_info_stru*)identify_dev_info)->sz_product_type, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_product_type));
				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_product_version, ((ios_dev_info_stru*)identify_dev_info)->sz_product_version, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_product_version));
				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_phone_number, ((ios_dev_info_stru*)identify_dev_info)->sz_phone_number, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_phone_number));

				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_icc_id, ((ios_dev_info_stru*)identify_dev_info)->sz_icc_id, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_icc_id));
				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_imei, ((ios_dev_info_stru*)identify_dev_info)->sz_imei, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_imei));
				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_state, ((ios_dev_info_stru*)identify_dev_info)->sz_state, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_state));
			
				((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->b_access = ((ios_dev_info_stru*)identify_dev_info)->b_access;

				ret = CRG_SUCCESS;
			}
			else
				ret = CRG_NOT_FOUND;
		}
	}
	leave_dev_data_lock();

	return ret;
}

uint32_t push_identify_dev_data_to_list(uint32_t dev_type, void* identify_dev_info, usb_dev_info_stru* out_usb_dev_info)
{
	if (dev_type != ANDROID_DEVICE_TYPE && dev_type != IOS_DEVICE_TYPE)
		return CRG_INVALID_PARAM;

	uint32_t i = 0;
	uint32_t ret = CRG_NOT_FOUND;
	bool bfind = false;
	char vid[MAX_BUFF_LEN] = { 0 };
	char dev_id[MAX_BUFF_LEN] = { 0 };

	enter_dev_data_lock();
	if (dev_type == ANDROID_DEVICE_TYPE)
	{
		memcpy(dev_id, ((android_dev_info_stru*)identify_dev_info)->sz_dev_id, sizeof(((android_dev_info_stru*)identify_dev_info)->sz_dev_id));
				
		for (i = 0; i < g_vec_dev_all_info.size(); i++)
		{
			bool compare = check_usb_position_compare(((android_dev_info_stru*)identify_dev_info)->sz_path, 
				g_vec_dev_all_info[i].usb_dev_info.u8_bus_num, 
				g_vec_dev_all_info[i].usb_dev_info.sz_port_num, strlen(g_vec_dev_all_info[i].usb_dev_info.sz_port_num));

			if (!compare) // test point!
				continue;

			for (uint32_t j = 0; j < g_vec_dev_all_info[i].usb_dev_info.vec_dev_id.size(); j++)
			{
				if (strcasecmp(dev_id, g_vec_dev_all_info[i].usb_dev_info.vec_dev_id[j].c_str()) == 0)
				{
					bfind = true;
					break;
				}
			}
			if (bfind)
				break;
		}
		if (bfind)
		{
			if (g_vec_dev_all_info[i].p_identify_dev_info == NULL)
			{
				g_vec_dev_all_info[i].dw_dev_type = dev_type;
				g_vec_dev_all_info[i].p_identify_dev_info = malloc(sizeof(android_dev_info_stru));
				memcpy((void*)g_vec_dev_all_info[i].p_identify_dev_info, (void*)identify_dev_info, sizeof(android_dev_info_stru));
				memcpy(((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_pid, g_vec_dev_all_info[i].usb_dev_info.sz_pid, sizeof(g_vec_dev_all_info[i].usb_dev_info.sz_pid));
				memcpy(((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_vid, g_vec_dev_all_info[i].usb_dev_info.sz_vid, sizeof(g_vec_dev_all_info[i].usb_dev_info.sz_pid));
				memcpy(((android_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_plugin_times, g_vec_dev_all_info[i].usb_dev_info.sz_plugin_times, sizeof(g_vec_dev_all_info[i].usb_dev_info.sz_plugin_times));

				if (out_usb_dev_info != NULL)
				{
					memcpy((void*)out_usb_dev_info, (void*)&g_vec_dev_all_info[i].usb_dev_info, sizeof(usb_dev_info_stru)-sizeof(vector<string>) - sizeof(vector<string>));
					out_usb_dev_info->vec_dev_id = g_vec_dev_all_info[i].usb_dev_info.vec_dev_id;
					out_usb_dev_info->vec_object = g_vec_dev_all_info[i].usb_dev_info.vec_object;
				}

				ret = CRG_SUCCESS;
			}
			else
				ret = CRG_FOUND;
		}
	}
	else
	{
		memcpy(vid, ((ios_dev_info_stru*)identify_dev_info)->sz_vid, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_vid));
		memcpy(dev_id, ((ios_dev_info_stru*)identify_dev_info)->sz_unique_dev_id, sizeof(((ios_dev_info_stru*)identify_dev_info)->sz_unique_dev_id));
	
		for (i = 0; i < g_vec_dev_all_info.size(); i++)
		{
			if (strcasecmp(vid, g_vec_dev_all_info[i].usb_dev_info.sz_vid) != 0)
				continue;

			for (uint32_t j = 0; j < g_vec_dev_all_info[i].usb_dev_info.vec_dev_id.size(); j++)
			{
				if (strcasecmp(dev_id, g_vec_dev_all_info[i].usb_dev_info.vec_dev_id[j].c_str()) == 0)
				{
					bfind = true;
					break;
				}
			}
			if (bfind)
				break;
		}
		if (bfind)
		{
			if (g_vec_dev_all_info[i].p_identify_dev_info == NULL)
			{
				g_vec_dev_all_info[i].dw_dev_type = dev_type;
				g_vec_dev_all_info[i].p_identify_dev_info = malloc(sizeof(ios_dev_info_stru));
				memcpy((void*)g_vec_dev_all_info[i].p_identify_dev_info, (void*)identify_dev_info, sizeof(ios_dev_info_stru));

				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_pid, g_vec_dev_all_info[i].usb_dev_info.sz_pid, sizeof(g_vec_dev_all_info[i].usb_dev_info.sz_pid));
				memcpy(((ios_dev_info_stru*)(g_vec_dev_all_info[i].p_identify_dev_info))->sz_plugin_times, g_vec_dev_all_info[i].usb_dev_info.sz_plugin_times, sizeof(g_vec_dev_all_info[i].usb_dev_info.sz_plugin_times));

				if (out_usb_dev_info != NULL)
				{
					memcpy((void*)out_usb_dev_info, (void*)&g_vec_dev_all_info[i].usb_dev_info, sizeof(usb_dev_info_stru)-sizeof(vector<string>) - sizeof(vector<string>));
					out_usb_dev_info->vec_dev_id = g_vec_dev_all_info[i].usb_dev_info.vec_dev_id;
					out_usb_dev_info->vec_object = g_vec_dev_all_info[i].usb_dev_info.vec_object;
				}

				ret = CRG_SUCCESS;
			}
			else
				ret = CRG_FOUND;
		}
	}
	leave_dev_data_lock();

	return ret;
}


uint32_t push_usb_dev_data_to_list(usb_dev_info_stru* usb_dev_info)
{
	if (usb_dev_info == NULL)
		return CRG_INVALID_PARAM;

	dev_all_info_stru dev_all_info = {0};
	dev_all_info.dw_dev_type = UNKNOWN_DEVICE_TYPE;
	dev_all_info.p_identify_dev_info = NULL;
	memcpy((void*)&dev_all_info.usb_dev_info, (void*)usb_dev_info, sizeof(usb_dev_info_stru)-sizeof(vector<string>) - sizeof(vector<string>));
	dev_all_info.usb_dev_info.vec_dev_id = usb_dev_info->vec_dev_id;
	dev_all_info.usb_dev_info.vec_object = usb_dev_info->vec_object;
	enter_dev_data_lock();
	g_vec_dev_all_info.push_back(dev_all_info);
	leave_dev_data_lock();

	return CRG_SUCCESS;
}

static uint32_t find_dev_data_index_1(uint8_t bus_num, char* port_num, uint32_t port_num_len, string vid, string pid, uint32_t &out_data_index)
{
	uint32_t ret = CRG_NOT_FOUND;
	//char sz_port_num[MAX_PORT_PATH_LEN] = { 0 };
	
	for (uint32_t i = 0; i < g_vec_dev_all_info.size(); i++)
	{
		// convert_uint_to_string(sz_port_num, sizeof(sz_port_num), port_num, port_num_len);

		if (bus_num == g_vec_dev_all_info[i].usb_dev_info.u8_bus_num 
			&& strcasecmp(port_num, g_vec_dev_all_info[i].usb_dev_info.sz_port_num) == 0
			&& strcasecmp(vid.c_str(), g_vec_dev_all_info[i].usb_dev_info.sz_vid) == 0
			&& strcasecmp(pid.c_str(), g_vec_dev_all_info[i].usb_dev_info.sz_pid) == 0)
		{
			out_data_index = i;
			ret = CRG_FOUND;
			// found it 
			break;
		}
	}

	return ret;
}

uint32_t is_exist_identify_dev_data_from_list_and_usb(uint32_t dev_type)
{
	if (dev_type != ANDROID_DEVICE_TYPE && dev_type != IOS_DEVICE_TYPE)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_NOT_FOUND;

	enter_dev_data_lock();
	for (uint32_t i = 0; i < g_vec_dev_all_info.size(); i++)
	{
		if (g_vec_dev_all_info[i].dw_dev_type == dev_type)
		{
			// found it 
			string str;
			char sz_cmd[MAX_BUFF_LEN] = { 0 };
			snprintf(sz_cmd, sizeof(sz_cmd), "lsusb | grep \"%s:%s\"", g_vec_dev_all_info[i].usb_dev_info.sz_vid, g_vec_dev_all_info[i].usb_dev_info.sz_pid);
			if (run_cmd(sz_cmd, str) == CRG_SUCCESS)
			{
				if (str.find(g_vec_dev_all_info[i].usb_dev_info.sz_vid) != string::npos
					&& str.find(g_vec_dev_all_info[i].usb_dev_info.sz_pid) != string::npos)
				{
					ret = CRG_FOUND;
					break;
				}
			}
		}
	}
	leave_dev_data_lock();

	return ret;
}

uint32_t is_exist_identify_dev_data_from_list(uint32_t dev_type)
{
	if (dev_type != ANDROID_DEVICE_TYPE && dev_type != IOS_DEVICE_TYPE)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_NOT_FOUND;
	enter_dev_data_lock();
	for (uint32_t i = 0; i < g_vec_dev_all_info.size(); i++)
	{
		if (g_vec_dev_all_info[i].dw_dev_type == dev_type)
		{
			// found it 
			ret = CRG_FOUND;
			break;
		}
	}
	leave_dev_data_lock();
	return ret;
}

uint32_t find_dev_data_index_from_list(usb_dev_info_stru* in_usb_dev_info)
{
	uint32_t ret = CRG_NOT_FOUND;
	
	enter_dev_data_lock();
	for (uint32_t i = 0; i < g_vec_dev_all_info.size(); i++)
	{
		if (in_usb_dev_info->u8_bus_num == g_vec_dev_all_info[i].usb_dev_info.u8_bus_num
			&& in_usb_dev_info->u8_board_type == g_vec_dev_all_info[i].usb_dev_info.u8_board_type
			&& in_usb_dev_info->u8_board_port_num == g_vec_dev_all_info[i].usb_dev_info.u8_board_port_num
			&& memcmp(in_usb_dev_info->sz_board_num, g_vec_dev_all_info[i].usb_dev_info.sz_board_num, sizeof(in_usb_dev_info->sz_board_num)) == 0
			&& memcmp(in_usb_dev_info->sz_vid, g_vec_dev_all_info[i].usb_dev_info.sz_vid, sizeof(in_usb_dev_info->sz_vid)) == 0
			&& memcmp(in_usb_dev_info->sz_pid, g_vec_dev_all_info[i].usb_dev_info.sz_pid, sizeof(in_usb_dev_info->sz_pid)) == 0)
		{
		//	out_data_index = i;
			ret = CRG_FOUND;
			// found it 
			break;
		}
	}
	leave_dev_data_lock();

	return ret;
}

uint32_t get_usb_dev_data_from_list(uint8_t bus_num, char* port_num, uint32_t port_num_len, string vid, string pid, usb_dev_info_stru* out_usb_dev_info)
{
	if (port_num == NULL || port_num_len == 0 || vid.length() == 0 || pid.length() == 0 || out_usb_dev_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	uint32_t data_index = 0;

	enter_dev_data_lock();

	ret = find_dev_data_index_1(bus_num, port_num, port_num_len, vid, pid, data_index);
	if (ret == CRG_FOUND)
	{
		memcpy(out_usb_dev_info, &g_vec_dev_all_info[data_index].usb_dev_info, sizeof(usb_dev_info_stru)-sizeof(vector<string>)-sizeof(vector<string>));
		out_usb_dev_info->vec_dev_id = g_vec_dev_all_info[data_index].usb_dev_info.vec_dev_id;
		out_usb_dev_info->vec_object = g_vec_dev_all_info[data_index].usb_dev_info.vec_object;
		ret = CRG_SUCCESS;
	}
	else
		ret = CRG_FAIL;

	leave_dev_data_lock();

	return ret;
}

uint32_t get_dev_type_data_from_list(uint8_t bus_num, char* port_num, uint32_t port_num_len, string vid, string pid, uint32_t &out_dev_type)
{	
	if (port_num == NULL || vid.length() == 0 || pid.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	uint32_t data_index = 0;

	enter_dev_data_lock();

	ret = find_dev_data_index_1(bus_num, port_num, port_num_len, vid, pid, data_index);
	if (ret == CRG_FOUND)
	{
		out_dev_type = g_vec_dev_all_info[data_index].dw_dev_type;
		ret = CRG_SUCCESS;
	}
	else
		ret = CRG_FAIL;

	leave_dev_data_lock();

	return ret;
}

uint32_t get_dev_position_data_from_list(uint8_t bus_num, char* port_num, uint32_t port_num_len, string vid, string pid, uint8_t & out_board_type, string & out_board_num, uint8_t & out_board_port_num)
{
	if (port_num == NULL || vid.length() == 0 || pid.length() == 0)
		return CRG_INVALID_PARAM;
	
	uint32_t ret = CRG_FAIL;
	uint32_t data_index = 0;

	enter_dev_data_lock();
	ret = find_dev_data_index_1(bus_num, port_num, port_num_len, vid, pid, data_index);
	if (ret == CRG_FOUND)
	{
		out_board_type = g_vec_dev_all_info[data_index].usb_dev_info.u8_board_type;
		out_board_port_num = g_vec_dev_all_info[data_index].usb_dev_info.u8_board_port_num;
		out_board_num = g_vec_dev_all_info[data_index].usb_dev_info.sz_board_num;
		ret = CRG_SUCCESS;
	}
	leave_dev_data_lock();

	return ret;
}

uint32_t pull_dev_data_from_list(uint8_t bus_num, char* port_num, uint32_t port_num_len, string vid, string pid)
{
	if (port_num == NULL || vid.length() == 0 || pid.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	uint32_t data_index = 0;

	enter_dev_data_lock();
	ret = find_dev_data_index_1(bus_num, port_num, port_num_len, vid, pid, data_index);
	if (ret == CRG_FOUND)
	{
		clear_dev_data(&g_vec_dev_all_info[data_index]);
		g_vec_dev_all_info.erase(g_vec_dev_all_info.begin() + data_index);
		vector<dev_all_info_stru>(g_vec_dev_all_info).swap(g_vec_dev_all_info);
		ret = CRG_SUCCESS;
	}
	else
		ret = CRG_FAIL;
	leave_dev_data_lock();
	return ret;
}
