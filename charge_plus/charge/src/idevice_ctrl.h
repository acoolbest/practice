#ifndef _IDEVICE_CONTROL_H_
#define _IDEVICE_CONTROL_H_

uint32_t idevice_start();
uint32_t idevice_close();

uint32_t idevice_enum_dev_list(string& out_str);
uint32_t idevice_get_state(string dev_path, string & out_str);
uint32_t idevice_shell_get_prop(string dev_id, string & out_str);

uint32_t idevice_enum_dev_list_from_ios_remote(FILE* &out_fp);
uint32_t kill_for_ios_remote(uint32_t remote_pid);
uint32_t destory_for_ios_remote(FILE* &out_fp);

#endif // _IDEVICE_CONTROL_H_
