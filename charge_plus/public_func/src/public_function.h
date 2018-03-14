#ifndef _PUBLIC_FUNCTION_H_
#define _PUBLIC_FUNCTION_H_

#include <tuple>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#define MAXLEN_LOGRECORD 500*1024

using namespace std;


void write_log(const char *fm, ...);

uint32_t get_app_json_config_file_path(string & out_file_path);
uint32_t get_path_of_userinfo(char* file_path, uint32_t len);
uint32_t get_path_of_config_json(char* file_path, uint32_t len);
uint32_t get_path_of_net_traffic(char* file_path, uint32_t len);
uint32_t get_path_of_net_traffic_sh(char* file_path, uint32_t len);

uint32_t is_exist_file(const char* file_name);
uint32_t is_exist_dir(const char* dir_name);
uint32_t read_file(const char * file_name, string & str_content);
uint32_t find_string_from_vector(string find_str, vector<string>* vec_str, string::size_type src_index);
void string_split(string s, string delim, vector<string>* ret);
string get_current_time(void);
uint32_t insert_string_map(map<string, string>* out, string &key, string &val);
uint32_t is_string_exist(map<string, string> src_map, string find_key, string& out_val);
void clear_map(map<string, string> & src_map);
void write_file(char *path, string js);
bool check_sum(char* pFrame, int8_t len, int8_t& sum);
void clear_install_app_result_log_info(install_app_result_log_info* pinfo);
char *strlwr(char *str);
uint32_t get_share_module_path(char* pModulePath, uint32_t len);
uint32_t get_current_path(char* out_path, uint32_t len);
void get_cur_time(char* cur_time, bool btype);
void color_print(const char *fm, ...);

uint32_t convert_uint_to_string(char* dest, uint32_t dest_len, uint8_t* src, uint32_t src_len);

uint32_t run_cmd(string cmd, string& out_str);

#endif // _PUBLIC_FUNCTION_H_
