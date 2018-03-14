#ifndef _FILEMD5_H_
#define _FILEMD5_H_
#include <iostream>
#include <openssl/md5.h>
#include <fstream>

using namespace std;
class CFileMd5
{
public:
    string file_md5(string file_name);
	bool CompareMd5(std::string curPath, std::string strMd5);
};
#endif //_FILEMD5
