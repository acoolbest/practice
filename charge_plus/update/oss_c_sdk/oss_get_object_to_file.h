#ifndef OSS_SAMPLE_UTIL_H
#define OSS_SAMPLE_UTIL_H
#include <stdbool.h>
#include "aos_http_io.h"
#include "aos_string.h"
#include "aos_transport.h"
#include "oss_define.h"
OSS_CPP_START
void init_sample_config(oss_config_t *config,char* OSS_ENDPOINT,char* ACCESS_KEY_ID,char* ACCESS_KEY_SECRET,int is_cname);
void init_sample_request_options(oss_request_options_t *options,char* OSS_ENDPOINT,char* ACCESS_KEY_ID,char* ACCESS_KEY_SECRET,int is_cname);
void release_oss();
bool init_oss();
int64_t get_file_size(const char *file_path);
void get_object_to_file();
int oss_download(const char* remotepath, const char* key_id, const char* key_secret, const char* localpath, const char * localpath_utf8,
	long timeout,char* ss);
int64_t head_object_by_url(const char * signed_url,char * ss);
int get_object_by_signed_url(const char *signed_url,const char *filepath,long timeout,char * ss);
OSS_CPP_END

#endif
