#ifndef _ADB_CONTROL_H_
#define _ADB_CONTROL_H_

uint32_t adb_start();
uint32_t adb_close();
uint32_t adb_enum_dev_list(string& out_str);
uint32_t adb_get_state(string dev_path, string & out_str);
uint32_t adb_shell_get_prop(string dev_path, string & out_str);
uint32_t adb_set_install_location(string dev_path, uint32_t location);
uint32_t adb_enum_installed_app_list(string dev_path, string & out_str);
uint32_t adb_install_one_app(string dev_path, string apk_path, string & out_err_str);
uint32_t adb_run_one_app(string dev_path, string apk_package);

uint32_t adb_enum_dev_list_from_android_remote(FILE* &out_fp);
uint32_t kill_for_android_remote(uint32_t remote_pid);
uint32_t destory_for_android_remote(FILE* &out_fp);

#endif // _ADB_CONTROL_H_
