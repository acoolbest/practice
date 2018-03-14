#include <string.h>
#include <map> 
#include <stdio.h>
#include <unistd.h>

#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "public_func/src/json_read.h"
#include "third_party/libjson/libjson.h"
#include "http_req/src/http_req_interface.h"
#include "database.h"
#include "log_mgr.h"

user_config g_config_stru;

using namespace std;
using std::string;

#define			JSON_PROCESS_DIR_NODE				"activeRunPath"
#define			JSON_APPS_DIR_NODE					"activeAppsPath"
#define			JSON_GET_VERSION_ADDR_NODE			"verhttpRequestAdress"
#define			JSON_RECV_REQ_ADDR_NODE				"loghttpRequestAdress"
#define			JSON_SUCCESS_PHONE_ADDR_NODE		"sucesshttpRequestAdress"
#define			JSON_FAIL_PHONE_ADDR_NODE			"failhttpRequestAdress"
#define			JSON_HEART_ADDR_NODE				"hearthttpRequestAdress"
#define			JSON_REPORT_PORT_STATUS_NODE		"portstatushttpRequestAdress"

#define			UNKNOWN_DEVICE_TYPE_STR					"1"
#define			INSTALL_EXCEPTION_TYPE_STR				"2"
#define			ACCESS_DENIED_DEVICE_TYPE_STR			"3"
#define			USB_DEVICE_TYPE_STR					    "4"

#define			THREAD_COUNT				1

#define			SUCCESS_PHONE_ADDR_TYPE			0x01
#define			FAIL_PHONE_ADDR_TYPE			0x02
#define			NORMAL_DEVICE_ADDR_TYPE			0x03

typedef struct _CONNECT_SERVICE_STATUS_STRUCT_
{
	bool b_success_phone_addr_conected;
	bool b_fail_phone_addr_conected;
	bool b_normal_device_addr_conected;
}connect_service_status_info, *pConnectServiceStatusInfo;

static bool		g_thread_running = true;
static pthread_t g_pthread_pid;
static bool		g_thread_inited = false;


connect_service_status_info g_connect_server_status = {true, true, true};

void* send_local_db_to_server_thread(void* lpParam);

static bool Is_service_connected(int dw_type)
{
	bool ret = true;
	switch (dw_type) {
		case SUCCESS_PHONE_ADDR_TYPE:
			ret = g_connect_server_status.b_success_phone_addr_conected;
			break;
		case FAIL_PHONE_ADDR_TYPE:
			ret = g_connect_server_status.b_fail_phone_addr_conected;
			break;
		case NORMAL_DEVICE_ADDR_TYPE:
			ret = g_connect_server_status.b_normal_device_addr_conected;
			break;
		default:break;
	}
	return ret;
}

static void set_service_connect_status(int dw_type, bool bConnected)
{
	switch (dw_type) {
		case SUCCESS_PHONE_ADDR_TYPE:
			g_connect_server_status.b_success_phone_addr_conected = bConnected;
			break;
		case FAIL_PHONE_ADDR_TYPE:
			g_connect_server_status.b_fail_phone_addr_conected = bConnected;
			break;
		case NORMAL_DEVICE_ADDR_TYPE:
			g_connect_server_status.b_normal_device_addr_conected = bConnected;
			break;
		default:break;
	}
}

static bool is_thread_running()
{
	return g_thread_running;
}

static void set_thread_running_status(bool b_running)
{
	g_thread_running = b_running;
}

static uint32_t create_database_thread()
{
	uint32_t ret = CRG_SUCCESS;
	
	int err = pthread_create(&g_pthread_pid,NULL,send_local_db_to_server_thread,NULL);
	if(err != 0){
		write_log("[LOGERROR] can't create send_local_db_to_server_thread: %s\n",strerror(err));
		ret = CRG_FAIL; 
	}
	g_thread_inited = (ret == CRG_SUCCESS ? true : false);
	return ret;
}

#if 0
static int read_node_from_json_file(string str_node, string &str_value)
{
	char sz_config_path[MAX_PATH] = { 0 };
	uint32_t ret = get_path_of_config_json(sz_config_path);
	if (ret != CRG_FOUND) {
		write_log("File is not exist: %s file%s%s", sz_config_path, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		return ret;
	}
	string content = "";
	read_file(sz_config_path,content); 
	bool valid = libjson::is_valid(content);
	if(valid){
		JSONNode n = libjson::parse(content);
		JSONNode::const_iterator i = n.begin(); 	
		string str;		
		while(i != n.end()){  	
			std::string node_name = i -> name();
			if(node_name==str_node){
				str_value = i->as_string();
				break;
			} 
			i++;
		}
	}
	else{
		write_log("Load json file is failure, please check json format, file: %s\n", sz_config_path);
		return CRG_NOT_FOUND;
	}
	return CRG_SUCCESS;

}
#endif

static string make_access_denied_device_info_json(device_log_info* p_dev_info)
{
	string ret;
	JSONNode rn(JSON_NODE); 
	rn.push_back(JSONNode("type",ACCESS_DENIED_DEVICE_TYPE_STR));
	rn.push_back(JSONNode("device_id",p_dev_info->sz_dev_id));
	rn.push_back(JSONNode("vid",p_dev_info->sz_vid));
	rn.push_back(JSONNode("pid",p_dev_info->sz_pid));
	rn.push_back(JSONNode("terminal",p_dev_info->sz_terminal_id));
	rn.push_back(JSONNode("description",p_dev_info->sz_description));
	rn.push_back(JSONNode("time",p_dev_info->sz_log_times));
	ret = rn.write_formatted();
	return ret;

}

static string make_unknow_device_info_json(device_log_info* p_dev_info)
{
	string ret;
	JSONNode rn(JSON_NODE); 
	rn.push_back(JSONNode("type",UNKNOWN_DEVICE_TYPE_STR));
	rn.push_back(JSONNode("device_id",p_dev_info->sz_dev_id));
	rn.push_back(JSONNode("vid",p_dev_info->sz_vid));
	rn.push_back(JSONNode("pid",p_dev_info->sz_pid));
	rn.push_back(JSONNode("terminal",p_dev_info->sz_terminal_id));
	rn.push_back(JSONNode("description",p_dev_info->sz_description));
	rn.push_back(JSONNode("time",p_dev_info->sz_log_times));
	ret = rn.write_formatted();
	return ret;
}



static string make_usb_device_info_json(device_log_info* p_dev_info)
{
	string ret;
	JSONNode rn(JSON_NODE); 
	rn.push_back(JSONNode("type",USB_DEVICE_TYPE_STR));
	rn.push_back(JSONNode("device_id",p_dev_info->sz_dev_id));
	rn.push_back(JSONNode("vid",p_dev_info->sz_vid));
	rn.push_back(JSONNode("pid",p_dev_info->sz_pid));
	rn.push_back(JSONNode("terminal",p_dev_info->sz_terminal_id));	
	rn.push_back(JSONNode("time",p_dev_info->sz_log_times));
	ret = rn.write_formatted();		
	return ret;
}

static string make_normal_device_info_json(device_log_info* p_dev_info)
{
	string ret;
	JSONNode rn(JSON_NODE); 
	rn.push_back(JSONNode("device_id",p_dev_info->sz_dev_id));
	rn.push_back(JSONNode("model",p_dev_info->sz_model));
	rn.push_back(JSONNode("phone_number",p_dev_info->sz_phone_number));
	rn.push_back(JSONNode("sys_param",p_dev_info->sz_sys_param));
	rn.push_back(JSONNode("sys_version",p_dev_info->sz_sys_version));
	rn.push_back(JSONNode("vid",p_dev_info->sz_vid));
	rn.push_back(JSONNode("pid",p_dev_info->sz_pid));
	rn.push_back(JSONNode("terminal",p_dev_info->sz_terminal_id));
	rn.push_back(JSONNode("description",p_dev_info->sz_description));
	rn.push_back(JSONNode("time",p_dev_info->sz_log_times));
	ret = rn.write_formatted();
	return ret;
}

static string make_install_app_success_json(install_app_result_log_info* p_install_app_result_info)
{
	if (p_install_app_result_info == NULL || p_install_app_result_info->vec_installed_app_time.size() <= 0)
		return "";
			
	string ret;
	unsigned long i = 0;
	JSONNode rn(JSON_NODE);
	do
	{
		char sz_time_interval[50] = { 0 };
		sprintf(sz_time_interval, "%d", p_install_app_result_info->vec_installed_app_time[i].dw_time_interval);
		JSONNode n(JSON_ARRAY);
		n.set_name("appsData");
		JSONNode sn(JSON_NODE);
		sn.push_back(JSONNode("app_id", p_install_app_result_info->vec_installed_app_time[i].sz_app_id));
		sn.push_back(JSONNode("time_interval", sz_time_interval));
		n.push_back(sn);
		rn.push_back(n);
	} while (++i && i < p_install_app_result_info->vec_installed_app_time.size());

	rn.push_back(JSONNode("device_id", p_install_app_result_info->device_info.sz_dev_id));
	rn.push_back(JSONNode("vid", p_install_app_result_info->device_info.sz_vid));
	rn.push_back(JSONNode("pid", p_install_app_result_info->device_info.sz_pid));
	rn.push_back(JSONNode("terminal", p_install_app_result_info->device_info.sz_terminal_id));
	rn.push_back(JSONNode("description", p_install_app_result_info->device_info.sz_description));
	rn.push_back(JSONNode("time", p_install_app_result_info->device_info.sz_log_times));
	char sz_app_source[50] = { 0 };
	sprintf(sz_app_source, "%d", p_install_app_result_info->dw_app_source);
	rn.push_back(JSONNode("appSource", sz_app_source));
	ret = rn.write_formatted();
	return ret;
}

static string make_install_app_fail_json(install_app_result_log_info* p_install_app_result_info)
{
	if (p_install_app_result_info == NULL)
		return "";
			
	string ret;
	char sz_fail_reason[10] = { 0 };
	JSONNode rn(JSON_NODE);
	rn.push_back(JSONNode("type", INSTALL_EXCEPTION_TYPE_STR));
	rn.push_back(JSONNode("device_id", p_install_app_result_info->device_info.sz_dev_id));
	rn.push_back(JSONNode("vid", p_install_app_result_info->device_info.sz_vid));
	rn.push_back(JSONNode("pid", p_install_app_result_info->device_info.sz_pid));
	rn.push_back(JSONNode("terminal", p_install_app_result_info->device_info.sz_terminal_id));
	sprintf(sz_fail_reason, "%d", p_install_app_result_info->dw_result);
	rn.push_back(JSONNode("description", sz_fail_reason));
	rn.push_back(JSONNode("time", p_install_app_result_info->device_info.sz_log_times));
	ret = rn.write_formatted();
	return ret;
}

static string make_install_app_result_json(install_app_result_log_info* p_install_app_result_info)
{
	if (p_install_app_result_info == NULL)
		return "";
			
	string str;
	switch (p_install_app_result_info->dw_result)
	{
	case CRG_SUCCESS:
		str = make_install_app_success_json(p_install_app_result_info);
		break;
	default:
		str = make_install_app_fail_json(p_install_app_result_info);
		break;
	}
	return str;
}

static string make_heartbeat_json(char* p_terminal, char * p_version, char* p_net_new)
{
	if (p_terminal == NULL)
		return "";
	string ret;
	JSONNode rn(JSON_NODE); 
	rn.push_back(JSONNode("terminal",p_terminal));
	rn.push_back(JSONNode("software_version",p_version));
	#if 1
	rn.push_back(JSONNode("client_time",get_current_time().c_str()));
	rn.push_back(JSONNode("traffic",p_net_new));
	#endif
	ret = rn.write_formatted();
	rn.clear();//add by zhzq @ 2016-1-25 11:38:25
	return ret;
}

int send_device_info_to_server(device_log_info* p_dev_info,bool b_backup_db_if_fail)
{
	if (p_dev_info == NULL)
		return CRG_INVALID_PARAM;
			
	int ret = CRG_FAIL;
	int dw_type = 0;
	string str_json, str_url, str_node, str_err;
	int http_ret = 0;
	bool bSucc = true;

	switch (p_dev_info->dw_dev_type)
	{
	case ANDROID_DEVICE_TYPE:
	case IOS_DEVICE_TYPE:
		dw_type = NORMAL_DEVICE_ADDR_TYPE;
		if (p_dev_info->b_access)
		{
			str_node = JSON_RECV_REQ_ADDR_NODE;
			str_url = g_config_stru.log_url;
			str_json = make_normal_device_info_json(p_dev_info);
		}
		else
		{
			str_node = JSON_FAIL_PHONE_ADDR_NODE;
			str_url = g_config_stru.fail_url;
			str_json = make_access_denied_device_info_json(p_dev_info);
		}
		break;
	case UNKNOWN_DEVICE_TYPE:
		str_json = make_unknow_device_info_json(p_dev_info);
		str_node = JSON_FAIL_PHONE_ADDR_NODE;
		str_url = g_config_stru.fail_url;
		dw_type = FAIL_PHONE_ADDR_TYPE;
		break;
	case USB_DEVICE_TYPE:
		str_json = make_usb_device_info_json(p_dev_info);
		str_node = JSON_FAIL_PHONE_ADDR_NODE;
		str_url = g_config_stru.fail_url;
		dw_type = FAIL_PHONE_ADDR_TYPE;
		break;
	default:
		bSucc = false;
		break;
	}
	
	if (!bSucc)
		return CRG_INVALID_PARAM;
				
	//if (read_node_from_json_file(str_node, str_url) == CRG_SUCCESS)
	{
		try
		{
			http_ret = http_request_with_address_type(str_url, str_json, "Content-Type:application/json; charset=utf-8", str_err);
			if (http_ret != 200)
			{
				write_log("Send device info to service, dev type: %d, access: %d, vid: %s, pid: %s, err str: %s, url: %s%s[ERR: %d] %s",
					p_dev_info->dw_dev_type, p_dev_info->b_access, p_dev_info->sz_vid, p_dev_info->sz_pid, str_err.c_str(), str_url.c_str(), PRINT_POINT_STR, http_ret, PRINT_FAIL_STR);
			}
			else
			{
				write_log("Send device info to service, dev type: %d, access: %d, vid: %s, pid: %s, S/N: %s url: %s%s%s",
					p_dev_info->dw_dev_type, p_dev_info->b_access, p_dev_info->sz_vid, p_dev_info->sz_pid, p_dev_info->sz_dev_id, str_url.c_str(), PRINT_POINT_STR, PRINT_OK_STR);
			}
		}
		catch (exception ex)
		{
			write_log("Send device info to service, dev type: %d, access: %d, vid: %s, pid: %s, url: %s, err: %s%s%s",
				p_dev_info->dw_dev_type, p_dev_info->b_access, p_dev_info->sz_vid, p_dev_info->sz_pid, str_url.c_str(), ex.what(), PRINT_POINT_STR, PRINT_EXCEPTION_STR);
		}
		set_service_connect_status(dw_type, (http_ret == 200 ? true : false));
	}
	
	ret = http_ret == 200 ? CRG_SUCCESS : CRG_FAIL;
		
	if (http_ret != 200) // failure
	{
		if (b_backup_db_if_fail)
		{
			push_device_info_to_database(p_dev_info);
					
		}
	}
	
	return ret;
}

int send_install_app_result_to_server(install_app_result_log_info* p_install_app_result_info,bool b_backup_db_if_fail)
{
	if (p_install_app_result_info == NULL)
		return CRG_INVALID_PARAM;
			
	int http_ret = 0;
	int ret = CRG_FAIL;
	int dw_type = 0;
	string str_json, str_url, str_node, str_err;
	str_json = make_install_app_result_json(p_install_app_result_info);
	switch (p_install_app_result_info->dw_result)
	{
		case CRG_SUCCESS:
			str_node = JSON_SUCCESS_PHONE_ADDR_NODE;
			str_url = g_config_stru.success_url;
			dw_type = SUCCESS_PHONE_ADDR_TYPE;
			break;
		default:
			str_node = JSON_FAIL_PHONE_ADDR_NODE;
			dw_type = FAIL_PHONE_ADDR_TYPE;
			str_url = g_config_stru.fail_url;
			break;
	}
	
	//if (read_node_from_json_file(str_node, str_url) == CRG_SUCCESS)
	{
		try
		{
			http_ret = http_request_with_address_type(str_url, str_json, "Content-Type:application/json; charset=utf-8", str_err);
			if (http_ret != 200)
			{
				if (p_install_app_result_info->vec_installed_app_time.size() > 0)
				{
					write_log("Send install app result to service, app source: %d, app id: %s, install result: %d, vid: %s, pid: %s, err str: %s, url: %s%s[ERR: %d] %s",
							p_install_app_result_info->dw_app_source, p_install_app_result_info->vec_installed_app_time[0].sz_app_id, p_install_app_result_info->dw_result,
							p_install_app_result_info->device_info.sz_vid, p_install_app_result_info->device_info.sz_pid, str_err.c_str(), str_url.c_str(), PRINT_POINT_STR, http_ret, PRINT_FAIL_STR);
				}
				else
				{
					write_log("Send install app result to service, app source: %d, install result: %d, vid: %s, pid: %s, err str: %s, url: %s%s[ERR: %d] %s",
						p_install_app_result_info->dw_app_source, p_install_app_result_info->dw_result, p_install_app_result_info->device_info.sz_vid, p_install_app_result_info->device_info.sz_pid, str_err.c_str(), str_url.c_str(),
							PRINT_POINT_STR, http_ret, PRINT_FAIL_STR);
				}
						
			}
			else
			{
				if (p_install_app_result_info->vec_installed_app_time.size() > 0)
				{
					write_log("Send install app result to service, app source: %d, app id: %s, install result: %d, vid: %s, pid: %s, url: %s%s%s",
						p_install_app_result_info->dw_app_source, p_install_app_result_info->vec_installed_app_time[0].sz_app_id, p_install_app_result_info->dw_result,
							p_install_app_result_info->device_info.sz_vid, p_install_app_result_info->device_info.sz_pid, str_url.c_str(), PRINT_POINT_STR, PRINT_OK_STR);
				}
				else
				{
					write_log("Send install app result to service, app source: %d, install result: %d, vid: %s, pid: %s, url: %s%s%s",
						p_install_app_result_info->dw_app_source, p_install_app_result_info->dw_result, p_install_app_result_info->device_info.sz_vid, p_install_app_result_info->device_info.sz_pid, str_url.c_str(),
							PRINT_POINT_STR, PRINT_OK_STR);
				}
			}
		}
		catch (exception ex)
		{
			if (p_install_app_result_info->vec_installed_app_time.size() > 0)
			{
				write_log("Send install app result to service, app source: %d, app id: %s, install result: %d, vid: %s, pid: %s, url: %s, err: %s%s%s",
					p_install_app_result_info->dw_app_source, p_install_app_result_info->vec_installed_app_time[0].sz_app_id, p_install_app_result_info->dw_result,
						p_install_app_result_info->device_info.sz_vid, p_install_app_result_info->device_info.sz_pid, str_url.c_str(), ex.what(), PRINT_POINT_STR, PRINT_EXCEPTION_STR);
			}
			else
			{
				write_log("Send install app result to service, app source: %d, install result: %d, vid: %s, pid: %s, url: %s, err: %s%s%s",
					p_install_app_result_info->dw_app_source, p_install_app_result_info->dw_result, p_install_app_result_info->device_info.sz_vid, p_install_app_result_info->device_info.sz_pid, str_url.c_str(),
						ex.what(), PRINT_POINT_STR, PRINT_EXCEPTION_STR);
			}
		}
			
			set_service_connect_status(dw_type, (http_ret == 200 ? true : false));
	}
	
	ret = http_ret == 200 ? CRG_SUCCESS : CRG_FAIL;
		
	if (http_ret != 200) // failure
	{
		if (b_backup_db_if_fail)
		{
			push_install_app_result_to_database(p_install_app_result_info);	
		}
	}
	
	return ret;
}

int send_heartbeat_info_to_server(char* p_terminal, char* p_version, char* p_net_new)
{
	if (p_terminal == NULL || p_version == NULL || p_net_new == NULL)
		return CRG_INVALID_PARAM;
			
	int ret = CRG_FAIL;
	int http_ret = 0;
	string str_json, str_url, str_err;
	str_json = make_heartbeat_json(p_terminal, p_version, p_net_new);
	str_url = g_config_stru.heart_url;
	//if (read_node_from_json_file(JSON_HEART_ADDR_NODE, str_url) == CRG_SUCCESS)
	{
		try
		{
			http_ret = http_request_with_address_type(str_url, str_json, "Content-Type:application/json; charset=utf-8", str_err);
			if (http_ret != 200)
			{
				write_log("Send heartbeat info to service, terminal: %s, version: %s, net_traffic: %s, err str: %s, url: %s%s[ERR: %d] %s",
						p_terminal, p_version, p_net_new, str_err.c_str(), str_url.c_str(), PRINT_POINT_STR, http_ret, PRINT_FAIL_STR);
			}
			else
			{
				write_log("Send heartbeat info to service, terminal: %s, version: %s, net_traffic: %s, url: %s%s%s",
						p_terminal, p_version, p_net_new, str_url.c_str(), PRINT_POINT_STR, PRINT_OK_STR);
			}
		}
		catch (exception ex)
		{
			write_log("Send heartbeat info to service, terminal: %s, version: %s, net_traffic: %s, url: %s, err: %s%s%s",
					p_terminal, p_version, p_net_new, str_url.c_str(), ex.what(), PRINT_POINT_STR, PRINT_EXCEPTION_STR);
						
		}
	}
	
	if (http_ret == 200) // success
	{
		ret = CRG_SUCCESS;
	}
	else
	{
		ret = CRG_FAIL;
	}
	return ret;
}

void* send_local_db_to_server_thread(void* lpParam)
{
	install_app_result_log_info_map install_app_result_map;
	device_log_info_map dev_info_map;
		
	do
	{
		if (Is_service_connected(SUCCESS_PHONE_ADDR_TYPE) || Is_service_connected(FAIL_PHONE_ADDR_TYPE))
		{
			if (pull_all_install_app_result_from_database(&install_app_result_map) == CRG_SUCCESS)
			{
				install_app_result_log_info_map::iterator iter;
				for (iter = install_app_result_map.begin(); iter != install_app_result_map.end();)
				{
					if (is_thread_running())
					{
						if (send_install_app_result_to_server(&iter->second, false) == CRG_SUCCESS)
						{
							remove_install_app_result_from_database(iter->first);
						}
					}
					clear_install_app_result_log_info(&(iter->second));
					install_app_result_map.erase(iter++);
				}
				install_app_result_map.clear();
				install_app_result_log_info_map(install_app_result_map).swap(install_app_result_map);
			}
		}

		if (!is_thread_running()) break;
		
		if (Is_service_connected(NORMAL_DEVICE_ADDR_TYPE) || Is_service_connected(FAIL_PHONE_ADDR_TYPE))
		{
			if (pull_all_device_info_from_database(&dev_info_map) == CRG_SUCCESS)
			{
				device_log_info_map::iterator iter1;
				for (iter1 = dev_info_map.begin(); iter1 != dev_info_map.end();)
				{
					if (send_device_info_to_server(&iter1->second, false) == CRG_SUCCESS)
					{
						remove_device_info_from_database(iter1->first);
					}
					dev_info_map.erase(iter1++);
				}
				dev_info_map.clear();
				device_log_info_map(dev_info_map).swap(dev_info_map);//map <int, device_log_info>
			}
		}
		for(int i=0;i<60;i++)
		{
			if (!is_thread_running())
			{
				break;
			}
			sleep(1);
		}
	} while (is_thread_running());
	return 0;
}

int init_log_manager(char* p_db_name)
{
	if (p_db_name == NULL)
		return CRG_INVALID_PARAM;
	read_config(g_config_stru);
	uint32_t dw_ret = CRG_FAIL;

	g_connect_server_status.b_success_phone_addr_conected = true;
	g_connect_server_status.b_fail_phone_addr_conected = true;
	g_connect_server_status.b_normal_device_addr_conected = true;

	dw_ret = init_database(p_db_name);
	if (dw_ret != CRG_SUCCESS)
		return dw_ret;

	return create_database_thread();
}

static void set_database_thread_exit_status()
{
	set_thread_running_status(false);
}

static void wait_database_exit()
{
	if(g_thread_inited) pthread_join(g_pthread_pid, NULL);
	g_thread_inited = false;
}

int destory_log_manager()
{
	set_database_thread_exit_status();
	wait_database_exit();
	destory_database();
	return CRG_SUCCESS;
}

