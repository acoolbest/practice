#ifndef _DEF_H_
#define _DEF_H_

#include <vector>
#include <map>
#include <string>
#include <string.h>
//#include <mcheck.h>

using namespace std;

using std::vector;
using std::string;

// version info
#define SOFTWARE_POWER_ON_VERSION							("1.1.0")
#define SOFTWARE_POWER_ON_RELEASE_DATA						("2016-06-13")

#define SOFTWARE_POWER_OFF_VERSION							("1.0.0")
#define SOFTWARE_POWER_OFF_RELEASE_DATA						("2016-04-25")
//

// device state
//#define ANDROID_CONNECT_DEVICE_LIST_STR						"Á¬œÓµÄÉè±žÁÐ±í"
#define ANDROID_CONNECT_DEVICE_LIST_STR						"List of devices attached"
#define IOS_APP_LIST_STR						            "CFBundleIdentifier, CFBundleVersion, CFBundleDisplayName"
#define IOS_CONNECT_DEVICE_LIST_STR							"ERROR: Unable to retrieve device list!"

#define ANDROID_DEVICE_STATUS								"device"
#define ANDROID_UNAUTHORIZED_STATUS							"unauthorized"
#define ANDROID_OFFLINE_STATUS								"offline"

#define IOS_DEVICE_STATUS									"device"
#define IOS_UNAUTHORIZED_STATUS								"unauthorized"
#define IOS_OFFLINE_STATUS									"offline"

// check device status timeout
#define WAIT_TIMEOUT_FOR_ANDROID_DEVICE						20	// 20s	// wait for change to "device" status
#define WAIT_TIMEOUT_FOR_IOS_DEVICE							20	// 20s	// wait for change to "device" status

// power param setting
#define HW_POWER_ON_TIME									10		// 10s
#define HW_POWER_OFF_TIME									0xFF	// 255s

// app auto run
#define APP_AUTO_RUN										"1"
#define APP_AUTO_UNRUN										"0"

// device type

#define			UNKNOWN_DEVICE_TYPE						0x0F
#define			ANDROID_DEVICE_TYPE						0x01	
#define			IOS_DEVICE_TYPE							0x02
#define			USB_DEVICE_TYPE							0x03

#define			APPLE_INT_VID							0x05AC
#define			APPLE_U_VID								"05AC"
#define			APPLE_L_VID								"05ac"

#define			CRG_FAIL										0x0000
#define			CRG_SUCCESS										0x0001
#define			CRG_INVALID_PARAM								0x0002
#define			CRG_FOUND										0x0003
#define			CRG_NOT_FOUND									0x0004
#define			CRG_NOT_START_SYSTEM							0x0005
#define			CRG_NOT_ENOUGH_BUFF								0x0006
#define			CRG_CANCELED_BY_USER							0x0007
#define			CRG_DEVICE_NOT_FOUND							0x0008
#define			CRG_DEVICE_OFF_LINE								0x0009
#define			CRG_DEVICE_INVALID_APK							0x000A
#define			CRG_VERIFICATION_TIMEOUT_BY_USER				0x000B
#define			CRG_VERIFICATION_FAILURE_BY_USER				0x000C

#define			CRG_TIMEOUT					0x000D

#define			CRG_NO_SUPPORT				0x000E
#define			CRG_NO_WORK					0x000F

#define			CRG_INTERNAL_ERROR_STR		0x0010
#define			CRG_DEVICE_ALREADY_EXISTS	0x0011





#define			_POWER_CONTROL_TURN_ON_				1
// POWER CONTROL
#define			HW_POWER_ON_STATUS					0x01
#define			HW_POWER_OFF_STATUS					0x02
#define			HW_DEFAULT_POWER_STATUS				HW_POWER_ON_STATUS

#define			HW_DEFAULT_CONTROL_POWER			HW_CONTROL_POWER_ON
#define			HW_CONTROL_POWER_ON					0x01
#define			HW_CONTROL_POWER_OFF				0x02


#define         BOARD_CONFIG_INI_FILE                           "config/board_config.ini"
#define         CCKJ_BOARD_INFO_SECTION                         "CCFL_BOARD_INFO"
#define         BOARD_NUM_KEY                                   "board_num_%d"
#define         COM_NUM_KEY                                     "com_num_%d"

#define			_REPORT_PORT_CONTROL_TURN_ON_					1

#define         HW_REPORT_PORT_STATUS_CMD                       0x00
#define         HW_POWER_OFF_CMD                                0x01
#define         HW_POWER_ON_CMD                                 0x03
#define         HW_RESET_HUB_CMD                                0x04
#define			HW_QUERY_HUB_CMD								0x06
#define			HW_CONTROL_LIGHT_CMD							0x07

#define			HW_LIGHT_ON_STATUS								0x01		
#define			HW_LIGHT_OFF_STATUS								0x00

#define         MAX_COM_COUNT                           10

#define         MAX_SERIAL_BUFFER_LENGHT                0xff
#define         MAX_BOARD_CHILD_HUB_COUNT               4
#define         MAX_PER_PORT_COUNT                      4
#define         MAX_BOARD_ALL_PORT_COUNT                (MAX_BOARD_CHILD_HUB_COUNT*MAX_PER_PORT_COUNT)

#define         BOARD_ALL_HUB_PORT_COUNT                (MAX_BOARD_CHILD_HUB_COUNT + 1)

#define         RESPONSE_TIMEOUT                1000


// JSON
#define			APP_LIST_NAME_JSON					"config/APPList.json"
#define			USERINFO_NAME_JSON					"config/userInfo.json"
#define			CONFIG_NAME_JSON					"application/config.json"

// android && ios
#define			ANDROID_STR							"ANDROID"
#define			IOS_STR								"IOS"
#define			APP_ID_KEY							"AppId"
#define			APP_ICON_STR					    "AppIcon"
#define			APP_BUNDLE_ID_KEY					"AppBundleId"
#define			APP_NAME_STR						"AppName"
#define			APP_PATH_STR						"AppPath"
#define			APP_VERSION_STR						"AppVersion"
#define			APP_AUTO_RUN_STR					"autoOpen"
//

#define			APP_SOURCE_FROM_CC					0x01
#define			APP_SOURCE_FROM_XY					0x02

#define			MAX_LEN								4096

#define			MAX_DEVICE_COUNT					127

#define			MAX_BUFF_LEN						260
//
#define			MAX_THREADPOOL_COUNT				MAX_DEVICE_COUNT

//
#define		INCREASE_TYPE									0x001
#define		DECREASE_TYPE									0x002
#define		GET_TYPE										0x003

//
#define		CCKJ_BOARD_PER_PORT_COUNT						4
//
#define		DEFAULT_BOARD_TYPE								0x00			// ¹€¿Ø»úÄ¬ÈÏ°å×Ó
#define		CCKJ_BOARD_TYPE									0x01			// ³©³ä&&·œÀ¶µçÂ·°å


#define         CCKJ_VID                                        0x05E3          // ³©³ä&&·œÀ¶VID
#define         CCFL_L_PRODUCT                                  "ccflhub"
#define         CCKJ_L_PRODUCT                                  "cckjhub"
//


//
#define		CHARGEDOWNLOADOK		(2)

#ifndef _WIN32
typedef unsigned long DWORD;
#define INVALID_HANDLE_VALUE    (void*)0xffffffff
#define TRUE    1
#define FALSE   0
#endif  //#ifndef _WIN32


#ifndef _WIN32
typedef void* HANDLE;
#endif

//
enum statu_control
{
	statu_erro,
	disconnect,
	connected
};

typedef struct _BOARD_USB_DEVICE_INFO_STRUCT_
{
	int dwBoardType;
	int dwHubPortNum[BOARD_ALL_HUB_PORT_COUNT];
	string strObject;


}BoardDeviceInfo, *pBoardDeviceInfo;

typedef struct _PORT_STATUS_STRUCT_
{
	char boardID; 		//Éè±ž±àºÅ
	char portNum;		//¶Ë¿ÚºÅ
	char status;		//µ±Ç°×ŽÌ¬£š0x00:¶Ïµç		0x01:ÕýÔÚ³äµç	0x02:¹ÊÕÏ£©
}PortStatusInfo, *PPortStatusInfo;

typedef void(*CLIENTCALLBACK)(int dwComPortNum, std::string strBoardNum, PortStatusInfo* pInfo, int len);

typedef struct DeviceProperty_Struct{
	//char IdentifyNumber[MAX_BUFF_LEN];
	char Name[MAX_BUFF_LEN];
	char SerialNumber[MAX_BUFF_LEN];
	char ProductType[MAX_BUFF_LEN];
	char ProductVersion[MAX_BUFF_LEN];
	char PhoneNumber[MAX_BUFF_LEN];
	char ICCID[MAX_BUFF_LEN];
	char IMEI[MAX_BUFF_LEN];
} DeviceProperty;


// APP STRUCT
typedef struct _INSTALL_ANDROID_APP_STRUCT_
{
	char szApkID[MAX_BUFF_LEN];
	char szApkName[MAX_BUFF_LEN];
	char szApkPackage[MAX_BUFF_LEN];
	char szApkPath[MAX_BUFF_LEN];
	char szApkVersion[MAX_BUFF_LEN];
	char szApkAutoRun[MAX_BUFF_LEN];
}InstallAndroidAppInfo, *pInstallAndroidAppInfo;

typedef struct _INSTALL_IOS_APP_STRUCT_
{
	char szIpaID[MAX_BUFF_LEN];
	char szIpaName[MAX_BUFF_LEN];
	char szIpaBundleId[MAX_BUFF_LEN];
	char szIpaPath[MAX_BUFF_LEN];
	char szIpaVersion[MAX_BUFF_LEN];
}InstallIosAppInfo, *pInstallIosAppInfo;

// DEVICE STRUCT
typedef struct _ANDROID_DEVICE_INFO_STRUCT_
{
	bool bAccess;
	char szPluginTimes[MAX_BUFF_LEN];
	char szDeviceId[MAX_BUFF_LEN];
	char szSerialNo[MAX_BUFF_LEN];
	char szVid[MAX_BUFF_LEN];
	char szPid[MAX_BUFF_LEN];
	char szName[MAX_BUFF_LEN];
	char szBrand[MAX_BUFF_LEN];
	char szModel[MAX_BUFF_LEN];
	char szPath[MAX_BUFF_LEN];
	char szObject[MAX_BUFF_LEN];
	char szState[MAX_BUFF_LEN];
	char szOsVersion[MAX_BUFF_LEN];
}AndroidDeviceInfo, *pAndroidDeviceInfo;

typedef struct _IOS_DEVICE_INFO_STRUCT_
{
	bool bAccess;
	char UniqueDeviceID[MAX_BUFF_LEN];
	char szPluginTimes[MAX_BUFF_LEN];
	char szVid[MAX_BUFF_LEN];
	char szPid[MAX_BUFF_LEN];
	char szState[MAX_BUFF_LEN];
	DeviceProperty devProp;
}IosDeviceInfo, *pIosDeviceInfo;

typedef struct _UNKOWN_DEVICE_INFO_STRUCT_
{
	char szPluginTimes[MAX_BUFF_LEN];
	char szVid[MAX_BUFF_LEN];
	char szPid[MAX_BUFF_LEN];
	char vecSerialNo[MAX_BUFF_LEN];
	//vector<string> vecDeviceId;				// Éè±žid
	//vector<string> vecSerialNo;		
}UnknownDeviceInfo, *pUnknownDeviceInfo;

typedef struct _USB_DEVICE_INFO_STRUCT_
{
	uint32_t dwBusNum;
	uint32_t dwAddressNum;

	uint32_t dwBoardType;
	uint32_t dwPortNum;
	char szBoardNum[MAX_BUFF_LEN];
	char szVid[MAX_BUFF_LEN];
	char szPid[MAX_BUFF_LEN];
	char vecSerialNo[MAX_BUFF_LEN];
	char szPluginTimes[MAX_BUFF_LEN];			// Éè±ž²åÈëÊ±Œä
	uint8_t port_number[8];						// ±£Žæ¶Ë¿ÚºÅµÄŸø¶ÔÂ·Ÿ¶
	//vector<string> vecObject;
	//vector<string> vecSerialNo;
}UsbDeviceInfo, *pUsbDeviceInfo;

// UPLOAD LOG INFO
typedef struct _DEVICE_LOG_INFO_STRUCT_
{
	uint32_t dwDevType;							// Éè±žÀàÐÍ
	bool bAccess;								// ÊÇ·ñ¿ÉÒÔ·ÃÎÊ
	char szDevId[MAX_BUFF_LEN];				// Éè±žid
	char szModel[MAX_BUFF_LEN];				// Éè±žÐÍºÅ
	char szPhoneNumber[MAX_BUFF_LEN];			// ÊÖ»úºÅÂë
	char szSysParam[MAX_BUFF_LEN];				// Éè±žÏµÍ³
	char szSysVersion[MAX_BUFF_LEN];			// Éè±žÏµÍ³°æ±ŸºÅ
	char szTerminalId[MAX_BUFF_LEN];			// ³äµçÉè±žºÅ
	char szDescription[MAX_BUFF_LEN];			// ³äµçÉè±žÃèÊö
	char szVid[MAX_BUFF_LEN];					// Éè±žÉú²ú³§ÉÌID
	char szPid[MAX_BUFF_LEN];					// Éè±ž²úÆ·ID
	char szPluginTimes[MAX_BUFF_LEN];			// Éè±ž²åÈëÊ±Œä
	char szLogTimes[MAX_BUFF_LEN];				// Éú³ÉLOGÊ±Œä
}DeviceLogInfo, *pDeviceLogInfo;

typedef struct _INSTALL_APP_TIME_STRUCT_
{
	char szAppId[MAX_BUFF_LEN];				// ÓŠÓÃ id
	uint32_t time_interval;						// °²×°Ê±³€£šµ¥Î»£ºS£©
}InstallAppTimeInfo, *pInstallAppTimeInfo;

typedef struct _INSTALL_APP_RESULT_LOG_STRUCT_
{
	uint32_t dwResult;										// °²×°œá¹û
	uint32_t dwAppSource;									// ÓŠÓÃÀŽÔŽ
	DeviceLogInfo deviceInfo;							
	vector<InstallAppTimeInfo> vecInstalledAppTime;			// °²×°Ê±³€
}InstallAppResultLogInfo, *pInstallAppResultLogInfo;

typedef struct _REPORT_PORT_STATUS_LOG_STRUCT_
{
	char szTerminalId[MAX_BUFF_LEN];			// ³äµçÉè±žºÅ
	char szBoardManufacturer[MAX_BUFF_LEN];	// µçÂ·°æ³§ÉÌ
	char szBoardSerialNum[MAX_BUFF_LEN];		// µçÂ·°åÄÚµÄÐòÁÐºÅ
	char szBoardVid[MAX_BUFF_LEN];				// µçÂ·°å³§ÉÌVID
	char szBoardPid[MAX_BUFF_LEN];				// µçÂ·°å²úÆ·PID
	char szPortInfo[MAX_BUFF_LEN];				// ¶Ë¿ÚÐÅÏ¢
	uint32_t dwPortInfoLen;						// ¶Ë¿ÚÐÅÏ¢³€¶È
	char szDescription[MAX_BUFF_LEN];			// ÃèÊö
	char szLogTimes[MAX_BUFF_LEN];				// Ê±Œä
}ReportPortStatusLogInfo, *pReportPortStatusLogInfo;

typedef struct  _tagDownLoadStruct
{
	bool						m_bCreateSubPath;
	bool						m_bOverWrite;
	unsigned	long			m_totalLen;
	unsigned	long			m_downLen;
	FILE 	*					m_fp;
	//	HWND					m_hWnd;
	int			m_downType;
	int							m_index;
	string						m_downLoadSrcUrl;
	string						m_downLoadDesPath;
	int							m_fileIndex;
	_tagDownLoadStruct()
	{
		m_bCreateSubPath = true;
		m_bOverWrite = true;
		m_totalLen = 0;
		m_downLen = 0;
		m_fp = NULL;
		//m_hWnd = NULL;
		m_downType = 0;
		m_index = 0;
		m_downLoadSrcUrl = "";
		m_downLoadDesPath = "";
		m_fileIndex = 0;
	}
}DOWNLOADSTRUCT, *PDownLoadStruct;

typedef struct _tagDownLoadProgress
{
	double				m_percent;
	int					m_downType;
	int					m_index;
	bool				m_bFinished;
	bool				m_bDownSucc;
	string				m_strDesPath;
	unsigned	long			m_totalLen;
	unsigned	long			m_downLen;

	_tagDownLoadProgress()
	{
		m_percent = 0;
		m_downType = 0;
		m_index = 4;
		m_bFinished = false;
		m_bDownSucc = true;
		m_strDesPath = "";
		m_totalLen = 0;
		m_downLen = 0;
	}
}DOWNLOADPROGRESS, *lpDownLoadProgress;


#define CONTENT_TYPE_JSON "application/json"
#define CONTENT_TYPE_TEXT "text/plain"
#define HTTP_REQUEST_ADRESS "http://58.68.240.102:8088/get_version"
#define FTP_DOWNLOAD_ADRESS "ftp://58.68.240.102"
#define UPDATA_TEMP_FLODER "updata"
#define USERINFO_PASSWORD "changchong"
#define USERINFO_NAME "charge123"

enum _requestFileType
{
	none,
	app_install,//°²×°°ü°æ±Ÿ
	client_install,//³©³ä°²×°³ÌÐò°æ±Ÿ
	thirdPart_dirver//µÚÈý·œÇý¶¯³ÌÐò°æ±Ÿ
};

struct FileVersion
{
	string appId;		//ÓŠÓÃ³ÌÐòID
	string appType;		//ÓŠÓÃ³ÌÐòÏµÍ³Àà±ð£¬0--Android, 1--Ios
	string appName;		//ÓŠÓÃ³ÌÐòÃû³Æ
	string appPkgName;	//ÓŠÓÃ°üÃû",
	string fileName;	//ÓŠÓÃÎÄŒþÃû",
	string md5;			//md5Öµ",
	string downUrl;		//ÏÂÔØµØÖ·"
	string version;		//ÓŠÓÃ°æ±ŸºÅ"
	string autoOpen;	//0:could not auto run app£» 1:auto run app£¬add by zhzq @ 2016-2-19 10:13:34
	//	string iconUrl;		//ÍŒ±êÏÂÔØµØÖ·
};

struct DownLoadNode
{
	string savePath;	//±£ŽæµØÖ·
	//	string iconPath;	//ÍŒ±ê±£ŽæÂ·Ÿ¶
	FileVersion fileVersion;
	bool bNeedDown;
};



struct UserConfig
{
	int time;//minu
	string verUrl;//
	string sucessUrl;//
	string failUrl;//
	string logUrl;//
	string heartUrl;//
	string account;//
	string password;//
	string driverPath;//
	string webPath;//ÐÂÔöwebžüÐÂœÓ¿ÚÄ¿ÂŒ£¬151026£¬ºÎ
	string appPath;//
	string cmdPath;//
	string appsVersion;
	string installVersion;
	string driverVersion;
	string webVersion; //ÐÂÔöwebžüÐÂœÓ¿Ú°æ±ŸºÅ£¬151026£¬ºÎ
	string province;
	string terminal;
	int try_download_count;//ÏÂÔØÖØžŽŽÎÊý£¬160513£¬ºÎ
	int per_download_timeout;//每次下载超时时间，单位：分。
	//FTP是超时时间，阿里云为分段大小，按每秒1M换算.160616,何
};
typedef std::map<std::string, std::string> JsonValueMap;
typedef std::vector<FileVersion> VersionVector;

struct AppNode
{
	string strAppId;
	string strAppName;
	string strFileName;
	string strAppBundleId;
	string strAppPath;
	string strAppVersion;
	string strIconPath;
};
//

#define NULL_STR									"NULL"
//
#define PRINT_START_STR								"[START]\n"
#define PRINT_FINISH_STR							"[FINISH]\n"
#define PRINT_END_STR								"[END]\n"
#define PRINT_OK_STR								"[OK]\n"
#define PRINT_FAIL_STR								"[FAIL]\n"
#define PRINT_EXIST_STR								"[EXIST]\n"
#define PRINT_NOSPACE_STR							"[NO SPACE]\n"
#define PRINT_CANCELED_BY_USER_STR					"[CANCELED_BY_USER]\n"
#define PRINT_INTERNAL_ERROR_STR					"[INTERNAL_ERROR]"
#define PRINT_VERIFICATION_TIMEOUT_BY_USER_STR		"[VERIFICATION_TIMEOUT_BY_USER]\n"
#define PRINT_VERIFICATION_FAILURE_BY_USER_STR		"[VERIFICATION_FAILURE_BY_USER]\n"
#define PRINT_DEVICE_NOT_FOUND_STR					"[DEVICE REMOVED]\n"
#define PRINT_DEVICE_OFF_LINE_STR					"[DEVICE OFFLINE]\n"
#define PRINT_INVALID_APK_STR						"[INVALID_APK]\n"
#define PRINT_NOT_START_SYSTEM_STR					"[NOT START SYSTEM]\n"
#define PRINT_DEVICE_ALREADY_EXISTS_STR				"[DEVICE_ALREADY_EXISTS]\n"
#define PRINT_NOTFOUND_STR							"[NOT FOUND]\n"
#define PRINT_INVALID_BOARD_STR						"[INVALID BOARD]\n"
#define PRINT_ERR_FILE_STR							"[ERROR FILE]\n"
#define PRINT_FOUND_STR								"[FOUND]\n"
#define PRINT_REMOVED_STR							"[REMOVED]\n"
#define PRINT_EXCEPTION_STR							"[EXCEPTION]\n"
#define PRINT_POWER_ON_STR							"[POWER ON]\n"
#define PRINT_POWER_OFF_STR							"[POWER OFF]\n"
#define PRINT_TIMEOUT_STR							"[TIMEOUT]\n"
#define PRINT_POINT_STR								" ----------> "

typedef struct
{
	uint32_t global_exit;
	uint32_t com_exit;
	uint32_t usb_exit;
	uint32_t listenadb_exit;
	uint32_t listenios_exit;
	uint32_t heartbeat_exit;
	uint32_t update_exit;
}GlobalExit;
extern GlobalExit g_exit;


#endif // _DEF_H_

