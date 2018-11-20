/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：HttpClient.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年08月14日
 *   描    述：
 *
 #include "HttpCurl.h"
 ================================================================*/

#include <string>
#include "HttpClient.h"
#include "json/json.h"
#include "util.h"
#include "md5.h"
#include "Base64.h"

using namespace std;

#define IM_SERVER_VERSION 1 //1:test;0:business

size_t write_data_string(void *ptr, size_t size, size_t nmemb, void *userp)
{
    size_t len = size * nmemb;
    string* response = (string *)userp;
    
    response->append((char*)ptr, len);
    
    return len;
}

size_t write_data_binary(void *ptr, size_t size, size_t nmemb, AudioMsgInfo* pAudio)
{
    size_t nLen = size * nmemb;
    if(pAudio->data_len + nLen <= pAudio->fileSize + 4)
    {
        memcpy(pAudio->data + pAudio->data_len, ptr, nLen);
        pAudio->data_len += nLen;
    }
    return nLen;
}


CHttpClient::CHttpClient(void)
{
    
}

CHttpClient::~CHttpClient(void)
{
    
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    string* str = dynamic_cast<string*>((string *)lpVoid);
    if( NULL == str || NULL == buffer )
    {
        return -1;
    }
    
    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
    return nmemb;
}

CURLcode CHttpClient::Post(const string & strUrl, const string & strPost, string & strResponse)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
		log("failed to initialize url............");
        return CURLE_FAILED_INIT;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	log("http post communicating..................");
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}

CURLcode CHttpClient::Get(const string & strUrl, string & strResponse)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        return CURLE_FAILED_INIT;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
    /**
     * 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
     * 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
     */
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}

string CHttpClient::UploadByteFile(const string &strUrl, void* pData, int nSize)
{
    if(strUrl.empty())
        return "";
    
    CURL* curl = curl_easy_init();
    if (!curl)
        return "";
    struct curl_slist *headerlist = NULL;
    headerlist = curl_slist_append(headerlist, "Content-Type: multipart/form-data; boundary=WebKitFormBoundary8riBH6S4ZsoT69so");
    // what URL that receives this POST
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
//    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);    //enable verbose for easier tracing
    string body = "--WebKitFormBoundary8riBH6S4ZsoT69so\r\nContent-Disposition: form-data; name=\"file\"; filename=\"1.audio\"\r\nContent-Type:image/jpg\r\n\r\n";
    body.append((char*)pData, nSize);    // image buffer
    string str = "\r\n--WebKitFormBoundary8riBH6S4ZsoT69so--\r\n\r\n";
    body.append(str.c_str(), str.size());
    // post binary data
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    // set the size of the postfields data
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
    string strResp;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strResp);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    
    // Perform the request, res will get the return code
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if (CURLE_OK != res) {
        log("curl_easy_perform failed, res=%d", res);
        return "";
    }
    
    // upload 返回的json格式不一样，要特殊处理
    Json::Reader reader;
    Json::Value value;
    
    if (!reader.parse(strResp, value)) {
        log("json parse failed: %s", strResp.c_str());
        return "";
    }
    
    if (value["error_code"].isNull()) {
        log("no code in response %s", strResp.c_str());
        return "";
    }
    uint32_t nRet = value["error_code"].asUInt();
    if(nRet != 0)
    {
        log("upload faile:%u", nRet);
        return "";
    }
    return value["url"].asString();
}

bool CHttpClient::DownloadByteFile(const string &url, AudioMsgInfo* pAudioMsg)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,2);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_binary);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, pAudioMsg);
    CURLcode res = curl_easy_perform(curl);
    
    int retcode = 0;
    res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
    if(CURLE_OK != res || retcode != 200) {
        log("curl_easy_perform failed, res=%d, ret=%u", res, retcode);
    }
    double nLen = 0;
    res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &nLen);
    curl_easy_cleanup(curl);
    if (nLen != pAudioMsg->fileSize) {
        return false;
    }
    return true;
}


CURLcode CHttpClient::Post(const string & strUrl, const string & strPost, string & strResponse, string& strHeader)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		log("failed to initialize url............");
		return CURLE_FAILED_INIT;
	}

	struct curl_slist *headerlist = NULL;
	headerlist = curl_slist_append(headerlist, strHeader.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	char szErrorMsg[512];
	memset(szErrorMsg, 0, sizeof(szErrorMsg));
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, szErrorMsg);
	log("http post communicating..................");
	res = curl_easy_perform(curl);
	if (res != 0)
	{
		log("Error No: %d, Error Msg: %s", res, szErrorMsg);
	}
	curl_easy_cleanup(curl);
	return res;
}


void CHttpClient::GetAdditionalInfo(string& strToken, string& strHeader)
{
	//privillege auth
	time_t nCurrTime = time(NULL);
	long long llCurrTime = nCurrTime * 1000;
	char szCurrentTime[16];
	memset(szCurrentTime, 0, sizeof(szCurrentTime));
	sprintf(szCurrentTime, "%lld", llCurrTime);

#if IM_SERVER_VERSION == 1
	string strLoginInfo = "userrest:d~!#%^*jhfs|456?dsjf";
#else
	string strLoginInfo = "userrest:7*c6C!WQrKlAy&$QneXvM&n94OobsB3Z";
#endif

	string strEncodeResult = base64_encode(strLoginInfo);
	string strUserPassword = "Authorization:Basic " + strEncodeResult;
	strHeader.append("Content-type:application/json\r\n");
	strHeader += (strUserPassword + "\r\nRequest-time:" + szCurrentTime);

	string strOrigToken = "";
	strOrigToken += szCurrentTime;
	strOrigToken += "{Ab$Cd8;i.gsdf_12p*^34_%}";

	char szToken[36];
	memset(szToken, 0, sizeof(szToken));
	MD5_Calculate(strOrigToken.c_str(), strOrigToken.length(), szToken);
	strToken = szToken;
}