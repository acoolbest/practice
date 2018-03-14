#include "fileMd5.h"
#include <algorithm>

string CFileMd5::file_md5(string file_name)
{
    MD5_CTX md5;
    unsigned char md[16];
    char tmp[33]={'\0'};
    int length,i;
    char buffer[1024];
    string hash="";
    MD5_Init(&md5);
    ifstream fin(file_name.c_str(),ios::in|ios::binary);
    while(!fin.eof())
    {
        fin.read(buffer, 1024);
        length = fin.gcount();
        if (length > 0) {
            MD5_Update(&md5,buffer, length);

        }

    }
    MD5_Final(md,&md5);
    for(i=0; i<16; i++){
        sprintf(tmp, "%02X", md[i]);
        hash+=(string)tmp;

    }
    return hash;

}

bool CFileMd5::CompareMd5(std::string curPath,std::string md5)
{
    string strMd5 = file_md5(curPath);
    transform(strMd5.begin(), strMd5.end(), strMd5.begin(), ::tolower);
    std::string strTargetMd5(md5);
    transform(strTargetMd5.begin(), strTargetMd5.end(), strTargetMd5.begin(), ::tolower);
    return strMd5 == strTargetMd5;
}
