#ifndef _CHARGE_DATA_DB_H_
#define _CHARGE_DATA_DB_H_
typedef struct _CHARGE_DATA_STRUCT_
{
	long timestart;
	uint16_t chargetime;
	char qrcode[MAX_BUFF_LEN];
	char hostname[MAX_BUFF_LEN];
	char commandid[MAX_BUFF_LEN];
	long timenow;
	uint32_t chargeresult;
	uint32_t errorcode;
}charge_data_stru, *p_charge_data_stru;
typedef map <int, charge_data_stru> charge_data_stru_map;


uint32_t init_charge_data_db();
uint32_t destory_charge_data_db();
uint32_t insert_charge_data_to_database(charge_data_stru charge_data_info);
uint32_t select_all_chargeing_data_from_database(charge_data_stru_map* p_charge_data_map);




#endif // _CHARGE_DATA_DB_H_


