#include <pthread.h> 
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "device_data.h"
#include "usb_action_data.h"

using namespace std;

static bool										g_usb_action_data_lock_inited = false;
static pthread_mutex_t							g_usb_action_data_mutex;
static vector<usb_action_stru>					g_vec_usb_action;

static void enter_usb_action_data_lock()
{
	pthread_mutex_lock(&g_usb_action_data_mutex);
}

static void leave_usb_action_data_lock()
{
	pthread_mutex_unlock(&g_usb_action_data_mutex);
}

static uint32_t find_usb_action_data(string dev_path, string vid, string pid, uint32_t & out_data_index)
{
	uint32_t ret = CRG_NOT_FOUND;

	for (uint32_t i = 0; i < g_vec_usb_action.size(); i++)
	{
		if (strcasecmp((const char*)g_vec_usb_action[i].sz_dev_path, dev_path.c_str()) == 0
			&& strcasecmp(g_vec_usb_action[i].sz_vid, vid.c_str()) == 0)
		{
			// found it
			out_data_index = i;
			// action_list = g_vec_usb_action[i].action_list;
			ret = CRG_SUCCESS;
		}
	}

	return ret;
}

static void printf_usb_action_data_for_list()
{
	char tmp_pid[MAX_BUFF_LEN] = { 0 };
	write_log("%s current usb action list data:\n", PRINT_USB_STR);
	for (uint32_t i = 0; i < g_vec_usb_action.size(); i++)
	{
		memset(tmp_pid, 0, sizeof(tmp_pid));
		if (strcmp(g_vec_usb_action[i].sz_sz_pid[1], "") != 0)
			snprintf(tmp_pid, sizeof(tmp_pid), "%s %s", g_vec_usb_action[i].sz_sz_pid[0], g_vec_usb_action[i].sz_sz_pid[1]);
		else
			snprintf(tmp_pid, sizeof(tmp_pid), "%s", g_vec_usb_action[i].sz_sz_pid[0]);
		
		write_log("vid: %s, pid: %s, dev path: %s, action: %d %d\n", 
			g_vec_usb_action[i].sz_vid, tmp_pid, (char*)g_vec_usb_action[i].sz_dev_path, g_vec_usb_action[i].sz_action_list[0], g_vec_usb_action[i].sz_action_list[1]);
	}
}

static uint32_t push_usb_action_data(void *dev, string dev_path, string vid, string pid, usb_dev_info_stru* p_usb_dev_info, uint8_t action)
{
	if (dev == NULL || dev_path.length() <= 0 || vid.length() <= 0 || pid.length() <= 0 || (action != ACTION_PLUG_IN && action != ACTION_UNPLUG))
		return CRG_INVALID_PARAM;

	usb_action_stru usb_action = { 0 };
	clear_usb_dev_data(&usb_action.usb_dev_info);

	usb_action.p_dev = dev;
	memcpy(usb_action.sz_dev_path, dev_path.c_str(), dev_path.length());
	memcpy(usb_action.sz_vid, vid.c_str(), vid.length());
	memcpy(usb_action.sz_sz_pid[0], pid.c_str(), pid.length());
	usb_action.sz_action_list[0] = action;
	
	if (p_usb_dev_info != NULL)
	{
		memcpy((void*)&usb_action.usb_dev_info, (const void*)p_usb_dev_info, sizeof(usb_dev_info_stru) - sizeof(vector<string>) - sizeof(vector<string>));
		usb_action.usb_dev_info.vec_dev_id = p_usb_dev_info->vec_dev_id;
		usb_action.usb_dev_info.vec_object = p_usb_dev_info->vec_object;
	}

	g_vec_usb_action.push_back(usb_action);

	clear_usb_dev_data(&usb_action.usb_dev_info);
	return CRG_SUCCESS;
}

static uint32_t update_usb_action_data(uint32_t data_index, void *dev, string pid, usb_dev_info_stru* p_usb_dev_info, uint8_t action)
{
	if (dev == NULL || (action != ACTION_PLUG_IN && action != ACTION_UNPLUG))
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;

	if (data_index < g_vec_usb_action.size())
	{
		g_vec_usb_action[data_index].p_dev = dev;

		if (action == ACTION_PLUG_IN)
		{
			if (strlen((const char*)g_vec_usb_action[data_index].sz_action_list) < sizeof(g_vec_usb_action[data_index].sz_action_list))
			{
				memcpy(g_vec_usb_action[data_index].sz_sz_pid[strlen((const char*)g_vec_usb_action[data_index].sz_action_list)], pid.c_str(), pid.length());
				g_vec_usb_action[data_index].sz_action_list[strlen((const char*)g_vec_usb_action[data_index].sz_action_list)] = action;

				if (p_usb_dev_info != NULL)
				{
					clear_usb_dev_data(&g_vec_usb_action[data_index].usb_dev_info);
					memcpy((void*)&g_vec_usb_action[data_index].usb_dev_info, (const void*)p_usb_dev_info, sizeof(usb_dev_info_stru)-sizeof(vector<string>) - sizeof(vector<string>));
					g_vec_usb_action[data_index].usb_dev_info.vec_dev_id = p_usb_dev_info->vec_dev_id;
					g_vec_usb_action[data_index].usb_dev_info.vec_object = p_usb_dev_info->vec_object;
				}

				ret = CRG_SUCCESS;
			}
			else
			{
				char tmp_pid[MAX_BUFF_LEN] = { 0 };
					
				if (strcmp(g_vec_usb_action[data_index].sz_sz_pid[1], "") != 0)
					snprintf(tmp_pid, sizeof(tmp_pid), "%s %s", g_vec_usb_action[data_index].sz_sz_pid[0], g_vec_usb_action[data_index].sz_sz_pid[1]);
				else
					snprintf(tmp_pid, sizeof(tmp_pid), "%s", g_vec_usb_action[data_index].sz_sz_pid[0]);

				write_log("%s action list was not space, check action list size, vid: %s, pid: %s, dev path: %s, action:%d%s%s",
					PRINT_USB_STR, g_vec_usb_action[data_index].sz_vid, tmp_pid, g_vec_usb_action[data_index].sz_dev_path, action, PRINT_POINT_STR, PRINT_BUG_STR);

				ret = CRG_NOT_ENOUGH_BUFF;
			}
		}
		else
		{
			if (strlen((const char*)g_vec_usb_action[data_index].sz_action_list) > 0)
			{
				if (g_vec_usb_action[data_index].sz_action_list[strlen((const char*)g_vec_usb_action[data_index].sz_action_list) - 1] == ACTION_PLUG_IN)
				{
					g_vec_usb_action[data_index].sz_action_list[strlen((const char*)g_vec_usb_action[data_index].sz_action_list) - 1] = '\0';
					if (strlen((const char*)g_vec_usb_action[data_index].sz_action_list) == 0)
					{
						write_log("%s usb action data was empty, because of the this time is unplug action, the last time is plug action. delete it from action list, vid: %s, pid: %s, dev path: %s%s%s",
							PRINT_USB_STR, g_vec_usb_action[data_index].sz_vid, g_vec_usb_action[data_index].sz_sz_pid[0], g_vec_usb_action[data_index].sz_dev_path, PRINT_POINT_STR, PRINT_OK_STR);
					
						clear_usb_dev_data(&g_vec_usb_action[data_index].usb_dev_info);
						memset(&g_vec_usb_action[data_index], 0, sizeof(usb_action_stru));
						g_vec_usb_action.erase(g_vec_usb_action.begin() + data_index);
						vector<usb_action_stru>(g_vec_usb_action).swap(g_vec_usb_action);
						ret = CRG_EMPTY_DATA;
					}
					else
						ret = CRG_SUCCESS;
				}
				else
				{
					char tmp_pid[MAX_BUFF_LEN] = { 0 };

					if (strcmp(g_vec_usb_action[data_index].sz_sz_pid[1], "") != 0)
						snprintf(tmp_pid, sizeof(tmp_pid), "%s %s", g_vec_usb_action[data_index].sz_sz_pid[0], g_vec_usb_action[data_index].sz_sz_pid[1]);
					else
						snprintf(tmp_pid, sizeof(tmp_pid), "%s", g_vec_usb_action[data_index].sz_sz_pid[0]);

					write_log("%s the two action is the same. vid: %s, pid: %s, dev path: %s, action: %d%s%s",
						PRINT_USB_STR, g_vec_usb_action[data_index].sz_vid, tmp_pid, g_vec_usb_action[data_index].sz_dev_path, action, PRINT_POINT_STR, PRINT_BUG_STR);
					// it's bug. the two action is the same
				}
			}
			else
			{
				write_log("%s it's not found this usb any action from action list. action list index: %d%s%s", PRINT_USB_STR, data_index, PRINT_POINT_STR, PRINT_BUG_STR);
				// it's bug
			}
		}
	}

	return ret;
}

uint32_t clear_usb_action(vector<usb_action_stru> * vec_usb_action)
{
	if (vec_usb_action == NULL)
		return CRG_FAIL;

	while (vec_usb_action->size() > 0)
	{
		clear_usb_dev_data(&(*vec_usb_action)[0].usb_dev_info);
		memset(&(*vec_usb_action)[0], 0, sizeof(usb_action_stru));
		vec_usb_action->erase(vec_usb_action->begin());
	}
	vector<usb_action_stru>(*vec_usb_action).swap(*vec_usb_action);
	return CRG_SUCCESS;
}

uint32_t clear_usb_action_for_list()
{
	uint32_t ret = CRG_FAIL;
	enter_usb_action_data_lock();
	ret = clear_usb_action(&g_vec_usb_action);
	leave_usb_action_data_lock();
	return ret;
}

uint32_t pull_all_usb_action_data_from_list(vector<usb_action_stru> & out_vec_usb_action)
{
	uint32_t ret = CRG_FAIL;
	usb_action_stru usb_action = {0};
	clear_usb_dev_data(&usb_action.usb_dev_info);
	
	enter_usb_action_data_lock();
	
	for (uint32_t i = 0; i < g_vec_usb_action.size(); i++)
	{
		clear_usb_dev_data(&usb_action.usb_dev_info);
		memcpy((void*)&usb_action, (const void*)&g_vec_usb_action[i], sizeof(usb_action_stru)-sizeof(vector<string>) - sizeof(vector<string>));
		usb_action.usb_dev_info.vec_dev_id = g_vec_usb_action[i].usb_dev_info.vec_dev_id;
		usb_action.usb_dev_info.vec_object = g_vec_usb_action[i].usb_dev_info.vec_object;
		out_vec_usb_action.push_back(usb_action);
	}
	ret = clear_usb_action(&g_vec_usb_action);
	leave_usb_action_data_lock();

	clear_usb_dev_data(&usb_action.usb_dev_info);

	return ret;
}

uint32_t save_usb_action_data_to_list(void *dev, string dev_path, string vid, string pid, usb_dev_info_stru* p_usb_dev_info, uint8_t action)
{
	uint32_t ret = CRG_FAIL;
	uint32_t data_index = 0;

	enter_usb_action_data_lock();
	ret = find_usb_action_data(dev_path, vid, pid, data_index);
	if (ret == CRG_NOT_FOUND)
	{
		ret = push_usb_action_data(dev, dev_path, vid, pid, p_usb_dev_info, action);
		if (ret != CRG_SUCCESS)
		{
			write_log("%s push new usb device action to action list, vid: %s, pid: %s, port: %s, action: %d, err: %d%s%s", PRINT_USB_STR, vid.c_str(), pid.c_str(), dev_path.c_str(), action, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		}
	}
	else
	{
		ret = update_usb_action_data(data_index, dev, pid, p_usb_dev_info, action);
		if (ret != CRG_SUCCESS && ret != CRG_EMPTY_DATA)
		{
			write_log("%s update usb device action to action list, vid: %s, pid: %s, port: %s, action: %d, err:%d%s%s", PRINT_USB_STR, vid.c_str(), pid.c_str(), dev_path.c_str(), action, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
		}
	}
	
	printf_usb_action_data_for_list();

	leave_usb_action_data_lock();
	
	return ret;
}

uint32_t init_usb_action_data_lock()
{
	uint32_t ret = CRG_FAIL;
	g_usb_action_data_lock_inited = false;

	ret = pthread_mutex_init(&g_usb_action_data_mutex, NULL) == 0 ? CRG_SUCCESS : CRG_FAIL;
	if (ret == CRG_SUCCESS)
		g_usb_action_data_lock_inited = true;
	else
		write_log("%s init mutex for usb action%s%s", PRINT_USB_STR, PRINT_POINT_STR, PRINT_FAIL_STR);

	return ret;
}

uint32_t destory_usb_action_data_lock()
{
	if (g_usb_action_data_lock_inited)
	{
		g_usb_action_data_lock_inited = false;
		return pthread_mutex_destroy(&g_usb_action_data_mutex) == 0 ? CRG_SUCCESS : CRG_FAIL;
	}
	return CRG_SUCCESS;
}
