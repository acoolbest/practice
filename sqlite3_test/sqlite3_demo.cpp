/* ************************************************************************
 *       Filename:  sqlite3_demo.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2016年10月28日 11时27分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <string.h>
#include <stdio.h>
#include <map>
#include <sqlite3.h>
#include <stdint.h>
#include <iostream>

using namespace std;

#define INSTALL_APP_RESULT_DATABASE_TYPE	0x01
#define DEVICE_INFO_DATABASE_TYPE			0x02
#define	CRG_FAIL							0x0000		
#define	CRG_SUCCESS							0x0001
#define	CRG_INVALID_PARAM					0x0002
#define	MAX_BUFF_LEN						260
#define SQLITE3_DEMO						"sqlite3_demo.db3"

typedef struct _INSTALL_APP_RESULT_LOG_STRUCT_
{
	uint32_t								dw_dev_type;
	char									sz_log_times[MAX_BUFF_LEN];
}install_app_result_log_info, *p_install_app_result_log_info;

typedef struct _DEVICE_LOG_INFO_STRUCT_
{
	uint32_t								dw_dev_type;
	char									sz_log_times[MAX_BUFF_LEN];
}device_log_info, *p_device_log_info;

typedef map <int, install_app_result_log_info> install_app_result_log_info_map;
typedef map <int, device_log_info> device_log_info_map;

pthread_mutex_t 							device_info_database_mutex;
pthread_mutex_t 							install_app_result_database_mutex;
sqlite3 * gp_db; 


static string get_insert_sql_for_install_app_result()
{
	return ("insert into AppResultLog(DevType, LogTimes) \
			values(?,?)");
}

static string get_insert_sql_for_device_info()
{
	return ("insert into DeviceInfoLog(DevType, LogTimes) \
			values(?,?)");
}

static string get_select_sql_for_install_app_result()
{
	return ("SELECT Id, DevType, LogTimes \
			FROM AppResultLog");
}

static string get_select_sql_for_Device_info()
{
	return ("SELECT Id, DevType, LogTimes \
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

static int create_database(int dw_type)
{
	string str;
	switch (dw_type)
	{
		case INSTALL_APP_RESULT_DATABASE_TYPE:
			str = "create table if not exists AppResultLog("
				"Id INTEGER  primary key,"
				"DevType int null, "
				"LogTimes string null)";

			break;
		case DEVICE_INFO_DATABASE_TYPE:
			str = "create table if not exists DeviceInfoLog("
				"Id INTEGER  primary key,"
				"DevType int null, "
				"LogTimes string null)";
			break;
		default:
			break;
	}

	char* err_msg;  

	int res= sqlite3_exec(gp_db , str.c_str() , 0 , 0 , &err_msg);  

	if (res != SQLITE_OK) {  
		printf("Execute SQL error : %s\n",err_msg);
		return CRG_FAIL;  
	}
	
	return CRG_SUCCESS;
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
		sqlite3_bind_int(pStmt, 1, p_dev_info->dw_dev_type);
		sqlite3_bind_text(pStmt, 2, p_dev_info->sz_log_times, strlen(p_dev_info->sz_log_times), NULL);
		if(sqlite3_step(pStmt) == SQLITE_DONE)
		{   
			ret = CRG_SUCCESS;
		}else{
			printf("push_device_info_to_database faile\n");
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
		sqlite3_bind_int(pStmt, 1, p_install_app_result_info->dw_dev_type);
		sqlite3_bind_text(pStmt, 2, p_install_app_result_info->sz_log_times, strlen(p_install_app_result_info->sz_log_times), NULL);

		if(sqlite3_step(pStmt) == SQLITE_DONE)
		{   
			ret = CRG_SUCCESS;
		}else{
			printf("push_insert_sql_for_install_app_result faile\n");
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
		printf("pull_all_device_info_from_database,  %s\n", sqlite3_errmsg(gp_db));    
		return ret;    
	}  
	
	//int  ncolumn = sqlite3_column_count(stmt);  
	
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int dw_id = 0;
		const unsigned char* str = NULL;
		device_log_info DeviceInfo = {0};
		dw_id = sqlite3_column_int(stmt,0);
		DeviceInfo.dw_dev_type = sqlite3_column_int(stmt,1);
		str = sqlite3_column_text(stmt,2);
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
		printf("[LOGERROR] pull_all_install_app_result_from_database,  %s\n", sqlite3_errmsg(gp_db));    
		return ret;    
	}  

	//int  ncolumn = sqlite3_column_count(stmt);  

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int dw_id = 0;
		const unsigned char* str = NULL;
		install_app_result_log_info AppResultInfo = {0};

		dw_id = sqlite3_column_int(stmt,0);
		AppResultInfo.dw_dev_type = sqlite3_column_int(stmt,1);
		str = sqlite3_column_text(stmt,2);
		if (str != NULL)
			memcpy(AppResultInfo.sz_log_times, str, strlen((const char*)str));

		ret = insert_install_app_result_map(p_install_app_result_map, dw_id, AppResultInfo);
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

	if (res != SQLITE_OK)  
	{  
		printf("Execute SQL error : %s\n",err_msg);
		return CRG_FAIL;  
	}else{
		ret = CRG_SUCCESS;
	}
	pthread_mutex_unlock(&install_app_result_database_mutex);

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
	if (res != SQLITE_OK)  
	{  
		printf("Execute SQL error : %s\n",err_msg);
		return CRG_FAIL;  
	}else{
		ret = CRG_SUCCESS;
	}
	pthread_mutex_unlock(&device_info_database_mutex);
	return ret;
}

uint32_t init_database(char* p_db_name)
{	
	int res = sqlite3_open(p_db_name, &gp_db);
	if (res != SQLITE_OK){  
		printf("can't open database: %s\n",sqlite3_errmsg(gp_db));
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

int main()
{
	if(init_database((char *)SQLITE3_DEMO) != CRG_SUCCESS) return 1;
	
	char dev_log_time[] = "2016-10-28 14:12:16";
	char install_log_time[] = "2016-10-28 14:12:18";
	
	device_log_info p_dev_info;
	p_dev_info.dw_dev_type = 1;
	strcpy(p_dev_info.sz_log_times, dev_log_time);
	
	install_app_result_log_info p_install_app_result_info;
	p_install_app_result_info.dw_dev_type = 2;
	strcpy(p_install_app_result_info.sz_log_times, install_log_time);

//insert
	for(int i=0; i<3; i++)
	{
		push_install_app_result_to_database(&p_install_app_result_info);
		push_device_info_to_database(&p_dev_info);
	}
	getchar();
//select
	install_app_result_log_info_map install_app_result_map;
	
	if (pull_all_install_app_result_from_database(&install_app_result_map) == CRG_SUCCESS)
	{
		install_app_result_log_info_map::iterator iter;
		for (iter = install_app_result_map.begin(); iter != install_app_result_map.end();)
		{
//delete
			remove_install_app_result_from_database(iter->first);
			printf("delete install from sqlite3 : %d, %s\n", iter->second.dw_dev_type, iter->second.sz_log_times);
			install_app_result_map.erase(iter++);
			getchar();
		}
		install_app_result_map.clear();
		install_app_result_log_info_map(install_app_result_map).swap(install_app_result_map);
	}
	
	getchar();
//select
	device_log_info_map dev_info_map;

	if (pull_all_device_info_from_database(&dev_info_map) == CRG_SUCCESS)
	{
		device_log_info_map::iterator iter1;
		for (iter1 = dev_info_map.begin(); iter1 != dev_info_map.end();)
		{
//delete
			remove_device_info_from_database(iter1->first);
			printf("delete device from sqlite3 : %d, %s\n", iter1->second.dw_dev_type, iter1->second.sz_log_times);
			dev_info_map.erase(iter1++);
			getchar();
		}
		dev_info_map.clear();
		device_log_info_map(dev_info_map).swap(dev_info_map);
	}
	
	getchar();
	
	destory_database();
	return 0;
}