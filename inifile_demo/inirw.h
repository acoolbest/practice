#ifndef _INI_RW_H_

#define _INI_RW_H_


#ifdef __cplusplus

extern "C" {

#endif


//加载ini文件至内存

int iniFileLoad(const char *filename);

//释放ini文件所占资源

void iniFileFree();


//获取字符串，不带引号

int iniGetString(const char *section, const char *key, char *value, int size, const char *defvalue);

//获取整数值

int iniGetInt(const char *section, const char *key, int defvalue);

//获取浮点数

double iniGetDouble(const char *section, const char *key, double defvalue);


//设置字符串：若value为NULL，则删除该key所在行，包括注释

int iniSetString(const char *section, const char *key, const char *value);

//设置整数值：base取值10、16、8，分别表示10、16、8进制，缺省为10进制

int iniSetInt(const char *section, const char *key, int value, int base);


#ifdef __cplusplus

}

#endif


#endif