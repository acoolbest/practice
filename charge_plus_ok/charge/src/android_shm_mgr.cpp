#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <sys/shm.h>  
#include <sys/stat.h> 
#include "public_func/src/def.h"
#include "third_party/libjson/libjson.h"
#include "public_func/src/public_function.h"
#include "android_shm_mgr.h"

#define	ANDROID_PID_DATA_POSITION_INDEX						0
#define	ANDROID_LIST_DATA_POSITION_INDEX					10

// static pthread_mutex_t							g_android_list_share_mem_mutex;
static void *										gp_android_list_shm_addr = NULL;
static int											g_android_list_shm_id = -1;
static bool											g_inited = false;
// static bool										g_all_data_lock_inited = false;
/*
static uint32_t init_share_mem_data_lock()
{
	uint32_t ret = CRG_FAIL;
	g_all_data_lock_inited = false;
	if (pthread_mutex_init(&g_android_list_share_mem_mutex, NULL) == 0)
	{
		ret = CRG_SUCCESS;
		g_all_data_lock_inited = true;
	}
	return ret;
}

static uint32_t destory_share_mem_data_lock()
{
	if (!g_all_data_lock_inited)
		return CRG_SUCCESS;

	uint32_t ret = CRG_FAIL;
	
	if (pthread_mutex_destroy(&g_android_list_share_mem_mutex) == 0)
	{
		ret = CRG_SUCCESS;
	}
	g_all_data_lock_inited = false;

	return ret;
}

static void enter_android_list_share_mem_data_lock()
{
	pthread_mutex_lock(&g_android_list_share_mem_mutex);
}

static void leave_android_list_share_mem_data_lock()
{
	pthread_mutex_unlock(&g_android_list_share_mem_mutex);
}
*/

static uint32_t clear_share_mem(void * & shm_addr)
{
	uint32_t ret = CRG_INVALID_PARAM;
	if (shm_addr != NULL)
	{
		memset(shm_addr, 0, SHARE_MEM_SIZE);
		ret = CRG_SUCCESS;
	}
	return ret;
}

static uint32_t free_share_mem(int & shm_id, void * & shm_addr)
{
	if (shm_id < 0)
		return CRG_INVALID_PARAM;

	if(shm_addr != NULL)
	{
		shmdt(shm_addr);
		shm_addr = NULL;
	}
	shmctl(shm_id, IPC_RMID, 0);
	shm_id = -1;
	
	return CRG_SUCCESS;
}

static uint32_t create_share_mem(key_t key, size_t size, int shmflg, int & out_shm_id, void * & out_shm_addr)
{
	uint32_t ret = CRG_FAIL;
	
	out_shm_id = shmget(key, size, shmflg);
	if (out_shm_id < 0)
	{
		write_log("%s create android share memory%s%s", PRINT_ANDROID_STR, PRINT_POINT_STR, PRINT_FAIL_STR);
		return ret;
	}
	out_shm_addr = shmat(out_shm_id, 0, 0);
	if (out_shm_addr == (void*)-1)
	{
		write_log("%s map android share memory%s%s", PRINT_ANDROID_STR, PRINT_POINT_STR, PRINT_FAIL_STR);
		goto _CREATE_ANDROID_SHARE_MEM_FAILURE_;
	}
	
	clear_share_mem(out_shm_addr);
	ret = CRG_SUCCESS;

_CREATE_ANDROID_SHARE_MEM_FAILURE_:
	if (ret != CRG_SUCCESS)
	{
		free_share_mem(out_shm_id, out_shm_addr);
	}
	
	return ret;
}

uint32_t write_android_list_data_to_share_mem(const char * json_str, uint32_t len)
{
	if (gp_android_list_shm_addr == NULL)
		return CRG_FAIL;

	snprintf((char*)gp_android_list_shm_addr + ANDROID_LIST_DATA_POSITION_INDEX, SHARE_MEM_SIZE - ANDROID_LIST_DATA_POSITION_INDEX, "%d", len);
	if (json_str != NULL && len > 0)
	{
		memcpy((char*)gp_android_list_shm_addr + strlen((const char*)gp_android_list_shm_addr + ANDROID_LIST_DATA_POSITION_INDEX) + ANDROID_LIST_DATA_POSITION_INDEX + 1, json_str, len);
	}
	return CRG_SUCCESS;

}

uint32_t read_android_list_data_from_share_mem(char * out_json_str, uint32_t & len)
{
	if (out_json_str == NULL || len < SHARE_MEM_SIZE)
		return CRG_INVALID_PARAM;

	if (gp_android_list_shm_addr == NULL)
		return CRG_FAIL;

	uint32_t ret = CRG_EMPTY_DATA;
	
	// enter_android_list_share_mem_data_lock();

	if (strlen((const char*)gp_android_list_shm_addr + ANDROID_LIST_DATA_POSITION_INDEX) > 0)
	{
		len = atoi((const char*)gp_android_list_shm_addr + ANDROID_LIST_DATA_POSITION_INDEX);
		if (len > 0)
		{
			memcpy(out_json_str, (const char*)gp_android_list_shm_addr + strlen((const char*)gp_android_list_shm_addr + ANDROID_LIST_DATA_POSITION_INDEX) + ANDROID_LIST_DATA_POSITION_INDEX + 1, len);
			printf("Android share mem: str: %s\n", out_json_str);
		}
		else
			printf("Android share mem: len = 0\n");

		ret = CRG_SUCCESS;
	}
		
	// leave_android_list_share_mem_data_lock();

	return ret;
}

uint32_t write_android_pid_data_to_share_mem(uint32_t pid_num)
{
	if (gp_android_list_shm_addr == NULL)
		return CRG_FAIL;

	clear_share_mem(gp_android_list_shm_addr);
	snprintf((char*)gp_android_list_shm_addr + ANDROID_PID_DATA_POSITION_INDEX, SHARE_MEM_SIZE - ANDROID_PID_DATA_POSITION_INDEX, "%d", pid_num);
	return CRG_SUCCESS;
}

uint32_t read_android_pid_data_from_share_mem(uint32_t & pid_num)
{
	if (gp_android_list_shm_addr == NULL )
		return CRG_FAIL;

	uint32_t ret = CRG_EMPTY_DATA;

	// enter_android_list_share_mem_data_lock();

	if (strlen((const char*)gp_android_list_shm_addr + ANDROID_PID_DATA_POSITION_INDEX) > 0)
	{
		pid_num = atoi((const char*)gp_android_list_shm_addr + ANDROID_PID_DATA_POSITION_INDEX);
		ret = CRG_SUCCESS;
	}

	// leave_android_list_share_mem_data_lock();

	return ret;
}

uint32_t init_android_list_share_mem_model()
{
	key_t android_list_key = 20140522;									// share name for android
	int android_list_shmflg = IPC_CREAT | S_IRUSR | S_IWUSR | 0666;		// if not exist and create it.

	return create_share_mem(android_list_key, SHARE_MEM_SIZE, android_list_shmflg, g_android_list_shm_id, gp_android_list_shm_addr);
}

uint32_t destory_android_list_share_mem_model()
{
	return free_share_mem(g_android_list_shm_id, gp_android_list_shm_addr);
}


uint32_t init_all_android_share_mem_model()
{
	uint32_t ret = CRG_FAIL;

	if (init_android_list_share_mem_model() == CRG_SUCCESS)
	{
		ret = CRG_SUCCESS;
	}

	g_inited = (ret == CRG_SUCCESS ? true : false);

	return ret;
}

uint32_t destory_all_android_share_mem_model()
{
	if (!g_inited)
		return CRG_SUCCESS;

	destory_android_list_share_mem_model();

	g_inited = false;

	return CRG_SUCCESS;
}

uint32_t make_json_for_android_list(vector<android_dev_info_stru>* vec_android_dev_info, string & out_json_str)
{
	if (vec_android_dev_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	JSONNode rn(JSON_NODE);
	for (uint32_t i = 0; i < vec_android_dev_info->size(); i++)
	{
		JSONNode n(JSON_ARRAY);
		n.set_name("android_device_info");
		JSONNode sn(JSON_NODE);
		sn.push_back(JSONNode("dev_id", (*vec_android_dev_info)[i].sz_dev_id));
		sn.push_back(JSONNode("path", (*vec_android_dev_info)[i].sz_path));
		sn.push_back(JSONNode("state", (*vec_android_dev_info)[i].sz_state));
		n.push_back(sn);
		rn.push_back(n);
		ret = CRG_SUCCESS;
	}
	if (ret == CRG_SUCCESS)
		out_json_str = rn.write_formatted();

	return ret;
}

uint32_t parse_json_for_android_list(string json_str, vector<android_dev_info_stru>* out_vec_android_dev_info)
{
	if (json_str.length() == 0 || out_vec_android_dev_info == NULL)
		return CRG_INVALID_PARAM;
	
	if (!libjson::is_valid(json_str))
		return CRG_FAIL;

	android_dev_info_stru android_dev_info = { 0 };
	JSONNode n = libjson::parse(json_str);
	JSONNode::const_iterator iter = n.begin();
	string str;
	while (iter != n.end())
	{
		string node_name = iter->name();
		if (node_name == "android_device_info")
		{
			if (iter->type() == JSON_ARRAY)
			{
				JSONNode::const_iterator child_iter = (*iter).begin();
				while (child_iter != (*iter).end())
				{
					memset((void*)&android_dev_info, 0, sizeof(android_dev_info_stru));
					android_dev_info.dw_proc_state = PROC_IDLE_STATE;

					JSONNode::const_iterator i = (*child_iter).begin();
					while (i != (*child_iter).end())
					{
						node_name = i->name();

						if (node_name == "dev_id")
						{
							str = i->as_string();
							memcpy(android_dev_info.sz_dev_id, str.c_str(), strlen(str.c_str()));
						}
						else if (node_name == "path")
						{
							str = i->as_string();
							memcpy(android_dev_info.sz_path, str.c_str(), strlen(str.c_str()));
						}
						else if (node_name == "state")
						{
							str = i->as_string();
							memcpy(android_dev_info.sz_state, str.c_str(), strlen(str.c_str()));
							android_dev_info.b_access = (strcasecmp(android_dev_info.sz_state, ANDROID_DEVICE_STATUS) == 0) ? true : false;
						}
						i++;
					}
					
					out_vec_android_dev_info->push_back(android_dev_info);
					child_iter++;
				}
			}
		}
		iter++;
	}
	return out_vec_android_dev_info->size() > 0 ? CRG_SUCCESS : CRG_FAIL;
}
