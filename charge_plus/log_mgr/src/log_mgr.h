#ifndef _LOG_MANAGER_H_
#define _LOG_MANAGER_H_

int init_log_manager(char* p_db_name);
int destory_log_manager();
int send_device_info_to_server(device_log_info* p_dev_info,bool b_backup_db_if_fail = true);
int send_install_app_result_to_server(install_app_result_log_info* p_install_app_result_info,bool b_backup_db_if_fail = true);
int send_heartbeat_info_to_server(char* p_terminal, char* p_version, char* p_net_new);

#endif // _LOG_MANAGER_H_
