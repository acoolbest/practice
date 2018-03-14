#ifndef _DATA_BASE_H_
#define _DATA_BASE_H_

typedef map <int, install_app_result_log_info> install_app_result_log_info_map;
typedef map <int, device_log_info> device_log_info_map;

uint32_t init_database(char* p_db_name);
void destory_database();

int push_device_info_to_database(device_log_info* p_dev_info);
int pull_all_device_info_from_database(device_log_info_map* p_dev_info_map);
int remove_device_info_from_database(int dw_id);

int push_install_app_result_to_database(install_app_result_log_info* p_install_app_result_info);
int pull_all_install_app_result_from_database(install_app_result_log_info_map* p_install_app_result_map);
int remove_install_app_result_from_database(int dw_id);

void clear_install_app_result_log_info(install_app_result_log_info* pinfo);

#endif // _DATA_BASE_H_
