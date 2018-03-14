#include "HttpRequestInterface.h"
#include <map>
#include "HttpRequest.h"
#include "MyResultClass.h"
#include "PublicFunction.h"
#include <unistd.h>

using namespace std;



int HttpRequestWithAdressType(std::string strReqAdress, std::string& strJosn, std::string contentType)
{
    MyResultClass mc;
    long http_code = 404;
    HttpRequest request;
    request.SetRequestUrl(strReqAdress);
    request.SetResultCallback(std::bind(&MyResultClass::MyRequestResultCallback, &mc, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    request.SetRequestHeader(contentType);
    request.SetPostData(strJosn);
    request.SetRequestTimeout(2);

    //CPublicFunction::WriteLog("[DEBUG] send json  is: %s\n",strJosn.c_str());
    HANDLE hRequest = request.PerformRequest(HttpRequest::REQUEST_ASYNC);
    if (hRequest)
    {
        while (mc.IsRequestFinish() == false) sleep(1);
		request.GetHttpCode(hRequest, &http_code);
		std::string header;
		request.GetReceiveHeader(hRequest, &header);
		#if 0
        if (request.GetHttpCode(hRequest, &http_code))
            CPublicFunction::WriteLog("[DEBUG]http code:%ld\n",http_code);
        std::string header;
        if (request.GetReceiveHeader(hRequest, &header))
        {
            CPublicFunction::WriteLog("[DEBUG]http header is: %s\n",header.c_str());
        }
		#endif
       /* 
           if (request.GetReceiveContent(hRequest, &retdata))
           {
           log_debug("[DEBUG]http getdata is: %s\n",retdata.c_str());
           }
           */
        HttpRequest::Close(hRequest);
    }
    return http_code;
}
int HttpPost(std::string strReqAdress, std::string& strJosn, std::string contentType, string& retdata)
{
    MyResultClass mc;
    long http_code = 404;
    HttpRequest request;
    request.SetRequestUrl(strReqAdress);
    request.SetResultCallback(std::bind(&MyResultClass::MyRequestResultCallback, &mc, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    request.SetRequestHeader(contentType);
    request.SetPostData(strJosn);
    request.SetRequestTimeout(2);

    //CPublicFunction::WriteLog("[DEBUG] send json  is: %s\n",strJosn.c_str());
	#if 1
	HANDLE hRequest = request.PerformRequest(HttpRequest::REQUEST_ASYNC);
    if (hRequest)
    {
        while (mc.IsRequestFinish() == false) sleep(1);
		
		request.GetHttpCode(hRequest, &http_code);
		std::string header;
		request.GetReceiveHeader(hRequest, &header);
		request.GetReceiveContent(hRequest, &retdata);
		#if 0
        if (request.GetHttpCode(hRequest, &http_code))
            CPublicFunction::WriteLog("[DEBUG]http code:%ld\n",http_code);
        std::string header;
        if (request.GetReceiveHeader(hRequest, &header))
        {
            CPublicFunction::WriteLog("[DEBUG]http header is: %s\n",header.c_str());
        }
        
		if (request.GetReceiveContent(hRequest, &retdata))
		{
			CPublicFunction::WriteLog("[DEBUG]http getdata is: %s\n",retdata.c_str());
		}
		#endif
        HttpRequest::Close(hRequest);

    }
	#endif
    return http_code;
}
