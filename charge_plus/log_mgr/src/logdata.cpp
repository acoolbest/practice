#include <string.h>
#include <vector>
#include <map> 
#include <unistd.h>
#include "public_func/src/def.h"
#include "log_mgr.h"
#include "http_req/src/http_req.h"
#include "public_func/src/public_function.h"
#include "public_func/src/json_read.h"
#include "logdata.h"

static char g_terminal_id[MAX_BUFF_LEN] = "";
static bool g_inited = false;

static int make_ios_device_log(ios_dev_info_stru* p_dev_src, device_log_info* p_dev_dest)
{
	if (p_dev_src == NULL || p_dev_dest == NULL)
		return CRG_INVALID_PARAM;

	p_dev_dest->dw_dev_type = IOS_DEVICE_TYPE;
	p_dev_dest->b_access = p_dev_src->b_access;
	memcpy(p_dev_dest->sz_dev_id, p_dev_src->sz_unique_dev_id, strlen(p_dev_src->sz_unique_dev_id));
	memcpy(p_dev_dest->sz_model,"Apple", sizeof(char)*strlen("Apple"));
	memcpy(p_dev_dest->sz_sys_param, p_dev_src->sz_product_type, strlen(p_dev_src->sz_product_type));
	// memcpy(p_dev_dest->sz_phone_number, p_dev_src->sz_phone_number,strlen(p_dev_src->sz_phone_number));
	memcpy(p_dev_dest->sz_phone_number, NULL_STR, strlen(NULL_STR)*sizeof(char));
	memcpy(p_dev_dest->sz_description, NULL_STR, strlen(NULL_STR)*sizeof(char));
	snprintf(p_dev_dest->sz_sys_version, sizeof(char)*MAX_BUFF_LEN,"Ios %s", p_dev_src->sz_product_version);

	strcpy(p_dev_dest->sz_terminal_id, g_terminal_id);

	memcpy(p_dev_dest->sz_pid, p_dev_src->sz_pid, sizeof(p_dev_src->sz_pid));
	memcpy(p_dev_dest->sz_vid, p_dev_src->sz_vid, sizeof(p_dev_src->sz_vid));

	memcpy(p_dev_dest->sz_plugin_times, p_dev_src->sz_plugin_times, sizeof(p_dev_src->sz_plugin_times));

	string cur_time = get_current_time();                                                                                                                          
	strcpy(p_dev_dest->sz_log_times, cur_time.c_str());
	return CRG_SUCCESS;
}

static int make_android_device_log(android_dev_info_stru* p_dev_src, device_log_info* p_dev_dest)
{
	if (p_dev_src == NULL || p_dev_dest == NULL)
		return CRG_INVALID_PARAM;

	p_dev_dest->dw_dev_type = ANDROID_DEVICE_TYPE;
	p_dev_dest->b_access = p_dev_src->b_access;
	memcpy(p_dev_dest->sz_phone_number, NULL_STR, strlen(NULL_STR)*sizeof(char));
	memcpy(p_dev_dest->sz_description, NULL_STR, strlen(NULL_STR)*sizeof(char));

	strcpy(p_dev_dest->sz_terminal_id, g_terminal_id);

	memcpy(p_dev_dest->sz_dev_id, p_dev_src->sz_dev_id, sizeof(p_dev_src->sz_dev_id));
	memcpy(p_dev_dest->sz_pid, p_dev_src->sz_pid, sizeof(p_dev_src->sz_pid));
	memcpy(p_dev_dest->sz_vid, p_dev_src->sz_vid, sizeof(p_dev_src->sz_vid));
	memcpy(p_dev_dest->sz_plugin_times, p_dev_src->sz_plugin_times, sizeof(p_dev_src->sz_plugin_times));

	string cur_time = get_current_time();                                                                                                                          
	strcpy(p_dev_dest->sz_log_times, cur_time.c_str());

	if (p_dev_src->b_access)
	{
		memcpy(p_dev_dest->sz_model, p_dev_src->sz_model, sizeof(p_dev_src->sz_model));
		memcpy(p_dev_dest->sz_sys_param, p_dev_src->sz_brand, sizeof(p_dev_src->sz_brand));
		snprintf(p_dev_dest->sz_sys_version, sizeof(char)*MAX_BUFF_LEN,"Android %s", p_dev_src->sz_os_version);
	}
	else
	{
		memcpy(p_dev_dest->sz_model, NULL_STR, strlen(NULL_STR)*sizeof(char));
		memcpy(p_dev_dest->sz_sys_param, NULL_STR, strlen(NULL_STR)*sizeof(char));
		snprintf(p_dev_dest->sz_sys_version, sizeof(char)*MAX_BUFF_LEN,"Android");
	}

	return CRG_SUCCESS;
}

static int make_unknow_device_log(usb_dev_info_stru* p_dev_src, device_log_info* p_dev_dest)
{
	if (p_dev_src == NULL || p_dev_dest == NULL)
		return CRG_INVALID_PARAM;

	p_dev_dest->dw_dev_type = UNKNOWN_DEVICE_TYPE;
	p_dev_dest->b_access = false;

	if(p_dev_src->vec_dev_id.size()) strcpy(p_dev_dest->sz_dev_id,p_dev_src->vec_dev_id[0].c_str());

	memcpy(p_dev_dest->sz_model, NULL_STR, strlen(NULL_STR)*sizeof(char));
	memcpy(p_dev_dest->sz_sys_param, NULL_STR, strlen(NULL_STR)*sizeof(char));
	memcpy(p_dev_dest->sz_phone_number, NULL_STR, strlen(NULL_STR)*sizeof(char));
	memcpy(p_dev_dest->sz_description, NULL_STR, strlen(NULL_STR)*sizeof(char));
	memcpy(p_dev_dest->sz_sys_version, NULL_STR, strlen(NULL_STR)*sizeof(char));

	strcpy(p_dev_dest->sz_terminal_id, g_terminal_id);

	memcpy(p_dev_dest->sz_pid, p_dev_src->sz_pid, sizeof(p_dev_src->sz_pid));
	memcpy(p_dev_dest->sz_vid, p_dev_src->sz_vid, sizeof(p_dev_src->sz_vid));
	memcpy(p_dev_dest->sz_plugin_times, p_dev_src->sz_plugin_times, sizeof(p_dev_src->sz_plugin_times));

	string cur_time = get_current_time();                                                                                                                          
	strcpy(p_dev_dest->sz_log_times, cur_time.c_str());

	return CRG_SUCCESS;
}

static int make_usb_device_log(usb_dev_info_stru* p_dev_src, device_log_info* p_dev_dest)
{
	if (p_dev_src == NULL || p_dev_dest == NULL)
		return CRG_INVALID_PARAM;

	p_dev_dest->dw_dev_type = USB_DEVICE_TYPE;
	p_dev_dest->b_access = false;
	
	if(p_dev_src->vec_dev_id.size()) strcpy(p_dev_dest->sz_dev_id, p_dev_src->vec_dev_id[0].c_str());

	strcpy(p_dev_dest->sz_terminal_id, g_terminal_id);

	memcpy(p_dev_dest->sz_pid, p_dev_src->sz_pid, sizeof(p_dev_src->sz_pid));
	memcpy(p_dev_dest->sz_vid, p_dev_src->sz_vid, sizeof(p_dev_src->sz_vid));
	memcpy(p_dev_dest->sz_plugin_times, p_dev_src->sz_plugin_times, sizeof(p_dev_src->sz_plugin_times));

	string cur_time = get_current_time();                                                                                                                          
	strcpy(p_dev_dest->sz_log_times, cur_time.c_str());

	return CRG_SUCCESS;
}

int send_device_info(int dw_device_type, void* p_devinfo)
{
	int ret = CRG_FAIL;
	device_log_info dev_info = { 0 };
	switch (dw_device_type)
	{
		case ANDROID_DEVICE_TYPE:
			ret = make_android_device_log((android_dev_info_stru*)p_devinfo, &dev_info);
			break;
		case IOS_DEVICE_TYPE:
			ret = make_ios_device_log((ios_dev_info_stru*)p_devinfo, &dev_info);
			break;
		case UNKNOWN_DEVICE_TYPE:
			ret = make_unknow_device_log((usb_dev_info_stru*)p_devinfo, &dev_info);
			break;
		case USB_DEVICE_TYPE:
			ret = make_usb_device_log((usb_dev_info_stru*)p_devinfo, &dev_info);
			break;
		default:
			break;
	}

	if (ret == CRG_SUCCESS)
		ret = send_device_info_to_server(&dev_info);

	return ret;
}

int send_install_app_result(uint32_t dw_result, uint32_t dw_device_type, uint32_t dw_app_source, char* p_app_id, uint32_t dw_interval, void* p_devinfo)
{
	int ret = CRG_FAIL;
	install_app_time_info install_app_time = { 0 };
	install_app_result_log_info install_app_result_info;
	clear_install_app_result_log_info(&install_app_result_info);

	if (dw_result == CRG_SUCCESS && p_app_id != NULL)
	{
		memcpy(install_app_time.sz_app_id, p_app_id, sizeof(char)*strlen(p_app_id));
		install_app_time.dw_time_interval = dw_interval;
		install_app_result_info.vec_installed_app_time.push_back(install_app_time);
	}
	install_app_result_info.dw_result = dw_result;
	install_app_result_info.dw_app_source = dw_app_source;
	
	switch (dw_device_type)
	{
		case ANDROID_DEVICE_TYPE:
			ret = make_android_device_log((android_dev_info_stru*)p_devinfo, &install_app_result_info.device_info);
			break;
		case IOS_DEVICE_TYPE:
			ret = make_ios_device_log((ios_dev_info_stru*)p_devinfo, &install_app_result_info.device_info);
			break;
		default:
			break;
	}

	if (ret == CRG_SUCCESS)
		ret = send_install_app_result_to_server(&install_app_result_info);

	clear_install_app_result_log_info(&install_app_result_info);

	return ret;
}

static int init_terminal_name(void)
{
	user_config cnfg;
	if (read_config(cnfg) == CRG_SUCCESS && cnfg.terminal.size() > 0 && strcmp(cnfg.terminal.c_str(),"null") != 0 )
	{
		strcpy(g_terminal_id, cnfg.terminal.c_str());
		return CRG_SUCCESS;
	}
	else
	{
		write_log("read terminal name failed.\n");
		return CRG_FAIL;
	}
}

int init_log_model(char* p_db_name)
{
	uint32_t ret = CRG_FAIL;
	write_log("init log model%s%s", PRINT_POINT_STR,PRINT_START_STR);
	if(init_terminal_name() != CRG_SUCCESS) ret = CRG_FAIL;
	else ret = init_log_manager(p_db_name);

	if (ret == CRG_SUCCESS){
		write_log("init log model%s%s",PRINT_POINT_STR,PRINT_OK_STR);
	}else{
		write_log("init log model%s%s",PRINT_POINT_STR,PRINT_FAIL_STR);
	}
	g_inited = (ret == CRG_SUCCESS ? true : false);
	return ret;
}

int destory_log_model()
{
	if(!g_inited)
		return CRG_SUCCESS;
	int ret = CRG_FAIL;
	write_log("destory log model%s%s", PRINT_POINT_STR,PRINT_START_STR);
	ret = destory_log_manager();
	if (ret == CRG_SUCCESS){
		write_log("destory log model%s%s",PRINT_POINT_STR,PRINT_OK_STR);
	}else{
		write_log("destory log model%s%s",PRINT_POINT_STR,PRINT_FAIL_STR);
	}

	return ret;
}

