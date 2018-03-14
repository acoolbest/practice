#include <string.h>
#include "stdio.h"
#include <map> 
#include <time.h>
#include <sqlite3.h>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "database.h"

using std::string;

#define INSTALL_APP_RESULT_DATABASE_TYPE						0x01
#define DEVICE_INFO_DATABASE_TYPE								0x02

pthread_mutex_t device_info_database_mutex;
pthread_mutex_t install_app_result_database_mutex;
sqlite3 * gp_db;  


static int create_database(int dw_type)
{
	string str;
	switch (dw_type)
	{
		case INSTALL_APP_RESULT_DATABASE_TYPE:
			str = "create table if not exists AppResultLog("
				"Id INTEGER  primary key,"
				"DevId string null, "
				"DevType int null, "
				"Access int null, "
				"Model string null, "

				"SysParam string null, "
				"SysVersion string null, "
				"Vid int null, "
				"Pid int null, "
				"PhoneNumber string null, "
				"PluginTimes string null, "

				"TerminalId string null,"
				"Description string null,"
				"AppSource int null, "
				"AppId string  null, "
				"TimeInterval int null,"
				"InstallResult int null,"
				"LogTimes string null)";

			break;
		case DEVICE_INFO_DATABASE_TYPE:
			str = "create table if not exists DeviceInfoLog("
				"Id INTEGER  primary key,"
				"DevId string null, "
				"DevType int null, "
				"Access int null, "
				"Model string null, "
				"SysParam string null, "
				"SysVersion string null, "
				"Vid int null, "
				"Pid int null, "
				"PhoneNumber string null, "
				"PluginTimes string null, "
				"TerminalId string null,"
				"Description string null,"
				"LogTimes string null)";
			break;
		default:
			break;
	}

	char* err_msg;  

	int res= sqlite3_exec(gp_db , str.c_str() , 0 , 0 , &err_msg);  

	if (res != SQLITE_OK)  
	{  
		write_log("[LOGERROR] 执行SQL 出错. %s",err_msg);
		return CRG_FAIL;  
	}else{
		return CRG_SUCCESS;
	}
}

static string get_insert_sql_for_install_app_result()
{
	return ("insert into AppResultLog(DevId, DevType, Access, Model, SysParam, SysVersion, Vid, Pid, PhoneNumber, PluginTimes, TerminalId, Description, AppSource, AppId, TimeInterval, InstallResult, LogTimes) \
			values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
}

static string get_insert_sql_for_device_info()
{
	return ("insert into DeviceInfoLog(DevId, DevType, Access, Model, SysParam, SysVersion, Vid, Pid, PhoneNumber, PluginTimes, TerminalId, Description, LogTimes) \
			values(?,?,?,?,?,?,?,?,?,?,?,?,?)");
}

static string get_select_sql_for_install_app_result()
{
	return ("SELECT Id, DevId, DevType, Access, Model, SysParam, SysVersion, Vid, Pid, PhoneNumber, PluginTimes, TerminalId, Description, AppSource, AppId, TimeInterval, InstallResult, LogTimes \
			FROM AppResultLog");
}

static string get_select_sql_for_Device_info()
{
	return ("SELECT Id, DevId, DevType, Access, Model, SysParam, SysVersion, Vid, Pid, PhoneNumber, PluginTimes, TerminalId, Description, LogTimes \
			FROM DeviceInfoLog");
}

static string get_delete_sql_for_install_app_result(int dw_id)
{
	char sz_del[MAX_BUFF_LEN] = { 0 };
	snprintf(sz_del, MAX_BUFF_LEN, "DELETE FROM AppResultLog where Id = %d", dw_id);
	return string(sz_del);
}

static string get_delete_sql_for_device_info(int dw_id)
{
	char sz_del[MAX_BUFF_LEN] = { 0 };
	snprintf(sz_del, MAX_BUFF_LEN, "DELETE FROM DeviceInfoLog where Id = %d", dw_id);
	return string(sz_del);
}

static int insert_install_app_result_map(install_app_result_log_info_map* p_install_app_result_map, int dw_id, install_app_result_log_info &install_app_result_info)
{
	if (p_install_app_result_map == NULL)
		return CRG_INVALID_PARAM;

	pair<install_app_result_log_info_map::iterator, bool> insert_pair;

	insert_pair = p_install_app_result_map->insert(pair<int, install_app_result_log_info>(dw_id, install_app_result_info));

	return insert_pair.second ? CRG_SUCCESS : CRG_FAIL;
}

static int insert_device_info_map(device_log_info_map* p_dev_info_map, int dw_id, device_log_info &dev_info)
{
	if (p_dev_info_map == NULL)
		return CRG_INVALID_PARAM;

	pair<device_log_info_map::iterator, bool> insert_pair;

	insert_pair = p_dev_info_map->insert(pair<int, device_log_info>(dw_id, dev_info));

	return insert_pair.second ? CRG_SUCCESS : CRG_FAIL;
}


int push_device_info_to_database(device_log_info* p_dev_info)
{
	if (p_dev_info == NULL)
		return CRG_INVALID_PARAM;
	int ret = CRG_FAIL;
	pthread_mutex_lock(&device_info_database_mutex);
	sqlite3_stmt* pStmt = NULL;
	int rc = sqlite3_prepare_v2(gp_db , get_insert_sql_for_device_info().c_str() , get_insert_sql_for_device_info().length() , &pStmt , NULL);
	if(rc == SQLITE_OK)
	{
		sqlite3_bind_text(pStmt, 1, p_dev_info->sz_dev_id, strlen(p_dev_info->sz_dev_id), NULL);
		sqlite3_bind_int(pStmt, 2, p_dev_info->dw_dev_type);
		sqlite3_bind_int(pStmt, 3, (int)p_dev_info->b_access);
		sqlite3_bind_text(pStmt, 4, p_dev_info->sz_model, strlen(p_dev_info->sz_model), NULL);
		sqlite3_bind_text(pStmt, 5, p_dev_info->sz_sys_param, strlen(p_dev_info->sz_sys_param), NULL);
		sqlite3_bind_text(pStmt, 6, p_dev_info->sz_sys_version, strlen(p_dev_info->sz_sys_version), NULL);
		sqlite3_bind_int(pStmt, 7, strtol(p_dev_info->sz_vid, NULL, 16));
		sqlite3_bind_int(pStmt, 8, strtol(p_dev_info->sz_pid, NULL, 16));
		sqlite3_bind_text(pStmt, 9, p_dev_info->sz_phone_number, strlen(p_dev_info->sz_phone_number), NULL);
		sqlite3_bind_text(pStmt, 10, p_dev_info->sz_plugin_times, strlen(p_dev_info->sz_plugin_times), NULL);
		sqlite3_bind_text(pStmt, 11, p_dev_info->sz_terminal_id, strlen(p_dev_info->sz_terminal_id), NULL);
		sqlite3_bind_text(pStmt, 12, p_dev_info->sz_description, strlen(p_dev_info->sz_description), NULL);
		sqlite3_bind_text(pStmt, 13, p_dev_info->sz_log_times, strlen(p_dev_info->sz_log_times), NULL);
		if(sqlite3_step(pStmt) == SQLITE_DONE)
		{   
			ret = CRG_SUCCESS;
		}else{
			write_log("[LOGERROR] push_device_info_to_database faile");
		}       
	}
	if(pStmt)
	{
		sqlite3_finalize(pStmt);
	}
	pthread_mutex_unlock(&device_info_database_mutex);
	return ret;
}

int push_install_app_result_to_database(install_app_result_log_info* p_install_app_result_info)
{
	if (p_install_app_result_info == NULL)
		return CRG_INVALID_PARAM;

	int ret = CRG_FAIL;
	pthread_mutex_lock(&install_app_result_database_mutex);
	sqlite3_stmt* pStmt = NULL;
	int rc = sqlite3_prepare_v2(gp_db , get_insert_sql_for_install_app_result().c_str() ,get_insert_sql_for_install_app_result().length() , &pStmt , NULL);
	if(rc == SQLITE_OK)
	{
		sqlite3_bind_text(pStmt, 1, p_install_app_result_info->device_info.sz_dev_id, strlen(p_install_app_result_info->device_info.sz_dev_id), NULL);
		sqlite3_bind_int(pStmt, 2, p_install_app_result_info->device_info.dw_dev_type);
		sqlite3_bind_int(pStmt, 3, (int)p_install_app_result_info->device_info.b_access);
		sqlite3_bind_text(pStmt, 4, p_install_app_result_info->device_info.sz_model, strlen(p_install_app_result_info->device_info.sz_model), NULL);
		sqlite3_bind_text(pStmt, 5, p_install_app_result_info->device_info.sz_sys_param, strlen(p_install_app_result_info->device_info.sz_sys_param), NULL);
		sqlite3_bind_text(pStmt, 6, p_install_app_result_info->device_info.sz_sys_version, strlen(p_install_app_result_info->device_info.sz_sys_version), NULL);		
		sqlite3_bind_int(pStmt, 7, strtol(p_install_app_result_info->device_info.sz_vid, NULL, 16));
		sqlite3_bind_int(pStmt, 8, strtol(p_install_app_result_info->device_info.sz_pid, NULL, 16));
		sqlite3_bind_text(pStmt, 9, p_install_app_result_info->device_info.sz_phone_number, strlen(p_install_app_result_info->device_info.sz_phone_number), NULL);
		sqlite3_bind_text(pStmt, 10, p_install_app_result_info->device_info.sz_plugin_times, strlen(p_install_app_result_info->device_info.sz_plugin_times), NULL);
		sqlite3_bind_text(pStmt, 11, p_install_app_result_info->device_info.sz_terminal_id, strlen(p_install_app_result_info->device_info.sz_terminal_id), NULL);
		sqlite3_bind_text(pStmt, 12, p_install_app_result_info->device_info.sz_description, strlen(p_install_app_result_info->device_info.sz_description), NULL);
		sqlite3_bind_int(pStmt, 13, p_install_app_result_info->dw_app_source);
		if (p_install_app_result_info->vec_installed_app_time.size() > 0)
		{
			sqlite3_bind_text(pStmt, 14, p_install_app_result_info->vec_installed_app_time[0].sz_app_id, strlen(p_install_app_result_info->vec_installed_app_time[0].sz_app_id), NULL);
			sqlite3_bind_int(pStmt, 15, p_install_app_result_info->vec_installed_app_time[0].dw_time_interval);
		}

		sqlite3_bind_int(pStmt, 16, p_install_app_result_info->dw_result);
		sqlite3_bind_text(pStmt, 17, p_install_app_result_info->device_info.sz_log_times, strlen(p_install_app_result_info->device_info.sz_log_times), NULL);

		if(sqlite3_step(pStmt) == SQLITE_DONE)
		{   
			ret = CRG_SUCCESS;
		}else{
			write_log("[LOGERROR] get_insert_sql_for_install_app_result faile");
		}       
	}
	if(pStmt)
	{
		sqlite3_finalize(pStmt);
	}
	pthread_mutex_unlock(&install_app_result_database_mutex);
	return ret;
}

int pull_all_device_info_from_database(device_log_info_map* p_dev_info_map)
{
	if (p_dev_info_map == NULL)
		return CRG_INVALID_PARAM;

	int ret = CRG_FAIL;

	pthread_mutex_lock(&device_info_database_mutex);

	sqlite3_stmt* stmt = 0;  
	int rc = sqlite3_prepare_v2(gp_db, get_select_sql_for_Device_info().c_str(), -1, &stmt, 0); 
	if (rc != SQLITE_OK)  
	{  
		write_log("[LOGERROR] pull_all_device_info_from_database,  %s\n", sqlite3_errmsg(gp_db));    
		return ret;    
	}  

	//int  ncolumn = sqlite3_column_count(stmt);  

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{


		int dw_id = 0;
		const unsigned char* str = NULL;
		device_log_info DeviceInfo = {0};
		dw_id = sqlite3_column_int(stmt,0);
		str = sqlite3_column_text(stmt,1);
		if (str != NULL)
			memcpy(DeviceInfo.sz_dev_id, str, strlen((const char*)str));
		DeviceInfo.dw_dev_type = sqlite3_column_int(stmt,2);
		DeviceInfo.b_access = sqlite3_column_int(stmt,3);
		str = sqlite3_column_text(stmt,4);
		if (str != NULL)
			memcpy(DeviceInfo.sz_model, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,5);
		if (str != NULL)
			memcpy(DeviceInfo.sz_sys_param, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,6);
		if (str != NULL)
			memcpy(DeviceInfo.sz_sys_version, str, strlen((const char*)str));

		snprintf(DeviceInfo.sz_vid, sizeof(DeviceInfo.sz_vid), "%04x", sqlite3_column_int(stmt,7));
		snprintf(DeviceInfo.sz_pid, sizeof(DeviceInfo.sz_pid), "%04x", sqlite3_column_int(stmt,8));

		str = sqlite3_column_text(stmt,9);
		if (str != NULL)
			memcpy(DeviceInfo.sz_phone_number, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,10);
		if (str != NULL)
			memcpy(DeviceInfo.sz_plugin_times, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,11);
		if (str != NULL)
			memcpy(DeviceInfo.sz_terminal_id, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,12);
		if (str != NULL)
			memcpy(DeviceInfo.sz_description, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,13);
		if (str != NULL)
			memcpy(DeviceInfo.sz_log_times, str, strlen((const char*)str));

		ret = insert_device_info_map(p_dev_info_map, dw_id, DeviceInfo);
	}


	sqlite3_finalize(stmt); 
	pthread_mutex_unlock(&device_info_database_mutex);
	return ret;
}

int pull_all_install_app_result_from_database(install_app_result_log_info_map* p_install_app_result_map)
{
	if (p_install_app_result_map == NULL)
		return CRG_INVALID_PARAM;

	int ret = CRG_FAIL;


	pthread_mutex_lock(&install_app_result_database_mutex);

	sqlite3_stmt* stmt = 0;  
	int rc = sqlite3_prepare_v2(gp_db, get_select_sql_for_install_app_result().c_str(), -1, &stmt, 0); 
	if (rc != SQLITE_OK)  
	{  
		write_log("[LOGERROR] pull_all_install_app_result_from_database,  %s\n", sqlite3_errmsg(gp_db));    
		return ret;    
	}  

	//int  ncolumn = sqlite3_column_count(stmt);  

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int dw_id = 0;
		const unsigned char* str = NULL;
		install_app_result_log_info AppResultInfo;
		clear_install_app_result_log_info(&AppResultInfo);

		dw_id = sqlite3_column_int(stmt,0);
		str = sqlite3_column_text(stmt,1);
		if (str != NULL)
			memcpy(AppResultInfo.device_info.sz_dev_id, str, strlen((const char*)str));
		AppResultInfo.device_info.dw_dev_type = sqlite3_column_int(stmt,2);
		AppResultInfo.device_info.b_access = sqlite3_column_int(stmt,3);
		str = sqlite3_column_text(stmt,4);
		if (str != NULL)
			memcpy(AppResultInfo.device_info.sz_model, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,5);
		if (str != NULL)
			memcpy(AppResultInfo.device_info.sz_sys_param, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,6);
		if (str != NULL)
			memcpy(AppResultInfo.device_info.sz_sys_version, str, strlen((const char*)str));

		snprintf(AppResultInfo.device_info.sz_vid, sizeof(AppResultInfo.device_info.sz_vid), "%04x", sqlite3_column_int(stmt,7));
		snprintf(AppResultInfo.device_info.sz_pid, sizeof(AppResultInfo.device_info.sz_pid), "%04x", sqlite3_column_int(stmt,8));

		str = sqlite3_column_text(stmt,9);
		if (str != NULL)
			memcpy(AppResultInfo.device_info.sz_phone_number, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,10);
		if (str != NULL)
			memcpy(AppResultInfo.device_info.sz_plugin_times, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,11);
		if (str != NULL)
			memcpy(AppResultInfo.device_info.sz_terminal_id, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,12);
		if (str != NULL)
			memcpy(AppResultInfo.device_info.sz_description, str, strlen((const char*)str));
		AppResultInfo.dw_app_source = sqlite3_column_int(stmt, 13);
		str = sqlite3_column_text(stmt,14);
		if (str != NULL)
		{
			install_app_time_info Info = { 0 };
			memcpy(Info.sz_app_id, str, strlen((const char*)str));
			Info.dw_time_interval = sqlite3_column_int(stmt,15);
			AppResultInfo.vec_installed_app_time.push_back(Info);
		}

		AppResultInfo.dw_result = sqlite3_column_int(stmt,16);
		str = sqlite3_column_text(stmt,17);
		if (str != NULL)
			memcpy(AppResultInfo.device_info.sz_log_times, str, strlen((const char*)str));

		ret = insert_install_app_result_map(p_install_app_result_map, dw_id, AppResultInfo);
		clear_install_app_result_log_info(&AppResultInfo);
	}
	sqlite3_finalize(stmt); 
	pthread_mutex_unlock(&install_app_result_database_mutex);
	return ret;
}

int remove_install_app_result_from_database(int dw_id)
{
	int ret = CRG_FAIL;
	pthread_mutex_lock(&install_app_result_database_mutex);
	char* err_msg; 
	char sz_del[MAX_BUFF_LEN] = { 0 };
	snprintf(sz_del, MAX_BUFF_LEN, "%s", get_delete_sql_for_install_app_result(dw_id).c_str());
	int res= sqlite3_exec(gp_db , sz_del , 0 , 0 , &err_msg);  
	//暂时先不删除SQLite中的数据
	if (res != SQLITE_OK)  
	{  
		write_log("[LOGERROR] 执行SQL 出错. %s",err_msg);
		return CRG_FAIL;  
	}else{
		ret = CRG_SUCCESS;
	}
	pthread_mutex_unlock(&install_app_result_database_mutex);
	ret = CRG_SUCCESS;
	return ret;
}

int remove_device_info_from_database(int dw_id)
{
	int ret = CRG_FAIL;
	pthread_mutex_lock(&device_info_database_mutex);
	char* err_msg; 
	char sz_del[MAX_BUFF_LEN] = { 0 };
	snprintf(sz_del, MAX_BUFF_LEN, "%s", get_delete_sql_for_device_info(dw_id).c_str());
	int res= sqlite3_exec(gp_db , sz_del , 0 , 0 , &err_msg);  
	//暂时先不删除SQLite中的数据
	if (res != SQLITE_OK)  
	{  
		write_log("[LOGERROR] 执行SQL 出错. %s",err_msg);
		return CRG_FAIL;  
	}else{
		ret = CRG_SUCCESS;
	}
	pthread_mutex_unlock(&device_info_database_mutex);
	return ret;
}

uint32_t init_database(char* p_db_name)
{
	char sz_db_filename[MAX_BUFF_LEN] = { 0 };
	char sz_module_path[MAX_BUFF_LEN] = { 0 };

	if(get_share_module_path(sz_module_path, MAX_PATH) != CRG_SUCCESS)
		readlink("/proc/self/exe", sz_module_path, MAX_PATH);
	if (strrchr(sz_module_path, '/') == NULL) return CRG_NOT_FOUND;
	strrchr(sz_module_path, '/')[0] = '\0';
	snprintf(sz_db_filename, MAX_BUFF_LEN, "%s/%s", sz_module_path, p_db_name);
	
	int res = sqlite3_open(sz_db_filename, &gp_db);
	if (res != SQLITE_OK){  
		write_log("[LOGERROR] can't open database: %s",sqlite3_errmsg(gp_db));
		sqlite3_close(gp_db); 
		return CRG_FAIL;
	} 
	if (create_database(INSTALL_APP_RESULT_DATABASE_TYPE) == CRG_SUCCESS
			&& create_database(DEVICE_INFO_DATABASE_TYPE) == CRG_SUCCESS)
	{
		pthread_mutex_init(&device_info_database_mutex, NULL);
		pthread_mutex_init(&install_app_result_database_mutex, NULL);
		return CRG_SUCCESS;
	}
	else 
	{
		sqlite3_close(gp_db);
		return CRG_FAIL;
	}
}

void destory_database()
{
	if(gp_db != NULL) sqlite3_close(gp_db);
	pthread_mutex_destroy(&device_info_database_mutex);
	pthread_mutex_destroy(&install_app_result_database_mutex);
}
