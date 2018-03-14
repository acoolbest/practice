#ifndef _ANDROID_SHARE_MEM_MANAGER_H_
#define _ANDROID_SHARE_MEM_MANAGER_H_

#define SHARE_MEM_SIZE							(2048 * 100)			// 200k

uint32_t init_all_android_share_mem_model();
uint32_t destory_all_android_share_mem_model();

uint32_t init_android_list_share_mem_model();
uint32_t destory_android_list_share_mem_model();

uint32_t write_android_list_data_to_share_mem(const char * json_str, uint32_t len);
uint32_t read_android_list_data_from_share_mem(char * out_json, uint32_t & len);

uint32_t write_android_pid_data_to_share_mem(uint32_t pid_num);
uint32_t read_android_pid_data_from_share_mem(uint32_t & pid_num);

uint32_t make_json_for_android_list(vector<android_dev_info_stru>* vec_android_dev_info, string & out_json_str);
uint32_t parse_json_for_android_list(string json_str, vector<android_dev_info_stru>* out_vec_android_dev_info);

#endif // _ANDROID_SHARE_MEM_MANAGER_H_
