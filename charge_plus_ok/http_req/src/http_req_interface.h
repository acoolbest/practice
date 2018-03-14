#ifndef _HTTP_REQ_INTERFACE_H_
#define _HTTP_REQ_INTERFACE_H_
#include <string>

int http_request_with_address_type(std::string str_req_address, std::string& str_josn, std::string content_type, std::string & out_err);

#endif

