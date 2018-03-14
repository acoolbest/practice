#include "FileDown.h"
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <string>
#include "def.h"
#include "PublicFunction.h"
#include "oss_get_object_to_file.h"
using namespace std;

int ftp_download(const char * remotepath,const char* username, const char* passwd, const char * localpath, long timeout) 
{
	int ret = CRG_FAIL;
	struct stat file_info;  
	int use_resume = 0;  
	curl_off_t local_file_len = -1 ; 
	//获取本地文件大小信息  
	if(stat(localpath, &file_info) == 0)  
	{  
		local_file_len = file_info.st_size;   
		use_resume = 1;  
	}

	FILE *f = fopen(localpath, "ab+");
	if (f == NULL) { 
		perror(NULL); 
		return ret; 
	}
	char login_str[MAX_BUFF_LEN];
	snprintf(login_str, 255, "%s:%s", username,passwd);
	//CPublicFunction::WriteLog("DEBUG: Download server type is %s",serverType);
	curl_global_init(CURL_GLOBAL_DEFAULT); 
	CURL* curlhandle = curl_easy_init(); 
	curl_easy_setopt(curlhandle, CURLOPT_URL, remotepath); 

	curl_easy_setopt(curlhandle, CURLOPT_USERPWD, login_str);

	//curl_easy_setopt(curlhandle, CURLOPT_CONNECTTIMEOUT, timeout); 
	curl_easy_setopt(curlhandle, CURLOPT_TIMEOUT, timeout); 

	/* Switch on full protocol/debug output */
	curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);

	// 设置断点续传  
	curl_easy_setopt(curlhandle, CURLOPT_RESUME_FROM_LARGE, use_resume?local_file_len:0); 
	curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, f); 
	CURLcode    r = curl_easy_perform(curlhandle);
	curl_easy_cleanup(curlhandle);
	curl_global_cleanup();
	fclose(f); 
		char chErrMsg[255] = "";
		switch (r)
		{
		case CURLE_OK:
			sprintf(chErrMsg, "文件%s下载成功，返回码%d",localpath,r); break;
		case  CURLE_UNSUPPORTED_PROTOCOL:
			sprintf(chErrMsg, "下载未开始，返回码%d,协议不支持", r);
			break;
		case CURLE_COULDNT_CONNECT:
			sprintf(chErrMsg, "下载未开始，返回码%d,无法联接服务器", r); break;
		case CURLE_OPERATION_TIMEDOUT:
			sprintf(chErrMsg, "下载中断，返回码%d,下载超时", r); break;
		case CURLE_RECV_ERROR:
			sprintf(chErrMsg, "下载中断，返回码%d,因网络被禁用终止下载", r); break;
		case CURLE_LOGIN_DENIED:
			sprintf(chErrMsg, "下载未开始，返回码%d,用户名或密码错", r); break;
		case CURLE_REMOTE_FILE_NOT_FOUND:
			sprintf(chErrMsg, "下载未开始，返回码%d,服务器文件不存在", r); break;
		default:
			sprintf(chErrMsg, "下载不成功，返回码%d", r);
		}
		CPublicFunction::WriteLog("%s",chErrMsg);
        ret=r;
	return ret;
}

int download(const char * remotepath,const char* username, const char* passwd, const char * localpath, long timeout) 
{
	char serverType[5]=""; 
        char ss[512]="";
	strncpy(serverType,remotepath,4);
	if(strcasecmp(serverType,"FTP:") != 0)
        {
		int ret= oss_download(remotepath,username,passwd,localpath,localpath,timeout,ss);
		CPublicFunction::WriteLog("[%s]下载结果，%s",localpath,ss);
		return ret;	
        }
	else
		return ftp_download(remotepath,username,passwd,localpath,timeout);
} 
