#include "util.h"
#include <sstream>
using namespace std;

CSLog g_imlog = CSLog(LOG_MODULE_IM);

CRefObject::CRefObject()
{
	m_lock = NULL;
	m_refCount = 1;
}

CRefObject::~CRefObject()
{

}

void CRefObject::AddRef()
{
	if (m_lock)
	{
		m_lock->lock();
		m_refCount++;
		m_lock->unlock();
	}
	else
	{
		m_refCount++;
	}
}

void CRefObject::ReleaseRef()
{
	if (m_lock)
	{
		m_lock->lock();
		m_refCount--;
		if (m_refCount == 0)
		{
			delete this;
			return;
		}
		m_lock->unlock();
	}
	else
	{
		m_refCount--;
		if (m_refCount == 0)
			delete this;
	}
}

uint64_t get_tick_count()
{
#ifdef _WIN32
	LARGE_INTEGER liCounter; 
	LARGE_INTEGER liCurrent;

	if (!QueryPerformanceFrequency(&liCounter))
		return GetTickCount();

	QueryPerformanceCounter(&liCurrent);
	return (uint64_t)(liCurrent.QuadPart * 1000 / liCounter.QuadPart);
#else
	struct timeval tval;
	uint64_t ret_tick;

	gettimeofday(&tval, NULL);

	ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
	return ret_tick;
#endif
}

void util_sleep(uint32_t millisecond)
{
#ifdef _WIN32
	Sleep(millisecond);
#else
	usleep(millisecond * 1000);
#endif
}

CStrExplode::CStrExplode(char* str, char seperator)
{
	m_item_cnt = 1;
	char* pos = str;
	while (*pos) {
		if (*pos == seperator) {
			m_item_cnt++;
		}

		pos++;
	}

	m_item_list = new char* [m_item_cnt];

	int idx = 0;
	char* start = pos = str;
	while (*pos) {
		if ( pos != start && *pos == seperator) {
			uint32_t len = pos - start;
			m_item_list[idx] = new char [len + 1];
			strncpy(m_item_list[idx], start, len);
			m_item_list[idx][len]  = '\0';
			idx++;

			start = pos + 1;
		}

		pos++;
	}

	uint32_t len = pos - start;
    if(len != 0)
    {
        m_item_list[idx] = new char [len + 1];
        strncpy(m_item_list[idx], start, len);
        m_item_list[idx][len]  = '\0';
    }
}

CStrExplode::~CStrExplode()
{
	for (uint32_t i = 0; i < m_item_cnt; i++) {
		delete [] m_item_list[i];
	}

	delete [] m_item_list;
}

char* replaceStr(char* pSrc, char oldChar, char newChar)
{
    if(NULL == pSrc)
    {
        return NULL;
    }
    
    char *pHead = pSrc;
    while (*pHead != '\0') {
        if(*pHead == oldChar)
        {
            *pHead = newChar;
        }
        ++pHead;
    }
    return pSrc;
}

string int2string(uint32_t user_id)
{
    stringstream ss;
    ss << user_id;
    return ss.str();
}

string int2string(int32_t user_id)
{
	stringstream ss;
	ss << user_id;
	return ss.str();
}
uint32_t string2int(const string& value)
{
    return (uint32_t)atoi(value.c_str());
}

string longlong2string(long long user_id)
{
	stringstream ss;
	ss << user_id;
	return ss.str();
}

long long string2longlong(const string& value)
{
	return atoll(value.c_str());
}


// 由于被替换的内容可能包含?号，所以需要更新开始搜寻的位置信息来避免替换刚刚插入的?号
void replace_mark(string& str, string& new_value, uint32_t& begin_pos)
{
    string::size_type pos = str.find('?', begin_pos);
    if (pos == string::npos) {
        return;
    }
    
    string prime_new_value = "'"+ new_value + "'";
    str.replace(pos, 1, prime_new_value);
    
    begin_pos = pos + prime_new_value.size();
}

void replace_mark(string& str, uint32_t new_value, uint32_t& begin_pos)
{
    stringstream ss;
    ss << new_value;
    
    string str_value = ss.str();
    string::size_type pos = str.find('?', begin_pos);
    if (pos == string::npos) {
        return;
    }
    
    str.replace(pos, 1, str_value);
    begin_pos = pos + str_value.size();
}


void writePid()
{
	uint32_t curPid;
#ifdef _WIN32
	curPid = (uint32_t) GetCurrentProcess();
#else
	curPid = (uint32_t) getpid();
#endif
    FILE* f = fopen("server.pid", "w");
    assert(f);
    char szPid[32];
    snprintf(szPid, sizeof(szPid), "%d", curPid);
    fwrite(szPid, strlen(szPid), 1, f);
    fclose(f);
}

inline unsigned char toHex(const unsigned char &x)
{
    return x > 9 ? x -10 + 'A': x + '0';
}

inline unsigned char fromHex(const unsigned char &x)
{
    return isdigit(x) ? x-'0' : x-'A'+10;
}

string URLEncode(const string &sIn)
{
    string sOut;
    for( size_t ix = 0; ix < sIn.size(); ix++ )
    {
        unsigned char buf[4];
        memset( buf, 0, 4 );
        if( isalnum( (unsigned char)sIn[ix] ) )
        {
            buf[0] = sIn[ix];
        }
        //else if ( isspace( (unsigned char)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以
        //{
        //    buf[0] = '+';
        //}
        else
        {
            buf[0] = '%';
            buf[1] = toHex( (unsigned char)sIn[ix] >> 4 );
            buf[2] = toHex( (unsigned char)sIn[ix] % 16);
        }
        sOut += (char *)buf;
    }
    return sOut;
}

string URLDecode(const string &sIn)
{
    string sOut;
    for( size_t ix = 0; ix < sIn.size(); ix++ )
    {
        unsigned char ch = 0;
        if(sIn[ix]=='%')
        {
            ch = (fromHex(sIn[ix+1])<<4);
            ch |= fromHex(sIn[ix+2]);
            ix += 2;
        }
        else if(sIn[ix] == '+')
        {
            ch = ' ';
        }
        else
        {
            ch = sIn[ix];
        }
        sOut += (char)ch;
    }
    return sOut;
}


int64_t get_file_size(const char *path)
{
    int64_t filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
        return filesize;
    }else{
        filesize = statbuff.st_size;
    }
    return filesize;
}

const char*  memfind(const char *src_str,size_t src_len, const char *sub_str, size_t sub_len, bool flag)
{
    if(NULL == src_str || NULL == sub_str || src_len <= 0)
    {
        return NULL;
    }
    if(src_len < sub_len)
    {
        return NULL;
    }
    const char *p;
    if (sub_len == 0)
        sub_len = strlen(sub_str);
    if(src_len == sub_len)
    {
        if(0 == (memcmp(src_str, sub_str, src_len)))
        {
            return src_str;
        }
        else
        {
            return NULL;
        }
    }
    if(flag)
    {
        for (int i = 0; i < src_len - sub_len; i++)
        {
            p = src_str + i;
            if(0 == memcmp(p, sub_str, sub_len))
                return p;
        }
    }
    else
    {
        for ( int i = (src_len - sub_len) ; i >= 0;i--  )
        {
            p = src_str + i;
            if ( 0 == memcmp(  p,sub_str,sub_len ) )
                return p;
            
        }
    }
    return NULL;
}


//标志位(按从低位到高位顺序排列最后1位代表是否管理员(0：是，1:否);第2位代表管理员状态(0:禁用，1:启用);第3位代表是否超级管理员(0；否，1:是);第4和5位代表用户状态，第6代表激活状态；第7位代表通讯录状态
uint32_t GetUserFlags(int32_t nAdmin, int32_t nAdminStatus, int32_t nSuperAdmin, int32_t nStaffStatus, int32_t nActiveStatus, int32_t nContactStatus)
{
	uint32_t unFlag = 0; 
	if (nAdmin)
	{
		unFlag |= 0x01;
	}

	if (nAdminStatus)
	{
		unFlag |= 0x02;
	}

	if (nSuperAdmin)
	{
		unFlag |= 0x04;
	}

	if (nStaffStatus == 1)
	{
		unFlag |= 0x08;
	}
	else if (nStaffStatus == -1)
	{
		unFlag |= 0x10;
	}

	if (nActiveStatus)
	{
		unFlag |= 0x20;
	}

	if (nContactStatus)
	{
		unFlag |= 0x40;
	}

	return unFlag;
}


//标志位(按从低位到高位顺序排列最后2位代表性别；第3和4位代表用户状态，第5位代表是否管理员(0：是，1:否);第6位代表管理员状态(0:禁用，1:启用);第7位代表是否超级管理员(0；否，1:是);8和9位代表员工状态，第10位代表激活状态；第11位代表通讯录状态位)
uint32_t GetUserFlags(int32_t nGender, int32_t nUserStatus, int32_t nAdmin, int32_t nAdminStatus, int32_t nSuperAdmin, int32_t nStaffStatus, int32_t nActiveStatus, int32_t nContactStatus)
{
	uint32_t unFlag = 0; //lower 2 bits for gender; lower 3 means contact status;lower 4 and 5 means user status
	if (nGender == 1) //male
	{
		unFlag |= 0x01;
	}
	else if (nGender == 2) //unknown
	{
		unFlag |= 0x02;
	}

	if (nUserStatus == 1)  //normal
	{
		unFlag |= 0x04;
	}
	else if (nUserStatus == -1) //deleted
	{
		unFlag |= 0x08;
	}

	if (nAdmin)
	{
		unFlag |= 0x10;
	}

	if (nAdminStatus)
	{
		unFlag |= 0x20;
	}

	if (nSuperAdmin)
	{
		unFlag |= 0x40;
	}

	if (nStaffStatus == 1)
	{
		unFlag |= 0x0080;
	}
	else if (nStaffStatus == -1)
	{
		unFlag |= 0x0100;
	}

	if (nActiveStatus)
	{
		unFlag |= 0x0200;
	}

	if (nContactStatus)
	{
		unFlag |= 0x0400;
	}

	return unFlag;
}



uint32_t GetProduct(string& strProduct)
{
	//用户订购产品(16位中每一位代表一种产品。1表示开通该产品；0则表示未开通。就目前开发的产品来看从低位到高位的每一位分别代表IM、审批、邮箱、公告、云盘、通讯录、文档和人力资源)
	uint32_t unProduct = 0;
	if (strProduct.find("IM") != string::npos)
	{
		unProduct = 0x01;
	}

	if (strProduct.find("WORKFLOW") != string::npos)
	{
		unProduct |= 0x02;
	}

	if (strProduct.find("MAIL") != string::npos)
	{
		unProduct |= 0x04;
	}

	if (strProduct.find("BULLETIN") != string::npos)
	{
		unProduct |= 0x08;
	}

	if (strProduct.find("CLOUDDISK") != string::npos)
	{
		unProduct |= 0x10;
	}

	if (strProduct.find("CONTACTS") != string::npos)
	{
		unProduct |= 0x20;
	}

	if (strProduct.find("DOC") != string::npos)
	{
		unProduct |= 0x40;
	}

	if (strProduct.find("HR") != string::npos)
	{
		unProduct |= 0x80;
	}

	return unProduct;
}


void generate_id(string& strUUID)
{
	uuid_t uid = { 0 };
	uuid_generate(uid);
	if (!uuid_is_null(uid))
	{
		char szUUID[64];
		memset(szUUID, 0, sizeof(szUUID));
		uuid_unparse(uid, szUUID);

		char szUUID1[64];
		memset(szUUID1, 0, sizeof(szUUID1));

		for (int i = 0, j = 0; szUUID[i] != 0; i++)
		{
			if (szUUID[i] != '-')
			{
				szUUID1[j++] = szUUID[i];
			}
		}
		strUUID = szUUID1;
	}
}