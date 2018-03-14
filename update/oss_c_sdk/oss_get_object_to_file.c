#include <string.h>
#include <sys/stat.h>
#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "oss_auth.h"
#include "oss_util.h"
#include "oss_api.h"
#include "oss_get_object_to_file.h"
#define OSS_CONTENT_LENGTH	"Content-Length"
#define DOWN_FILE_PART_SIZE (200000000L)
#define OSS_DOWN_TIMEOUT_RET   CURLE_OPERATION_TIMEDOUT
#define OSS_DOWN_OK        CURLE_OK
bool init_oss()
{
	if (aos_http_io_initialize(NULL, 0) != AOSE_OK)
	{
		return FALSE;
	}
	return TRUE;
}
void release_oss()
{
	aos_http_io_deinitialize();
}
int64_t head_object_by_url(const char * signed_url,char * ss)
{
    aos_pool_t *p = NULL;
    aos_string_t url;
    oss_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_http_request_t *req = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    char *content_length_str = NULL;
    char *object_type = NULL;
    int64_t content_length = 0;
    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    req = aos_http_request_create(p);
    req->method = HTTP_HEAD;
    headers = aos_table_make(p, 0);
    options->config = oss_config_create(options->pool);
    options->ctl = aos_http_controller_create(options->pool, 0);
    resp_headers = NULL;
    aos_str_set(&url, signed_url);
    printf("signed get url : %s\n", signed_url);
    s = oss_head_object_by_url(options,&url, headers, &resp_headers);
    if (aos_status_is_ok(s)) 
    {
        content_length_str = (char*)apr_table_get(resp_headers, OSS_CONTENT_LENGTH);
        if (content_length_str != NULL) 
        {
            content_length = atol(content_length_str);
        }

        object_type = (char*)apr_table_get(resp_headers, OSS_OBJECT_TYPE);
        
        sprintf(ss,"head object succeeded, object type:%s, content_length:%ld", 
               object_type, content_length);
    } 
    else 
    {
        sprintf(ss,"head object failed!code=%d,error_code:%s,err_msg:%s,req_id:%s",s->code,s->error_code,s->error_msg,s->req_id);
    }
    aos_pool_destroy(p);
    return content_length;
}

int64_t head_object(char* OSS_ENDPOINT,char* ACCESS_KEY_ID,char* ACCESS_KEY_SECRET,char* BUCKET_NAME,char* OBJECT_NAME)
{
	int64_t ret = 0;
    aos_pool_t *p = NULL;     
	aos_string_t bucket;     
	aos_string_t object;
    oss_request_options_t *options = NULL;     
	aos_table_t *headers = NULL;     
	aos_table_t *resp_headers = NULL;     
	aos_status_t *s = NULL;     
	char *content_length_str = NULL;     
	char *object_type = NULL;     
	int64_t content_length = 0;
    aos_pool_create(&p, NULL);
    /* 创建并初始化options */
    options = oss_request_options_create(p);     
	init_sample_request_options(options,OSS_ENDPOINT,ACCESS_KEY_ID,ACCESS_KEY_SECRET,0);
    /* 初始化参数 */     
	aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);
	headers = aos_table_make(p, 0) ;
    /* 获取文件元数据 */
	s = oss_head_object(options, &bucket, &object, headers, &resp_headers);
    if (aos_status_is_ok(s)) {
        /* 获取文件长度 */
        content_length_str = (char*) apr_table_get(resp_headers, OSS_CONTENT_LENGTH);         
		if (content_length_str != NULL) {
            content_length = atoll(content_length_str);
        }
        /* 获取文件的类型 */         
		object_type = (char*) apr_table_get(resp_headers, OSS_OBJECT_TYPE);
        printf("head object succeeded, object type:%s, content_length:%ld\n",                
			object_type, content_length);
		ret = content_length;
    } else {
        printf("head object failed\n") ;
    }
    aos_pool_destroy(p);
	return ret;
}

int oss_download(const char* remotepath, const char* key_id, const char* key_secret, const char* localpath, const char * localpath_utf8,long timeout,char *ss)
{
	char OSS_ENDPOINT[512] = "";
	char ACCESS_KEY_ID[260] = "";
	char ACCESS_KEY_SECRET[260] ="";
	char BUCKET_NAME[512] = "";
	char OBJECT_NAME[512] = "";
	int64_t down_end = 0;
	int64_t download_file_size = 0;/*本地文件大小*/
	int64_t downFilePartSize = 0;   /*每次下载文件部分大小*/
	int64_t content_length = 0;    /*服务器端文件大小*/
	strcpy(ACCESS_KEY_ID, key_id);
	strcpy(ACCESS_KEY_SECRET, key_secret);
	int ret = -1;
	char* phttp = "http://";
	if (0 == memcmp(remotepath, phttp, strlen(phttp)))
	{
		strcpy(BUCKET_NAME, remotepath + strlen("http://"));
	}
    else if(strstr(remotepath,"https://"))
        strcpy(BUCKET_NAME,remotepath+strlen("https://"));
	else
    {
        printf("[%s] not a aliyun url!\n",remotepath);
        return ret;
    }		
	char * p_next=strchr(BUCKET_NAME,'.');
    if(!p_next)
    {
        printf("[%s]  has no OSS_ENDPOINT!\n",BUCKET_NAME);
		return ret;
    }
    *p_next='\0';
    strcpy(OSS_ENDPOINT,p_next+1);
    p_next=strchr(OSS_ENDPOINT,'/');
    if(!p_next)
    {
        printf("[%s] has no OBJECT_NAME!\n",OSS_ENDPOINT);
		return ret;

    }
    *p_next='\0';
    strcpy(OBJECT_NAME,p_next+1);
    //printf("%s,%s,%s\n",OSS_ENDPOINT,ACCESS_KEY_ID,ACCESS_KEY_SECRET);
	content_length = head_object(OSS_ENDPOINT, ACCESS_KEY_ID, ACCESS_KEY_SECRET,BUCKET_NAME,OBJECT_NAME);
	if(!content_length) 
    {
		return ret;
	}
	download_file_size = get_file_size(localpath);
	printf("download_file_size is %ld\n",download_file_size);
	if(download_file_size < 0) download_file_size = 0;
    if(content_length==download_file_size) 
	{
		return OSS_DOWN_OK;
	}
	else if (content_length < download_file_size)
	{
		char cmd [256]="";
		sprintf(cmd,"rm  \"%s\" -f",localpath);
		system(cmd);
		download_file_size = 0;
	}
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    oss_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *params = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    aos_string_t file;
    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    init_sample_request_options(options,OSS_ENDPOINT,ACCESS_KEY_ID,ACCESS_KEY_SECRET, is_cname);
    options->ctl = aos_http_controller_create(options->pool, 0);
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);
    headers = aos_table_make(p, 0);
	char range_bytes[255] = "";
    aos_str_set(&file, localpath_utf8);
	/* 根据孙健周阳提出的修改意见，做出修改，每次只下载一部分，就退出检查，160615,何	*/
	/* 每次部分下载大小根据Timeout按1秒1M换算											*/
	downFilePartSize = 1000000LL * timeout;
	down_end =downFilePartSize>0 && content_length- download_file_size > downFilePartSize ?
		download_file_size + downFilePartSize - 1 : content_length - 1;
	sprintf(range_bytes, "bytes=%ld-%ld", download_file_size, down_end);
    //printf("rang_bytes: %s,downFilePartSize=%ld,timeout=%ld\n",range_bytes,downFilePartSize,timeout);
	/* 设置Range，读取文件的指定范围，bytes=20-100包括第20和第100个字符 */
	apr_table_set(headers, "Range", range_bytes);
	s = oss_get_object_to_file(options, &bucket, &object, headers, 
							   params, &file, &resp_headers);

	if (aos_status_is_ok(s)) 
	{
		ret = down_end + 1 == content_length ? OSS_DOWN_OK : OSS_DOWN_TIMEOUT_RET;
		sprintf(ss,"get object to local file size= %ld",down_end+1);
	} 
	else 
	{
		sprintf(ss,"get object to local file failed:code:%d,error_code:%s,err_msg:%s,req_id:%s",s->code,s->error_code,s->error_msg,s->req_id);
	}
    aos_pool_destroy(p);
	return ret;
}

void init_sample_config(oss_config_t *config,char* OSS_ENDPOINT,char* ACCESS_KEY_ID,char* ACCESS_KEY_SECRET,int is_cname)
{
    aos_str_set(&config->endpoint, OSS_ENDPOINT);
    aos_str_set(&config->access_key_id, ACCESS_KEY_ID);
    aos_str_set(&config->access_key_secret, ACCESS_KEY_SECRET);
    config->is_cname = is_cname;
}

void init_sample_request_options(oss_request_options_t *options,char* OSS_ENDPOINT,char* ACCESS_KEY_ID,char* ACCESS_KEY_SECRET, int is_cname)
{
    options->config = oss_config_create(options->pool);
    init_sample_config(options->config,OSS_ENDPOINT,ACCESS_KEY_ID,ACCESS_KEY_SECRET, is_cname);
    options->ctl = aos_http_controller_create(options->pool, 0);
}

int64_t get_file_size(const char *file_path)
{
    int64_t filesize = -1;
    struct stat statbuff;

    if(stat(file_path, &statbuff) < 0){
        return filesize;
    } else {
        filesize = statbuff.st_size;
    }

    return filesize;
}
int get_object_by_signed_url(const char *signed_url,const char *filepath,long timeout,char * ss)
{
    int ret=-1;
    aos_pool_t *p = NULL;
    aos_string_t url;
    aos_http_request_t *request = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *params = NULL;
    aos_table_t *resp_headers = NULL;
    oss_request_options_t *options = NULL;
    aos_status_t *s = NULL;   
    aos_string_t file;  
    int64_t download_file_size = 0;/*本地文件大小*/
    int64_t downFilePartSize = 0;   /*每次下载文件部分大小*/
    int64_t content_length = 0;    /*服务器端文件大小*/
    content_length = head_object_by_url(signed_url,ss);
    if(!content_length) 
    {
	printf("%s\n",ss);
        return ret;
    }
    download_file_size = get_file_size(filepath);
    printf("download_file_size is %ld\n",download_file_size);
    if(download_file_size < 0) download_file_size = 0;
    if(content_length==download_file_size) 
    {
		return OSS_DOWN_OK;
    }
    else if (content_length < download_file_size)
    {
		char cmd [256]="";
		sprintf(cmd,"rm  -f  \"%s\"",filepath);
		system(cmd);
		download_file_size = 0;
    }
    aos_pool_create(&p, NULL);
    options = oss_request_options_create(p);
    options->config = oss_config_create(options->pool);
    options->ctl = aos_http_controller_create(options->pool, 0);
    // create request
    request = aos_http_request_create(p);
    request->method = HTTP_GET;
    // create headers
    headers = aos_table_make(options->pool, 0);
    aos_str_set(&url, signed_url);
    aos_str_set(&file, filepath); 
    printf("signed get url : %s\n", signed_url);
    char range_bytes[255] = "";
    int64_t down_end = 0;
	/* 根据孙健周阳提出的修改意见，做出修改，每次只下载一部分，就退出检查，160615,何	*/
	/* 每次部分下载大小根据Timeout按1秒1M换算											*/
    downFilePartSize = 1000000L * timeout;
    down_end =downFilePartSize>0 && content_length- download_file_size > downFilePartSize ?
    download_file_size + downFilePartSize - 1 : content_length - 1;
    sprintf(range_bytes, "bytes=%ld-%ld", download_file_size, down_end);
    /* 设置Range，读取文件的指定范围，bytes=20-100包括第20和第100个字符 */
    apr_table_set(headers, "Range", range_bytes);
    s = oss_get_object_to_file_by_url(options,&url, headers,params, &file,&resp_headers);  
    if (aos_status_is_ok(s)) 
    {
		ret = 0;
		sprintf(ss,"get object to local file succeeded\n");
    } 
    else 
    {
		sprintf(ss,"get object to local file failed:code:%d,error_code:%s,err_msg:%s,req_id:%s",ret=s->code,s->error_code,s->error_msg,s->req_id);
    }
    aos_pool_destroy(p);
    return ret;
}
