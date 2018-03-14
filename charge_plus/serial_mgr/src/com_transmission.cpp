#include <stdio.h>
#include <string.h>
#include <string>
#include <pthread.h>


#include "com_transmission.h"

static com_helper*	gp_serial_port[MAX_COM_COUNT]={NULL};

static void enter_transmission_lock(uint8_t u8_com_index)
{
	pthread_mutex_lock(&gp_serial_port[u8_com_index]->transmission_mutex);
}

static void leave_transmission_lock(uint8_t u8_com_index)
{

	pthread_mutex_unlock(&gp_serial_port[u8_com_index]->transmission_mutex);
}

static uint32_t get_board_config_file_path(char* file_path, uint32_t len)
{
	if (file_path == NULL || len < MAX_PATH)
		return CRG_INVALID_PARAM;

	char sz_module_path[MAX_PATH] = { 0 };

	if (get_share_module_path(sz_module_path, MAX_PATH) != CRG_SUCCESS)
	{
		readlink("/proc/self/exe", sz_module_path, MAX_PATH);
	}

	if(strrchr(sz_module_path, '/') == NULL)
	{
		return CRG_NOT_FOUND;
	}

	strrchr(sz_module_path, '/')[0] = '\0';

	if(strrchr(sz_module_path, '/') == NULL)
	{
		return CRG_NOT_FOUND;
	}

	strrchr(sz_module_path, '/')[0] = '\0';

	memcpy(file_path, sz_module_path, MAX_PATH);
	sprintf(file_path, "%s/%s", file_path, BOARD_CONFIG_INI_FILE);
	
	return is_exist_file(file_path);
}

static uint32_t get_all_board_device_info_from_ini(uint8_t u8_board_type, vector<string>& vec_board_num, vector<uint8_t>& vec_port_num)
{
	int i = 1;
	uint32_t ret = CRG_FAIL;
	char sz_board_num[MAX_BUFF_LEN] = { 0 };
	uint8_t u8_com_port_num = 0;
	char sz_board_num_key[20] = { 0 };
	char sz_com_port_num_key[20] = { 0 };
	char sz_config_path[MAX_PATH] = { 0 };

	ret = get_board_config_file_path(sz_config_path, MAX_PATH);
	if (ret != CRG_FOUND)
		return ret;

	ret = CRG_FAIL;
	if (u8_board_type == CCKJ_BOARD_TYPE)
	{
		do
		{
			memset(sz_board_num_key, 0, 20);
			memset(sz_com_port_num_key, 0, 20);

			sprintf(sz_board_num_key, BOARD_NUM_KEY, i);
			sprintf(sz_com_port_num_key, COM_NUM_KEY, i);

			if (get_private_profile_string(CCKJ_BOARD_INFO_SECTION, sz_board_num_key, "", sz_board_num, MAX_BUFF_LEN, sz_config_path) > 0
					&& (u8_com_port_num = (uint8_t)get_private_profile_int(CCKJ_BOARD_INFO_SECTION, sz_com_port_num_key, 100, sz_config_path)) != 100
			   )
			{
				i++;
				vec_board_num.push_back(string(sz_board_num));
				vec_port_num.push_back(u8_com_port_num);
				ret = CRG_SUCCESS;
			}
			else
			{
				if (i <= 1)
				{
					// write_log("INI File name: %s file %s%s", sz_config_path, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
				}
				break;
			}
		} while (true);
	}

	return ret;
}

static uint32_t get_board_device_info_from_ini(uint8_t u8_board_type, uint32_t dwFindType, string & str_board_num, uint8_t & u8_board_port_num)
{
	int i = 1;
	uint32_t ret = CRG_FAIL;
	char sz_board_num[MAX_BUFF_LEN] = { 0 };
	uint8_t u8_com_port_num = 0;
	char sz_board_num_key[20] = { 0 };
	char sz_com_port_num_key[20] = { 0 };
	char sz_config_path[MAX_PATH] = { 0 };

	ret = get_board_config_file_path(sz_config_path, MAX_PATH);
	if (ret != CRG_FOUND)
		return ret;

	ret = CRG_FAIL;
	if (u8_board_type == CCKJ_BOARD_TYPE)
	{
		do
		{
			memset(sz_board_num_key, 0, 20);
			memset(sz_com_port_num_key, 0, 20);

			sprintf(sz_board_num_key, BOARD_NUM_KEY, i);
			sprintf(sz_com_port_num_key, COM_NUM_KEY, i);

			if (get_private_profile_string(CCKJ_BOARD_INFO_SECTION, sz_board_num_key, "", sz_board_num, MAX_BUFF_LEN, sz_config_path) > 0
					&& (u8_com_port_num = (uint8_t)get_private_profile_int(CCKJ_BOARD_INFO_SECTION, sz_com_port_num_key, 100, sz_config_path)) != 100
			   )
			{
				i++;
				if (dwFindType == 0)
				{
					if (strcasecmp(sz_board_num, str_board_num.c_str()) == 0)
					{
						ret = CRG_SUCCESS;
						u8_board_port_num = u8_com_port_num;
						break;
					}
				}
				else if (dwFindType == 1)
				{
					if (u8_board_port_num == u8_com_port_num)
					{
						ret = CRG_SUCCESS;
						str_board_num = string(sz_board_num);
						break;
					}
				}
			}
			else
			{
				if (i <= 1)
				{
					// write_log("INI File name: %s file %s%s", sz_config_path, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
				}
				break;
			}

		} while (true);
	}

	return ret;
}


static uint32_t get_board_device_com_num_from_ini(uint8_t u8_board_type, string str_board_num, uint8_t & u8_board_port_num)
{
	return get_board_device_info_from_ini(u8_board_type, 0, str_board_num, u8_board_port_num);
}

uint32_t get_board_device_usb_count_from_cmd_return(char* p_data, uint32_t dw_len, uint32_t &usb_count)
{
	if (dw_len != 6)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char query_ret_16port[6] = { 0x68, 0x02, 0x06, 0x01, 0x07, 0x16 };
	char query_ret_8port[6] = { 0x68, 0x02, 0x06, 0x02, 0x08, 0x16 };

	char no_support_ret[6] = { 0x68, 0x02, 0x01, 0x02, 0x03, 0x16 };
	char sz_cmd_fail_ret[6] =   { 0x68, 0x02, 0x01, 0x01, 0x02, 0x16 };

	if (strcasecmp(p_data, query_ret_8port) == 0)
	{
		usb_count = 8;
		ret = CRG_SUCCESS;
	}
	else if (strcasecmp(p_data, query_ret_16port) == 0)
	{
		usb_count = 16;
		ret = CRG_SUCCESS;
	}
	else if (strcasecmp(p_data, no_support_ret) == 0)
	{
		ret = CRG_NO_SUPPORT;
	}
	else if (strcasecmp(p_data, sz_cmd_fail_ret) == 0)
	{
		ret = CRG_NO_SUPPORT;
	}
	else
	{
		// device return data is wrong!
		ret = CRG_FAIL;
	}
	return ret;
}

static bool is_success_cmd_return(char* p_data, uint32_t dw_len)
{
	if (dw_len != 6)
		return false;

	char ack_ok[6] = { 0x68, 0x02, 0x01, 0x00, 0x01, 0x16 };
	//char ack_fail[6] = { 0x68, 0x02, 0x01, 0x01, 0x02, 0x16 };
	return memcmp(p_data, ack_ok, dw_len) == 0 ? true : false;
}

static uint32_t get_board_device_usb_count(uint8_t board_type, string str_board_num, uint8_t port_num, uint32_t &usb_count)
{
	uint32_t ret = CRG_FAIL;
	uint8_t u8_com_port_index = port_num;

	if (u8_com_port_index >= MAX_COM_COUNT)
		return CRG_INVALID_PARAM;

	write_log("COMDLL: board type: %d, board num: %s, get board device usb port count from com%d now.\n", board_type, str_board_num.c_str(), port_num);

	if (board_type == CCKJ_BOARD_TYPE)
	{
		int8_t p_data[MAX_SERIAL_BUFFER_LENGHT] = { 0x00, };
		uint32_t dw_data_len = MAX_SERIAL_BUFFER_LENGHT;
		int8_t* p = p_data;
		*p = 0x68;
		*(p += 2) = HW_QUERY_HUB_CMD;
		*(++p) = 0x00;
		*(p += 2) = 0x16;
		int8_t frame_len = p - p_data + 1;
		p_data[1] = frame_len - 4;
		check_sum((char *)p_data, frame_len, *(p - 1));

		enter_transmission_lock(u8_com_port_index);

		gp_serial_port[u8_com_port_index]->send_cmd((char *)p_data, frame_len);
		memset(p_data, 0, sizeof(p_data));
		ret = gp_serial_port[u8_com_port_index]->read_data((char *)p_data, dw_data_len);	

		if (ret == CRG_SUCCESS)
		{
			ret = get_board_device_usb_count_from_cmd_return((char*)p_data, dw_data_len, usb_count);
			if (ret == CRG_SUCCESS)
			{
				write_log("COMDLL: board type: %d, board num: %s, get board device usb port count is %d from com%d.\n", board_type, str_board_num.c_str(), usb_count, port_num);
			}
			else if (ret == CRG_NO_SUPPORT)
			{
				// board device is not support response data. default usb count 16.
				write_log("COMDLL: board type: %d, board num: %s, get board device usb port count from com%d, but it is not support request cmd. so default usb port count 16.\n", board_type, str_board_num.c_str(), port_num);
				usb_count = 16;
				ret = CRG_SUCCESS;
			}
			else if (ret == CRG_FAIL)
			{
				write_log("COMDLL: board type: %d, board num: %s, get board device usb port count from com%d, but board device return data is wrong. it's board bug.\n",
						board_type, str_board_num.c_str(), port_num);
			}
			else
			{
				write_log("COMDLL: board type: %d, board num: %s, get board device usb port count from com%d, but param is wrong. it's a bug.\n",
						board_type, str_board_num.c_str(), port_num);
			}
		}
		else
		{
			// board device not response data. default usb count 16.
			write_log("COMDLL: board type: %d, board num: %s, get board device usb port count from com%d, but there isn't return response data. so default usb port count 16.\n",
					board_type, str_board_num.c_str(), port_num);
			usb_count = 16;
			ret = CRG_SUCCESS;
		}

		leave_transmission_lock(u8_com_port_index);
	}

	return ret;
}

static uint32_t reset_board_hub(uint8_t board_type, uint8_t port_num, uint8_t hub_num)
{
	uint32_t dw_ret = CRG_FAIL;
	uint8_t u8_com_port_index = port_num;
	if (board_type == CCKJ_BOARD_TYPE)
	{
		if (u8_com_port_index >= MAX_COM_COUNT || hub_num >= BOARD_ALL_HUB_PORT_COUNT)
		{
			return CRG_INVALID_PARAM;
		}

		int8_t p_data[MAX_SERIAL_BUFFER_LENGHT] = { 0x00, };
		uint32_t dw_data_len = MAX_SERIAL_BUFFER_LENGHT;
		int8_t* p = p_data;
		*p = 0x68;
		*(p += 2) = HW_RESET_HUB_CMD;
		*(++p) = hub_num;
		*(p += 2) = 0x16;
		int8_t frame_len = p - p_data + 1;
		p_data[1] = frame_len - 4;
		check_sum((char *)p_data, frame_len, *(p - 1));
		
		enter_transmission_lock(u8_com_port_index);

		gp_serial_port[u8_com_port_index]->send_cmd((char *)p_data, frame_len);
		memset(p_data, 0, sizeof(p_data));
		dw_ret = gp_serial_port[u8_com_port_index]->read_data((char *)p_data, dw_data_len);	
		
		if (dw_ret == CRG_SUCCESS)
		{
			if(is_success_cmd_return((char *)p_data, dw_data_len))
				dw_ret = CRG_SUCCESS;
			else
				dw_ret = CRG_FAIL;
		}
		leave_transmission_lock(u8_com_port_index);
		//pthread_mutex_unlock(&commHelper->transmission_mutex);
	}

	return dw_ret;
}

static void pull_valid_board_root_hub_from_vector(string find_board_num, vector<string>& vec_board_num, vector<uint8_t>& vec_port_num)
{
	vector<string>::iterator itor;
	for (itor = vec_board_num.begin(); itor != vec_board_num.end();)
	{
		if (strcasecmp(find_board_num.c_str(), itor->c_str()) == 0)
		{
			vec_port_num.erase(vec_port_num.begin() + (itor - vec_board_num.begin()));
			vector<uint8_t>(vec_port_num).swap(vec_port_num);
			vec_board_num.erase(itor);
			vector<string>(vec_board_num).swap(vec_board_num);
			break;
		}

		itor++;
	}
}

static void get_invalid_board_root_hub(vector<string>& vec_board_num, vector<uint8_t>& vec_port_num)
{
	vector<board_device_info> vec_current_device_info;

	uint32_t ret_list = get_current_board_device_list(&vec_current_device_info);
	if (ret_list == CRG_SUCCESS)
	{
		for (uint32_t i = 0; i < vec_current_device_info.size(); i++)
		{
			string str_board_num;
			if (get_board_device_num_from_dev_path(vec_current_device_info[i].u8_board_type, vec_current_device_info[i].str_object, str_board_num) == CRG_SUCCESS)
			{
				pull_valid_board_root_hub_from_vector(str_board_num, vec_board_num, vec_port_num);
			}
		}
	}
	vec_current_device_info.clear();
	vector<board_device_info>(vec_current_device_info).swap(vec_current_device_info);
}


static uint32_t check_board_root_hub()
{
	uint32_t ret = CRG_FAIL;
	uint32_t try_count = 10;

	uint8_t u8_board_type = CCKJ_BOARD_TYPE;
	vector<string> vec_board_num;
	vector<uint8_t> vec_port_num;

	string::size_type board_read_conf_count = 0,board_invalid_count = 0;

	bool b_hub_ok = true;
	do
	{
		b_hub_ok = true;

		vec_board_num.clear();
		vector<string>(vec_board_num).swap(vec_board_num);
		vec_port_num.clear();
		vector<uint8_t>(vec_port_num).swap(vec_port_num);

		if (get_all_board_device_info_from_ini(u8_board_type, vec_board_num, vec_port_num) == CRG_SUCCESS)
		{
			board_read_conf_count = vec_board_num.size();
			get_invalid_board_root_hub(vec_board_num, vec_port_num);
			board_invalid_count = vec_board_num.size();
			for (uint32_t i = 0; i < vec_port_num.size(); i++)
			{
				b_hub_ok = false;
				write_log("COMDLL: board type: %d, board num: %s, it is invalid root hub, reset hub from com%d now.\n",
						u8_board_type, vec_board_num[i].c_str(), vec_port_num[i]);

				reset_board_hub(u8_board_type, vec_port_num[i], 0); // reset root hub! 
			}
		}
		else
		{
			b_hub_ok = false;
			write_log("COMDLL: check board root hub, read board config file error.\n");
		}

		if (b_hub_ok)
		{
			write_log("COMDLL: all root hub is OK.\n");
			ret = CRG_SUCCESS;
			break;
		}

		sleep(2);
		try_count--;
	} while (try_count > 0);

	//至少一个电路板可用即可启动charge程序 --change by zhzq @ 2016-5-26 17:07:16
	if((!b_hub_ok)&&(board_read_conf_count>board_invalid_count)) ret = CRG_SUCCESS;

	vec_board_num.clear();
	vector<string>(vec_board_num).swap(vec_board_num);
	vec_port_num.clear();
	vector<uint8_t>(vec_port_num).swap(vec_port_num);

	return ret;
}

static uint32_t check_board_child_hub()
{
	uint32_t ret = CRG_FAIL;
	uint32_t try_count = 10;
	vector<board_device_info> vec_current_device_info;
	bool b_hub_ok = true;
	do
	{
		b_hub_ok = true;
		vec_current_device_info.clear();
		vector<board_device_info>(vec_current_device_info).swap(vec_current_device_info);

		uint32_t ret_list = get_current_board_device_list(&vec_current_device_info);
		if (ret_list == CRG_SUCCESS)
		{
			for (uint32_t i = 0; i < vec_current_device_info.size(); i++)
			{
				if (vec_current_device_info[i].u8_board_type == CCKJ_BOARD_TYPE)
				{
					uint8_t u8_board_port_num = 0;
					string str_board_num;
					if (get_board_device_num_from_dev_path(vec_current_device_info[i].u8_board_type, vec_current_device_info[i].str_object, str_board_num) == CRG_SUCCESS
							&& get_board_device_com_num_from_ini(vec_current_device_info[i].u8_board_type, str_board_num, u8_board_port_num) == CRG_SUCCESS)
					{
						uint32_t usb_count = 0;
						uint32_t dw_board_hub_count = 0;

						// get board info, 8 OR 16 usb ports 
						if (get_board_device_usb_count(vec_current_device_info[i].u8_board_type, str_board_num, u8_board_port_num, usb_count) == CRG_SUCCESS)
						{
							dw_board_hub_count = usb_count / MAX_PER_PORT_COUNT + 1;
							// check child hub
							for (uint32_t m = 1; m < dw_board_hub_count; m++)
							{
								if (vec_current_device_info[i].dw_hub_port_num[m] == 0)
								{
									b_hub_ok = false;

									write_log("COMDLL: board type: %d, board num: %s, usb count: %d, hub num: %d, it is invalid child hub, reset hub from com%d now.\n",
											vec_current_device_info[i].u8_board_type, str_board_num.c_str(), (dw_board_hub_count - 1)*MAX_PER_PORT_COUNT, m, u8_board_port_num);

									reset_board_hub(vec_current_device_info[i].u8_board_type, u8_board_port_num, m);
								}
							}
						}
						else
						{
							b_hub_ok = false;
						}
					}
					else
					{
						b_hub_ok = false;
						write_log("COMDLL: check board child hub, board type: %d, boardnum: %s, read board config file error.\n",
								vec_current_device_info[i].u8_board_type, str_board_num.c_str());
					}
				}
				if (b_hub_ok) ret = CRG_SUCCESS;//add by zhzq @ 2016-7-28 11:07:19
			}
		}
		else
		{
			write_log("COMDLL: board type: %d, root hub, it is invalid hub OR not found, please check board device.\n", CCKJ_BOARD_TYPE);
			b_hub_ok = false;
		}

		if (b_hub_ok)
		{
			write_log("COMDLL: all child hub is OK.\n");
			ret = CRG_SUCCESS;
			break;
		}
		
		sleep(2);
		try_count--;
	} while (try_count > 0);

	
	
	vec_current_device_info.clear();
	vector<board_device_info>(vec_current_device_info).swap(vec_current_device_info);

	return ret;
}

uint32_t check_board_hub()
{
	return (check_board_root_hub() == CRG_SUCCESS && check_board_child_hub() == CRG_SUCCESS) ? CRG_SUCCESS : CRG_FAIL;
}

uint32_t control_mobile_client(uint8_t board_type, const char* board_num, uint8_t port_num, statu_control statu, uint8_t n_hight, uint8_t n_low)
{
	if (port_num >= MAX_BOARD_ALL_PORT_COUNT)
		return CRG_INVALID_PARAM;

	uint32_t dw_ret = CRG_FAIL;
	uint8_t u8_com_port_index = 0;
	if ((dw_ret = get_board_device_com_num_from_ini(board_type, string(board_num), u8_com_port_index)) != CRG_SUCCESS)
	{
		write_log("get connect com port num, board type: %d, board num: %s. please check board config file%s%s", board_type, board_num, PRINT_POINT_STR, PRINT_NOTFOUND_STR);
		return dw_ret;
	}
	dw_ret = CRG_FAIL;
	if (u8_com_port_index >= MAX_COM_COUNT)
		return dw_ret;

	//printf("u8_com_port_index is %d\n",u8_com_port_index);
	if (board_type == CCKJ_BOARD_TYPE)
	{
		//int cmd = 0;
		int8_t sum = 0;
		uint8_t p_data[MAX_SERIAL_BUFFER_LENGHT] = { 0x00, };// = { 0x68, 0x02, 0xff, 0xff, 0xff, 0x16 };
		uint32_t dw_data_len = MAX_SERIAL_BUFFER_LENGHT;
		uint8_t* p = p_data;
		*p = 0x68;
		p += 2;
		switch (statu)
		{
			case disconnect:
				*p = HW_POWER_OFF_CMD;
				*(++p) = port_num;
				*(++p) = n_low;
				*(++p) = n_hight;
				break;
			case connected:
				*p = HW_POWER_ON_CMD;
				*(++p) = port_num;
				break;
			default:
				break;
		}
		p += 2;
		*p = 0x16;
		int8_t frame_len = p - p_data + 1;
		check_sum((char *)p_data, frame_len, sum);
		*(p - 1) = sum;
		p_data[1] = frame_len - 4;

		enter_transmission_lock(u8_com_port_index);

		gp_serial_port[u8_com_port_index]->send_cmd((char *)p_data, frame_len);
		memset(p_data, 0, sizeof(p_data));
		dw_ret = gp_serial_port[u8_com_port_index]->read_data((char *)p_data, dw_data_len);	

		if (dw_ret == CRG_SUCCESS)
		{
			if(is_success_cmd_return((char *)p_data, dw_data_len))
				dw_ret = CRG_SUCCESS;
			else
				dw_ret = CRG_FAIL;
		}
		leave_transmission_lock(u8_com_port_index);
	}
	else
	{
	}

	return dw_ret;
}

uint32_t control_indicateor_light_client(uint8_t board_type, const char* board_num, uint8_t port_num, statu_control statu)
{
	if (port_num >= MAX_BOARD_ALL_PORT_COUNT)
		return CRG_INVALID_PARAM;

	uint32_t dw_ret = CRG_FAIL;
	uint8_t u8_com_port_index = 0;

	if ((dw_ret = get_board_device_com_num_from_ini(board_type, string(board_num), u8_com_port_index)) != CRG_SUCCESS)
	{
		write_log("get connect com port num, board type: %d, board num: %s. please check board config file%s%s", board_type, board_num, PRINT_POINT_STR, PRINT_NOTFOUND_STR);
		return dw_ret;
	}
	dw_ret = CRG_FAIL;
	if (u8_com_port_index >= MAX_COM_COUNT)
		return dw_ret;

	if (board_type == CCKJ_BOARD_TYPE)
	{
		int8_t sum = 0;
		uint8_t p_data[MAX_SERIAL_BUFFER_LENGHT] = { 0x00, };// = { 0x68, 0x03, 0x07, 0x00, 0x00, 0x17, 0x16 };
		uint32_t dw_data_len = MAX_SERIAL_BUFFER_LENGHT;
		uint8_t* p = p_data;
		*p = 0x68;
		*(p += 2) = HW_CONTROL_LIGHT_CMD;
		*(++p) = port_num;
		switch (statu)
		{
			case disconnect:
				*(++p) = HW_LIGHT_OFF_STATUS;
				break;
			case connected:
				*(++p) = HW_LIGHT_ON_STATUS;
				break;
			default:
				break;
		}
		p += 2;
		*p = 0x16;
		int8_t frame_len = p - p_data + 1;
		check_sum((char *)p_data, frame_len, sum);
		*(p - 1) = sum;
		p_data[1] = frame_len - 4;

		enter_transmission_lock(u8_com_port_index);
		
		gp_serial_port[u8_com_port_index]->send_cmd((char *)p_data, frame_len);
		memset(p_data, 0, sizeof(p_data));
		dw_ret = gp_serial_port[u8_com_port_index]->read_data((char *)p_data, dw_data_len);	

		if (dw_ret == CRG_SUCCESS)
		{
			if(is_success_cmd_return((char *)p_data, dw_data_len))
				dw_ret = CRG_SUCCESS;
			else
				dw_ret = CRG_FAIL;
		}
		leave_transmission_lock(u8_com_port_index);
		//pthread_mutex_unlock(&commHelper->transmission_mutex);

	}
	else
	{
	}

	return dw_ret;
}

void set_board_status(uint8_t fun)
{
	uint8_t u8_board_type = CCKJ_BOARD_TYPE;
	vector<string> vec_board_num;
	vector<uint8_t> vec_port_num;

	vec_board_num.clear();
	vec_port_num.clear();

	if (get_all_board_device_info_from_ini(u8_board_type, vec_board_num, vec_port_num) == CRG_SUCCESS)
	{
		for(uint32_t i=0;i<vec_board_num.size();i++)
		{
			for(uint8_t port_num=0;port_num<8;port_num++)
			{
				control_indicateor_light_client(u8_board_type, vec_board_num[i].c_str(), port_num, disconnect);
				control_mobile_client(u8_board_type, vec_board_num[i].c_str(), port_num, disconnect, HW_POWER_ON_TIME, HW_POWER_OFF_TIME);
			}
			
		}
		if(fun == INIT_BOARD_FUNC) write_log("init board status job done!\n");
		else if(fun == RESET_BOARD_FUNC)
		{
			for(uint32_t i=0;i<vec_board_num.size();i++)
			{
				for(uint32_t j=0;j<3;j++)
				{
					sleep(3);
					reset_board_hub(u8_board_type, vec_port_num[i], j);
				}
			}
			write_log("reset board status job done!\n");
		}
	}
	
	vec_board_num.clear();
	vector<string>(vec_board_num).swap(vec_board_num);
	vec_port_num.clear();
	vector<uint8_t>(vec_port_num).swap(vec_port_num);
	
}

uint32_t init_com_transmission()
{
	uint32_t ret = CRG_SUCCESS;
	char device[10] = {0};
	for (int i = 0; i < MAX_COM_COUNT; i++){
		gp_serial_port[i] = new com_helper();
		sprintf(device,"ttyS%d",i);
		if(gp_serial_port[i]->init_com(device, 9600) != CRG_SUCCESS)
		{
			write_log("init_com %s%s%s ", device, PRINT_POINT_STR, PRINT_FAIL_STR);
			ret = CRG_FAIL;
			break;
		}
	}
	set_board_status(INIT_BOARD_FUNC);
#if 0
	vector<string> vec_board_num;
	vector<uint32_t> vec_port_num;
	vec_board_num.clear();
	vec_port_num.clear();
	if (get_all_board_device_info_from_ini(CCKJ_BOARD_TYPE, vec_board_num, vec_port_num) == CRG_SUCCESS)
	{
		for(uint32_t i=0;i<vec_port_num.size();i++)
		{
			gp_serial_port[vec_port_num[i]] = new com_helper();
			sprintf(device,"ttyS%d",vec_port_num[i]);
			if(gp_serial_port[vec_port_num[i]]->init_com(device, 9600) != CRG_SUCCESS)
			{
				write_log("init_com %s%s%s ", device, PRINT_POINT_STR, PRINT_FAIL_STR);
				ret = CRG_FAIL;
				break;
			}
		}
	}
	else
	{
		ret = CRG_FAIL;
		write_log("init_com failed case by read board config file error.\n");
	}
	vec_board_num.clear();
	vector<string>(vec_board_num).swap(vec_board_num);
	vec_port_num.clear();
	vector<uint32_t>(vec_port_num).swap(vec_port_num);
#endif
	if (ret == CRG_SUCCESS)
		ret = check_board_hub();
	return ret;
}

void set_com_transmission_exit_status()
{
	for(uint32_t i=0;i<MAX_COM_COUNT;i++) 
	{
		if(gp_serial_port[i] != NULL)
			gp_serial_port[i]->set_thread_running_status(false);
	}
}

void wait_com_transmission_exit()
{
	for(uint32_t i=0;i<MAX_COM_COUNT;i++) 
	{
		if((gp_serial_port[i]!=NULL)&&gp_serial_port[i]->g_thread_inited)
		{
			pthread_join(gp_serial_port[i]->g_pthread_pid, NULL);
			gp_serial_port[i]->g_thread_inited = false;
		}
	}
}

uint32_t destory_com_transmission()
{
	for(uint32_t i=0;i<MAX_COM_COUNT;i++){
 		if(gp_serial_port[i] != NULL)
			delete gp_serial_port[i];
	}
	return CRG_SUCCESS;
}
