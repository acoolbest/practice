#ifndef _INIFILE_H__
#define _INIFILE_H__

uint32_t get_private_profile_int(const char* lp_app_name, const char* lp_key_name, uint32_t n_default, const char* lp_filename);
uint32_t get_private_profile_string(const char* lp_app_name, const char* lp_key_name, const char* lp_default, char* lp_return_string, uint32_t n_size, const char* lp_filename);

#endif
