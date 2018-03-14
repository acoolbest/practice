#ifndef _DEF_H_
#define _DEF_H_

#include <vector>
#include <map>
#include <string>
#include <string.h>
#include <stdint.h>

using namespace std;


// version info
#define SOFTWARE_POWER_ON_VERSION					("1.1.1")
#define SOFTWARE_POWER_ON_RELEASE_DATA				("2016-06-16")

#define SOFTWARE_POWER_OFF_VERSION					("1.0.0")
#define SOFTWARE_POWER_OFF_RELEASE_DATA				("2016-04-25")
//

// device state
//#define ANDROID_CONNECT_DEVICE_LIST_STR				"连接的设备列表"
#define ANDROID_CONNECT_DEVICE_LIST_STR				"List of devices attached"
#define IOS_CONNECT_DEVICE_LIST_STR					"ERROR: Unable to retrieve device list!"

#define ANDROID_DEVICE_STATUS						"device"
#define ANDROID_UNAUTHORIZED_STATUS					"unauthorized"
#define ANDROID_OFFLINE_STATUS						"offline"

#define IOS_DEVICE_STATUS							"device"
#define IOS_UNAUTHORIZED_STATUS						"unauthorized"
#define IOS_OFFLINE_STATUS							"offline"

// check device status timeout
#define WAIT_TIMEOUT_FOR_ANDROID_DEVICE				20	// 20s	// wait for change to "device" status
#define WAIT_TIMEOUT_FOR_IOS_DEVICE					20	// 20s	// wait for change to "device" status

// power param setting
#define HW_POWER_ON_TIME							10		// 10s
#define HW_POWER_OFF_TIME							0xFF	// 255s

// app auto run
#define APP_AUTO_RUN								"1"
#define APP_AUTO_UNRUN								"0"

// device type

#define			UNKNOWN_DEVICE_TYPE					0x0F
#define			ANDROID_DEVICE_TYPE					0x01	
#define			IOS_DEVICE_TYPE						0x02
#define			USB_DEVICE_TYPE						0x03

#define			APPLE_INT_VID						0x05AC
#define			APPLE_U_VID							"05AC"
#define			APPLE_L_VID							"05ac"

#define			CRG_FAIL							0x0000
#define			CRG_SUCCESS							0x0001
#define			CRG_INVALID_PARAM					0x0002
#define			CRG_FOUND							0x0003
#define			CRG_NOT_FOUND						0x0004
#define			CRG_NOT_START_SYSTEM				0x0005
#define			CRG_NOT_ENOUGH_BUFF					0x0006
#define			CRG_CANCELED_BY_USER				0x0007
#define			CRG_DEVICE_NOT_FOUND				0x0008
#define			CRG_DEVICE_OFF_LINE					0x0009
#define			CRG_DEVICE_INVALID_APK				0x000A
#define			CRG_VERIFICATION_TIMEOUT_BY_USER	0x000B
#define			CRG_VERIFICATION_FAILURE_BY_USER	0x000C

#define			CRG_TIMEOUT							0x000D

#define			CRG_NO_SUPPORT						0x000E
#define			CRG_NO_WORK							0x000F
#define			CRG_EMPTY_DATA						0x0010

#define			CRG_INTERNAL_ERROR_STR				0x0011
#define			CRG_DEVICE_ALREADY_EXISTS			0x0012

#define			_POWER_CONTROL_TURN_ON_				1
#define			_FILTER__INDUSTRIAL_PC_PERIPHERALS_	0

// POWER CONTROL
#define			HW_POWER_ON_STATUS					0x01
#define			HW_POWER_OFF_STATUS					0x02
#define			HW_DEFAULT_POWER_STATUS				HW_POWER_ON_STATUS

#define			HW_DEFAULT_CONTROL_POWER			HW_CONTROL_POWER_ON
#define			HW_CONTROL_POWER_ON					0x01
#define			HW_CONTROL_POWER_OFF				0x02


#define         BOARD_CONFIG_INI_FILE               "config/board_config.ini"
#define         CCKJ_BOARD_INFO_SECTION             "CCFL_BOARD_INFO"
#define         BOARD_NUM_KEY                       "board_num_%d"
#define         COM_NUM_KEY                         "com_num_%d"

#define         HW_POWER_OFF_CMD                    0x01
#define         HW_POWER_ON_CMD                     0x03
#define         HW_RESET_HUB_CMD                    0x04
#define			HW_QUERY_HUB_CMD					0x06
#define			HW_CONTROL_LIGHT_CMD				0x07

#define			HW_LIGHT_ON_STATUS					0x01		
#define			HW_LIGHT_OFF_STATUS					0x00

#define         MAX_COM_COUNT                       6

#define         MAX_SERIAL_BUFFER_LENGHT            0xff
#define         MAX_BOARD_CHILD_HUB_COUNT           4
#define         MAX_PER_PORT_COUNT                  4
#define         MAX_BOARD_ALL_PORT_COUNT            (MAX_BOARD_CHILD_HUB_COUNT*MAX_PER_PORT_COUNT)

#define         BOARD_ALL_HUB_PORT_COUNT            (MAX_BOARD_CHILD_HUB_COUNT + 1)

#define         RESPONSE_TIMEOUT                	1	// 1s

#define			MAX_PORT_PATH_LEN					10
// JSON
#define			APP_LIST_NAME_JSON					"config/APPList.json"
#define			USERINFO_NAME_JSON					"config/userInfo.json"
#define			NET_TRAFFIC_JSON					"config/net_traffic.json"
#define			NET_TRAFFIC_SH						"application/net_traffic.sh"
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
#define			RESET_BOARD_FUNC					0x00
#define			INIT_BOARD_FUNC						0x01

#define			APP_SOURCE_FROM_CC					0x01
#define			APP_SOURCE_FROM_XY					0x02

#define			MAX_LEN								4096

#define			MAX_DEVICE_COUNT					127

#define			MAX_BUFF_LEN						260

#define			MAX_PATH							260
//
#define			MAX_THREADPOOL_COUNT				10 // MAX_DEVICE_COUNT

//
#define			INCREASE_TYPE						0x001
#define			DECREASE_TYPE						0x002
#define			GET_TYPE							0x003

//
#define			CCKJ_BOARD_PER_PORT_COUNT			4
//
#define			DEFAULT_BOARD_TYPE					0x00			// 工控机默认板子
#define			CCKJ_BOARD_TYPE						0x01			// 畅充&&方蓝电路板


#define     	CCKJ_VID                            0x05E3          // 畅充&&方蓝VID
#define     	CCFL_L_PRODUCT                      "ccflhub"
#define     	CCKJ_L_PRODUCT                      "cckjhub"
//
#define			PROC_IDLE_STATE						0x00
#define			PROC_BUSY_STATE						0x01
//
#define		CHARGEDOWNLOADOK		(2)

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
	uint8_t u8_board_type;
	int dw_hub_port_num[BOARD_ALL_HUB_PORT_COUNT];
	string str_object;
}board_device_info, *p_board_device_info;

// APP STRUCT
typedef struct _INSTALL_ANDROID_APP_STRUCT_
{
	char sz_apk_id[MAX_BUFF_LEN];
	char sz_apk_name[MAX_BUFF_LEN];
	char sz_apk_package[MAX_BUFF_LEN];
	char sz_apk_path[MAX_BUFF_LEN];
	char sz_apk_version[MAX_BUFF_LEN];
	char sz_apk_auto_run[MAX_BUFF_LEN];
}install_android_app_info_stru, *p_install_android_app_info_stru;

// DEVICE STRUCT
typedef struct _ANDROID_DEVICE_INFO_STRUCT_
{
	bool								b_access;
	uint8_t								dw_proc_state;						// PROC_IDLE_STATE OR PROC_BUSY_STATE
	char 								sz_plugin_times[MAX_BUFF_LEN];
	char								sz_dev_id[MAX_BUFF_LEN];
	char								sz_vid[MAX_BUFF_LEN];
	char								sz_pid[MAX_BUFF_LEN];
	char								sz_name[MAX_BUFF_LEN];
	char								sz_brand[MAX_BUFF_LEN];
	char								sz_model[MAX_BUFF_LEN];
	char								sz_path[MAX_BUFF_LEN];
	char								sz_object[MAX_BUFF_LEN];
	char								sz_state[MAX_BUFF_LEN];
	char								sz_os_version[MAX_BUFF_LEN];
}android_dev_info_stru;

typedef struct _IOS_DEVICE_INFO_STRUCT_
{
	bool								b_access;
	uint8_t								dw_proc_state;						// PROC_IDLE_STATE OR PROC_BUSY_STATE
	char 								sz_plugin_times[MAX_BUFF_LEN];
	char								sz_unique_dev_id[MAX_BUFF_LEN];
	char								sz_vid[MAX_BUFF_LEN];
	char								sz_pid[MAX_BUFF_LEN];
	char								sz_name[MAX_BUFF_LEN];
	char								sz_serial_no[MAX_BUFF_LEN];
	char								sz_product_type[MAX_BUFF_LEN];
	char								sz_product_version[MAX_BUFF_LEN];
	char								sz_phone_number[MAX_BUFF_LEN];
	char								sz_icc_id[MAX_BUFF_LEN];
	char								sz_object[MAX_LEN];
	char								sz_state[MAX_BUFF_LEN];
	char								sz_imei[MAX_BUFF_LEN];
}ios_dev_info_stru;

typedef struct _USB_DEVICE_INFO_STRUCT_
{
	uint8_t								u8_bus_num;
	uint8_t								u8_addr_num;
	uint8_t								u8_board_type;
	uint8_t								u8_board_port_num;
	char								sz_board_num[MAX_BUFF_LEN];
	char								sz_vid[MAX_BUFF_LEN];
	char								sz_pid[MAX_BUFF_LEN];
	char								sz_plugin_times[MAX_BUFF_LEN];			
	char								sz_port_num[MAX_PORT_PATH_LEN];
	vector<string>						vec_object;
	vector<string>						vec_dev_id;
}usb_dev_info_stru, *p_usb_dev_info_stru;

// all device_info
typedef struct _DEVICE_INFO_
{
	uint32_t							dw_dev_type;							// eg: ANDROID_DEVICE_TYPE，IOS_DEVICE_TYPE，UNKNOWN_DEVICE_TYPE
	usb_dev_info_stru					usb_dev_info;
	void*								p_identify_dev_info;					// eg: (ios_dev_info_stru*)malloc(sizeof(ios_dev_info_stru))
}dev_all_info_stru;

// UPLOAD LOG INFO
typedef struct _DEVICE_LOG_INFO_STRUCT_
{
	uint32_t							dw_dev_type;							// 设备类型
	bool								b_access;								// 是否可以访问
	char								sz_dev_id[MAX_BUFF_LEN];				// 设备id
	char								sz_model[MAX_BUFF_LEN];				// 设备型号
	char								sz_phone_number[MAX_BUFF_LEN];			// 手机号码
	char								sz_sys_param[MAX_BUFF_LEN];				// 设备系统
	char								sz_sys_version[MAX_BUFF_LEN];			// 设备系统版本号
	char								sz_terminal_id[MAX_BUFF_LEN];			// 充电设备号
	char								sz_description[MAX_BUFF_LEN];			// 充电设备描述
	char								sz_vid[MAX_BUFF_LEN];					// 设备生产厂商ID
	char								sz_pid[MAX_BUFF_LEN];					// 设备产品ID
	char								sz_plugin_times[MAX_BUFF_LEN];			// 设备插入时间
	char								sz_log_times[MAX_BUFF_LEN];				// 生成LOG时间
}device_log_info, *p_device_log_info;

typedef struct _INSTALL_APP_TIME_STRUCT_
{
	char								sz_app_id[MAX_BUFF_LEN];				// 应用 id
	uint32_t							dw_time_interval;						// 安装时长（单位：S）
}install_app_time_info, *p_install_app_time_info;

typedef struct _INSTALL_APP_RESULT_LOG_STRUCT_
{
	uint32_t							dw_result;										// 安装结果
	uint32_t							dw_app_source;									// 应用来源
	device_log_info						device_info;							
	vector<install_app_time_info>		vec_installed_app_time;			// 安装时长
}install_app_result_log_info, *p_install_app_result_log_info;

struct user_config
{
	int time;//minu
	string ver_url;//
	string success_url;//
	string fail_url;//
	string log_url;//
	string heart_url;//
	string account;//
	string password;//
	string driver_path;//
	string webPath;//新增web更新接口目录，151026，何
	string appPath;//
	string cmdPath;//
	string appsVersion;
	string installVersion;
	string driverVersion;
	string webVersion; //新增web更新接口版本号，151026，何
	string province;
	string terminal;
	int try_download_count;//下载重复次数，160513，何
	string release_version;
	string release_date;
};

struct net_traffic_stru
{
	string net_new;
	string net_old;
};

#define NULL_STR									"NULL"
//
#define PRINT_START_STR								"[START]\n"
#define PRINT_FINISH_STR							"[FINISH]\n"
#define PRINT_END_STR								"[END]\n"
#define PRINT_OK_STR								"[OK]\n"
#define PRINT_FAIL_STR								"[FAIL]\n"
#define PRINT_BUG_STR								"[BUG]\n"
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
#define PRINT_EMPTY_FILE_STR						"[EMPTY FILE]\n"
#define PRINT_EMPTY_DATA_STR						"[EMPTY DATA]\n"
#define PRINT_FOUND_STR								"[FOUND]\n"
#define PRINT_NEGLECT_DEVICE_STR					"[NEGLECT DEVICE]\n"
#define PRINT_INDUSTRIAL_PC_PERIPHERALS_STR			"[INDUSTRIAL PC PERIPHERALS]\n"
#define PRINT_REMOVED_STR							"[REMOVED]\n"
#define PRINT_EXCEPTION_STR							"[EXCEPTION]\n"
#define PRINT_POWER_ON_STR							"[POWER ON]\n"
#define PRINT_POWER_OFF_STR							"[POWER OFF]\n"
#define PRINT_TIMEOUT_STR							"[TIMEOUT]\n"
#define PRINT_POINT_STR								" ----------> "

#define	PRINT_UNKNOWN_STR							"[UNKNOWN]"
#define	PRINT_ANDROID_STR							"[ANDROID]"
#define	PRINT_IOS_STR								"[IOS]"
#define	PRINT_USB_STR								"[USB]"

#endif // _DEF_H_

