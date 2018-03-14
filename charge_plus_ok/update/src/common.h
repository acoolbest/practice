#ifndef _COMMON_H_
#define _COMMON_H_
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include "def.h"
#include "PublicFunction.h"
#include "libjson/libjson.h"

void split(const string& s, const string& delim, vector<string>* ret);

int find_string_from_vector(string find_str, vector<string>* vec_str);


string GetCurrTime(void);
int InsertStringMap(map<string, string>* out, string &key, string &val);
int IsStringExist(map<string, string> src_map, string find_key, string& out_val);
void read_file(char *path, string &json_);
void write_file(char *path, string js);
int IsExistFile(char* filename);
bool CheckSum(char* pFrame, int8_t len, int8_t& sum);
void ClearUsbDeviceInfo(UsbDeviceInfo* pinfo);
void ClearInstallAppResultLogInfo(InstallAppResultLogInfo* pinfo);
bool  Json2Vector(std::string strContent, std::string& version, std::string& type, VersionVector& vec);
void  Vector2WriteJson(VersionVector vec,string strSaveFolder, string& writeJson);
string removeStr(string instr);
char *strlwr(char *str);
#endif // _COMMON_H_
