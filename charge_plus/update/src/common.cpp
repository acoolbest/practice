#include "common.h"


int find_string_from_vector(string find_str, vector<string>* vec_str)
{
    if (vec_str == NULL)
        return CRG_INVALID_PARAM;

    int dwret = CRG_NOT_FOUND;
    for (unsigned int i = 0; i < vec_str->size(); i++)
    {
        if ((*vec_str)[i].length() > strlen("package:"))
        {
            string src = (*vec_str)[i].substr(strlen("package:"));
            if (strcasecmp(find_str.c_str(), src.c_str()) == 0)
            {
                dwret = CRG_FOUND;
                break;
            }
        }

    }
    return dwret;

}

void split(const string& s, const string& delim, vector<string>* ret)
{
    string str;
    size_t last = 0;
    size_t index = s.find(delim, last);
    while (index != std::string::npos)
    {
        str = s.substr(last, index - last);
        if (!str.empty())
        {
            ret->push_back(str);

        }
        last = index + delim.length();
        index = s.find(delim, last);
    }
    if (index - last > 0)
    {
        ret->push_back(s.substr(last, index - last));
    }
}


string GetCurrTime(void)
{

    time_t timep;
    struct tm *p;
    char s[64];//
    time(&timep);
    p=localtime(&timep);
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", p);
    // printf("%s\n",  s);
    return string(s);
}


int InsertStringMap(map<string, string>* out, string &key, string &val)                                                                                          
{
    if (out == NULL)
        return CRG_INVALID_PARAM;

    pair<map<string, string>::iterator, bool> insert_pair;

    insert_pair = out->insert(pair<string, string>(key, val));

    return insert_pair.second ? CRG_SUCCESS : CRG_FAIL;


}

int IsStringExist(map<string, string> src_map, string find_key, string& out_val)
{
    int ret = CRG_NOT_FOUND;
    map<string, string>::iterator iter;
    iter = src_map.find(find_key);
    if (iter != src_map.end())
    {
        out_val = iter->second;
        ret = CRG_FOUND;
    }
    return ret;
}

int IsExistFile(char* filename)
{
    if (filename == NULL)
        return CRG_INVALID_PARAM;
    return access(filename, 4) == -1 ? CRG_NOT_FOUND : CRG_FOUND;

}    


bool CheckSum(char* pFrame, int8_t len, int8_t& sum)
{
    sum = 0;
    if (len < 4)
    {
        return false;

    }
    for (int i = 2; i < len - 2; i++)
    {
        sum += pFrame[i];

    }
    return sum == pFrame[len - 2];

}

void read_file(char *path, string &json_)
{ 
    //char *json_;
    size_t length_;
    char whitespace_[1024*1024]="";
    //size_t whitespace_length_;
    
    FILE *fp = fopen(path, "rb");
    if (!fp)
        fp = fopen(path, "rb");

    fseek(fp, 0, SEEK_END);
    length_ = (size_t)ftell(fp);
	
    fseek(fp, 0, SEEK_SET);
    //json_ = (char*)malloc(length_ + 1);
    json_.resize(length_ + 1);
    fread(whitespace_, 1, length_, fp);
    json_=whitespace_;
    //json_+='\0';//[length_] = '\0';
    //length_++; // include the null terminator
    fclose(fp);
	#if 0
    whitespace_length_ = 1024 * 1024;
    whitespace_ = (char *)malloc(whitespace_length_ + 4);
    char *p = whitespace_;
    for (size_t i = 0; i < whitespace_length_; i += 4) {
        *p++ = ' ';
        *p++ = '\n';
        *p++ = '\r';
        *p++ = '\t';

    }
    *p++ = '[';
    *p++ = '0';
    *p++ = ']';
    *p++ = '\0';

    free(whitespace_);// free(json_);
    #endif
    //return json_;
}

void write_file(char *path, string js) 
{
    FILE *fp = fopen(path, "w");
    if(fp){
        fputs(js.c_str(),fp);
        fclose(fp);
    }

}

void ClearUsbDeviceInfo(UsbDeviceInfo* pinfo)
{
    if (pinfo != NULL)
    {
        pinfo->dwAddressNum = 0;
        pinfo->dwBoardType = DEFAULT_BOARD_TYPE;
        pinfo->dwBusNum = 0;
        pinfo->dwPortNum = 0;
        memset(pinfo->szBoardNum, 0,sizeof(pinfo->szBoardNum));
        memset(pinfo->szPid,0, sizeof(pinfo->szPid));
        memset(pinfo->szVid,0,sizeof(pinfo->szVid));
        //pinfo->vecObject.clear();
        //pinfo->vecSerialNo.clear();

    }

}

void ClearInstallAppResultLogInfo(InstallAppResultLogInfo* pinfo)
{
    if (pinfo != NULL)
    {
        pinfo->dwResult = 0;
        pinfo->vecInstalledAppTime.clear();
        memset(&pinfo->deviceInfo,0, sizeof(pinfo->deviceInfo));

    }

}


bool  Json2Vector(std::string strContent, std::string& version, std::string& type, VersionVector& vec)
{
    int ret = CRG_FAIL;
    vec.clear();
    if (strContent.empty())
    {
        return false;
    }
    bool valid = libjson::is_valid(strContent);
    if(valid){
        JSONNode n = libjson::parse(strContent);
        JSONNode::const_iterator i = n.begin();
        string str;
        while(i != n.end()){
            std::string node_name = i -> name();
            if(node_name=="version"){
                version =  i->as_string();
            }
            if(node_name=="type"){
                type =  i->as_string();
            }
            if(node_name=="fileDatas"){
                JSONNode::const_iterator k = (*i).begin();
                while(k != (*i).end()){
                    FileVersion ver;
                    JSONNode::const_iterator j = (*k).begin(); 
                    while(j!=(*k).end())
                    {
                        std::string jnode_name = j -> name();
                        if(jnode_name=="appName"){
                            ver.appName =  j->as_string();
                        }
                        if(jnode_name=="appPkgName"){
                            ver.appPkgName =  j->as_string();
                        }
                        if(jnode_name=="fileName"){
                            ver.fileName =  j->as_string();
                        }
                        if(jnode_name=="md5"){
                            ver.md5 =  j->as_string();
                        }
                        if(jnode_name=="downUrl"){
                            ver.downUrl =  j->as_string();
                        }
                        if(jnode_name=="version"){
                            ver.version =  j->as_string();
                        }
                        if(jnode_name=="appId"){
                            ver.appId =  j->as_string();
                        }
                        if(jnode_name=="appType"){
                            ver.appType =  j->as_string();
                        }
						if(jnode_name=="isAutoOpen"){
                            ver.autoOpen =  j->as_string();
                        }
                        ++j;

                    }
                        vec.push_back(ver);
                    k++;
                } 
                ret= CRG_SUCCESS;
            }
            i++;
        }
    }
        return ret;
}
void  Vector2WriteJson(VersionVector vec,string strSaveFolder, string& writeJson)
{
        JSONNode rn(JSON_NODE);
        JSONNode ios(JSON_ARRAY); 
        ios.set_name("IOS");
        JSONNode android(JSON_ARRAY); 
        android.set_name("ANDROID");
        
    for (VersionVector::iterator itr = vec.begin(); itr != vec.end(); itr++) {
               JSONNode cn(JSON_NODE);
               cn.push_back(JSONNode("AppId",itr->appId));
               cn.push_back(JSONNode("AppName",itr->appName));
               cn.push_back(JSONNode("AppBundleId",itr->appPkgName));
               int pos = itr->downUrl.find_last_of('/');
            std::string fileName = RemoveHeader(itr->downUrl.substr(pos + 1)) ; 
                            //fileName = removeStr(fileName);
               cn.push_back(JSONNode("AppPath",strSaveFolder + "/" + fileName));
               cn.push_back(JSONNode("AppVersion",itr->version));
			   cn.push_back(JSONNode("autoOpen",itr->autoOpen));
        if(itr->appType==std::string("0")){ //android
            android.push_back(cn);
        }
        if(itr->appType==std::string("1")){ //ios
            ios.push_back(cn);
        }
    }
    rn.push_back(android);
    rn.push_back(ios);
    writeJson =rn.write_formatted();
}

string removeStr(string instr){
				std::string outstr = "";
				for(unsigned int k=0;k<instr.size();k++)
				{	
					if(instr[k] == ' ')
					{		
						outstr += '\\';
						outstr += ' ';
					}	
                    else if(instr[k] == '('){
						outstr += '\\';
						outstr += '(';
                    }
                    else if(instr[k] == ')'){
						outstr += '\\';
						outstr += ')';
                    }
					else	
					{		
						outstr += instr[k];	
					}	
				}
return outstr;      
}

char *strlwr(char *str)
{
    char *orign=str;

	for(unsigned int i = 0; i < strlen(str); i++)
	{
		str[i] = tolower(str[i]);
	}
    return orign;
}

