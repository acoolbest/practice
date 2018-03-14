#ifndef _LOGDATA_H_
#define _LOGDATA_H_

int init_log_model(char* p_db_name);
int destory_log_model();
int send_device_info(int dw_device_type, void* p_devinfo);
int send_install_app_result(uint32_t dw_result, uint32_t dw_device_type, uint32_t dw_app_source, char* p_app_id, uint32_t dw_interval, void* p_devinfo);

#endif // _LOGDATA_H_
