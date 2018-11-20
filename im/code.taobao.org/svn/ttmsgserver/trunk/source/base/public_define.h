//
//  public_define.h
//  im-server-mac-new
//
//  Created by luoning on 14-12-24.
//  Copyright (c) 2014年 luoning. All rights reserved.
//

#ifndef _public_define_h
#define _public_define_h

#include <iostream>
#include <set>
#include "IM.BaseDefine.pb.h"
#include "ostype.h"
using namespace std;

#define XIAO_T_UID  99999999

enum {
    USER_CNT_INC = 1,
    USER_CNT_DEC = 2,
};

enum {
    IM_GROUP_SETTING_PUSH = 1,
	IM_USER_SETTING_PUSH = 2,
};

enum {
    IM_PUSH_TYPE_NORMAL = 1,
    IM_PUSH_TYPE_SILENT = 2,
};

enum {
    IM_PC_LOGIN_STATUS_ON = 1,
    IM_PC_LOGIN_STATUS_OFF = 0,
};

// client type:
#define CLIENT_TYPE_FLAG_NONE    0x00
#define CLIENT_TYPE_FLAG_PC      0x01
#define CLIENT_TYPE_FLAG_MOBILE  0x02
#define CLIENT_TYPE_FLAG_BOTH    0x03

//product id:
#define HUAYUN_IM_PRODUCT_WORKFLOW	1
#define HUAYUN_IM_PRODUCT_EMAIL	2
#define HUAYUN_IM_PRODUCT_BULLETIN 3
#define HUAYUN_IM_ASST_NEW_FRIEND	4
#define HUAYUN_IM_ASST_NEW_STAFF	5
#define HUAYUN_IM_ASST_ATTENDANCE	6
/*
enum {
    CLIENT_TYPE_WINDOWS     = 0x01,
    CLIENT_TYPE_MAC         = 0x02,
    CLIENT_TYPE_IOS         = 0x11,
    CLIENT_TYPE_ANDROID     = 0x12,
};
*/

#define CHECK_CLIENT_TYPE_PC(type) \
({\
bool bRet = false;\
if ((type & 0x10) == 0x00)\
{\
bRet = true;\
}\
bRet;\
})

#define CHECK_CLIENT_TYPE_MOBILE(type) \
({\
bool bRet = false;\
if ((type & 0x10) == 0x10)\
{\
bRet = true;\
}\
bRet;\
})

enum {
    GENDER_UNKNOWN  = 0,
    GENDER_MAN      = 1,
    GENDER_WOMAN    = 2,
};

/*
enum {
    SESSION_TYPE_SINGLE     = 0x01,
    SESSION_TYPE_GROUP      = 0x02,
};
*/

/*
enum {
    MSG_TYPE_SINGLE_TEXT    = 0x01,
    MSG_TYPE_SINGLE_AUDIO   = 0x02,
    MSG_TYPE_GROUP_TEXT     = 0x11,
    MSG_TYPE_GROUP_AUDIO    = 0x12,
};
*/

#define CHECK_MSG_TYPE_SINGLE(type) \
({\
bool bRet = false;\
if ((IM::BaseDefine::MSG_TYPE_SINGLE_TEXT == type) || (IM::BaseDefine::MSG_TYPE_SINGLE_AUDIO == type) || (IM::BaseDefine::MSG_TYPE_SINGLE_FILE == type) || (IM::BaseDefine::MSG_TYPE_SINGLE_SYSTEM == type)\
 || (IM::BaseDefine::MSG_TYPE_SINGLE_FILE_STATUS == type))\
{\
bRet = true;\
}\
bRet;\
})


#define CHECK_MSG_TYPE_GROUP(type) \
({\
bool bRet = false;\
if ((IM::BaseDefine::MSG_TYPE_GROUP_TEXT == type) || (IM::BaseDefine::MSG_TYPE_GROUP_AUDIO == type) || (IM::BaseDefine::MSG_TYPE_GROUP_FILE == type) || (IM::BaseDefine::MSG_TYPE_GROUP_SYSTEM == type)\
 || (IM::BaseDefine::MSG_TYPE_GROUP_FILE_STATUS == type))\
{\ 
bRet = true;\
}\
bRet;\
})




typedef struct AudioMsgInfo{
    uint32_t    audioId;
    uint32_t    fileSize;
    uint32_t    data_len;
    uchar_t*    data;
    string      path;
    
} AudioMsgInfo_t;

typedef struct DBUserInfo_t
{
    uint32_t nId;//用户ID
    uint8_t nSex;// 用户性别 1.男;2.女
    uint8_t nStatus; // 用户状态0 正常， 1 离职
    uint32_t nDeptId;// 所属部门
    string strNick;// 花名
    string strDomain;// 花名拼音
    string strName;// 真名
    string strTel;// 手机号码
    string strEmail;// Email
    string strAvatar;// 头像
	uint32_t nJodNumber;
	string strSignature;
	string strDuties;
	string strPhone;	//固定电话
	string strFaxNum;
	string strPostCode;
	string strAddress;
	string strBirthday;
	DBUserInfo_t()
	{
		nId = 0;
		nSex = 1;
		nStatus = 0;
		nDeptId = 0;
		strNick = "";
		strDomain = "";
		strName = "";
		strTel = "";
		strEmail = "";
		strAvatar = "";
		nJodNumber = 0;
		strSignature = "";
		strDuties = "";
		strPhone = "";
		strFaxNum = "";
		strPostCode = "";
		strAddress = "";
		strBirthday = "";
	}
    DBUserInfo_t& operator=(const DBUserInfo_t& rhs)
    {
        if(this != &rhs)
        {
			nJodNumber = rhs.nJodNumber;
			strSignature = rhs.strSignature;
			strDuties = rhs.strDuties;
			strPhone = rhs.strPhone;
			strFaxNum = rhs.strFaxNum;
			strPostCode = rhs.strPostCode;
			strAddress = rhs.strAddress;
			strBirthday = rhs.strBirthday;
            nId = rhs.nId;
            nSex = rhs.nSex;
            nStatus = rhs.nStatus;
            nDeptId = rhs.nDeptId;
            strNick = rhs.strNick;
            strDomain = rhs.strDomain;
            strName = rhs.strName;
            strTel = rhs.strTel;
            strEmail = rhs.strEmail;
            strAvatar = rhs.strAvatar;
        }
        return *this;
    }
    
} DBUserInfo_t;

typedef hash_map<uint32_t, DBUserInfo_t*> DBUserMap_t;

typedef struct DBDeptInfo_t
{
    uint32_t nId;
    uint32_t nParentId;
    string strName;
    
    DBDeptInfo_t& operator=(const DBDeptInfo_t& rhs)
    {
        if(this != &rhs)
        {
            nId = rhs.nId;
            nParentId = rhs.nParentId;
            strName = rhs.strName;
        }
        return *this;
    }
    
} DBDeptInfo_t;

typedef hash_map<uint32_t, DBDeptInfo_t*> DBDeptMap_t;


typedef struct {
    uint32_t 	user_id;
    uint32_t	conn_cnt;
} user_conn_t;

typedef struct {
    uint32_t user_id;
    uint32_t status;
    uint32_t client_type;
} user_stat_t;

typedef struct
{
    uint32_t user_id;
    set<uint32_t> allow_user_ids;
    set<uint32_t> allow_group_ids;
    set<string>  authed_ips;
    set<string>  authed_interfaces;
} auth_struct;

#define MAX_MSG_LEN     4096

#endif
