#include <unistd.h>
#include <map>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "http_req.h"
#include "http_req_interface.h"

int http_request_with_address_type(std::string str_req_address, std::string& str_josn, std::string content_type, std::string & out_err)
{
	long http_code = 404;
	HttpRequest request;
	request.SetRequestUrl(str_req_address);
	request.SetRequestHeader(content_type);
	request.SetPostData(str_josn);
	request.SetRequestTimeout(1);
	HANDLE hRequest = request.PerformRequest(HttpRequest::REQUEST_SYNC);
	if (hRequest)
	{
		request.GetHttpCode(hRequest, &http_code);
		request.GetErrorString(hRequest, &out_err);
	}
	if(http_code == 200) return http_code;

	if(!http_code)
		write_log("Server connection failure!\n");
	else
		write_log("Server return error code %d!\n", http_code);
	return http_code;
}
