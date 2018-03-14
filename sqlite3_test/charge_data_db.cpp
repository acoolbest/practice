#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <map> 
#include <time.h>
#include <sqlite3.h>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "charge_data_db.h"

using namespace std;
/*
ÊÂÎñÐÔ
#define START_TRANSACTION() sqlite3_exec(db, "begin transaction;", NULL, NULL, NULL)  
#define END_TRANSACTION()   sqlite3_exec(db, "commit transaction;", NULL, NULL, NULL)  
*/
#define CHARGE_DATA_DB_NAME	"ChargeData.db3"
pthread_mutex_t	charge_data_mutex;
sqlite3 * gp_charge_data_db;

static uint32_t create_table()
{
	#if 1
	char sz_sqlcmd[] = "create table if not exists ChargeData ("
	"id integer primary key, "
	"timestart int64 null, "
	"chargetime int null, "
	"qrcode text null, "
	"hostname text null, "
	"commandid text null, "
	"timenow int64 null, "
	"chargeresult int null, "
	"errorcode int null);";
	#endif
	char* err_msg;

	int res= sqlite3_exec(gp_charge_data_db, sz_sqlcmd, 0, 0, &err_msg);  

	if (res != SQLITE_OK)  
	{  
		write_log("[LOGERROR]Execute create table SQL error: %s", err_msg);
		return CRG_FAIL;  
	}else{
		return CRG_SUCCESS;
	}
}

static int insert_chargeing_data_map(charge_data_stru_map* p_charge_data_map, int dw_id, charge_data_stru & charge_data_info)
{
	if (p_charge_data_map == NULL)
		return CRG_INVALID_PARAM;

	pair<charge_data_stru_map::iterator, bool> insert_pair;

	insert_pair = p_charge_data_map->insert(pair<int, charge_data_stru>(dw_id, charge_data_info));

	return insert_pair.second ? CRG_SUCCESS : CRG_FAIL;
}

uint32_t select_all_chargeing_data_from_database(charge_data_stru_map* p_charge_data_map)
{
	if (p_charge_data_map == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_SUCCESS;

	pthread_mutex_lock(&charge_data_mutex);

	char sz_sqlcmd[MAX_BUFF_LEN] = { 0 };
	
	snprintf(sz_sqlcmd, MAX_BUFF_LEN, 
		"SELECT * FROM ChargeData where timestart+chargetime*60>%ld;", time(NULL));
	
	sqlite3_stmt* stmt = 0;
	
	int rc = sqlite3_prepare_v2(gp_charge_data_db, sz_sqlcmd, -1, &stmt, 0);
	if (rc != SQLITE_OK)  
	{  
		write_log("[LOGERROR] pull_chargeing_data_from_database,  %s\n", sqlite3_errmsg(gp_charge_data_db));    
		return CRG_FAIL;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int dw_id = 0;
		const unsigned char* str = NULL;
		charge_data_stru charge_data_info;
		memset(&charge_data_info, 0, sizeof(charge_data_stru));
		
		dw_id = sqlite3_column_int(stmt,0);
		charge_data_info.timestart = sqlite3_column_int64(stmt,1);
		charge_data_info.chargetime = (uint16_t)sqlite3_column_int(stmt,2);
		str = sqlite3_column_text(stmt,3);
		if (str != NULL)
			memcpy(charge_data_info.qrcode, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,4);
		if (str != NULL)
			memcpy(charge_data_info.hostname, str, strlen((const char*)str));
		str = sqlite3_column_text(stmt,5);
		if (str != NULL)
			memcpy(charge_data_info.commandid, str, strlen((const char*)str));
		charge_data_info.timenow = sqlite3_column_int64(stmt,6);
		charge_data_info.chargeresult = sqlite3_column_int(stmt,7);
		charge_data_info.errorcode = sqlite3_column_int(stmt,8);
		
		if(insert_chargeing_data_map(p_charge_data_map, dw_id, charge_data_info) != CRG_SUCCESS){
			ret = CRG_FAIL;
			break;
		}
	}
	sqlite3_finalize(stmt); 
	pthread_mutex_unlock(&charge_data_mutex);
	return ret;
}

uint32_t insert_charge_data_to_database(charge_data_stru charge_data_info)
{
	pthread_mutex_lock(&charge_data_mutex);
	
	uint32_t ret = CRG_SUCCESS;

	char sz_sqlcmd[MAX_BUFF_LEN] = { 0 };
	
	snprintf(sz_sqlcmd, MAX_BUFF_LEN, 
		"INSERT INTO ChargeData values(NULL, %ld, %d, '%s', '%s', '%s', %ld, %d, %d);", 
		charge_data_info.timestart,
		charge_data_info.chargetime,
		charge_data_info.qrcode,
		charge_data_info.hostname,
		charge_data_info.commandid,
		charge_data_info.timenow,
		charge_data_info.chargeresult,
		charge_data_info.errorcode);
	
	char* err_msg;
	
	int res= sqlite3_exec(gp_charge_data_db, sz_sqlcmd, 0, 0, &err_msg); 

	if(res != SQLITE_OK){
		write_log("[LOGERROR]Execute insert SQL error: %s", err_msg);
		ret = CRG_FAIL;
	}
	
	pthread_mutex_unlock(&charge_data_mutex);
	
	return ret;
}

uint32_t init_charge_data_db()
{
	char sz_db_filename[MAX_BUFF_LEN] = { 0 };
	char sz_module_path[MAX_BUFF_LEN] = { 0 };

	if(get_share_module_path(sz_module_path, MAX_PATH) != CRG_SUCCESS)
		readlink("/proc/self/exe", sz_module_path, MAX_PATH);
	if (strrchr(sz_module_path, '/') == NULL) return CRG_NOT_FOUND;
	strrchr(sz_module_path, '/')[0] = '\0';
	snprintf(sz_db_filename, MAX_BUFF_LEN, "%s/%s", sz_module_path, CHARGE_DATA_DB_NAME);
	
	int res = sqlite3_open(sz_db_filename, &gp_charge_data_db);
	if (res != SQLITE_OK){
		write_log("[LOGERROR] can't open charge data database: %s",sqlite3_errmsg(gp_charge_data_db));
		sqlite3_close(gp_charge_data_db); 
		return CRG_FAIL;
	} 
	if (create_table() == CRG_SUCCESS)
	{
		pthread_mutex_init(&charge_data_mutex, NULL);
		return CRG_SUCCESS;
	}
	else 
	{
		sqlite3_close(gp_charge_data_db);
		return CRG_FAIL;
	}
}

uint32_t destory_charge_data_db()
{
	if(gp_charge_data_db != NULL) sqlite3_close(gp_charge_data_db);
	pthread_mutex_destroy(&charge_data_mutex);
	return CRG_SUCCESS;
}
