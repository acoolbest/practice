#ifndef _LIB_LOGDATA_H_
#define _LIB_LOGDATA_H_

#define			XY_APP_SOURCE_TYPE							0x01A

#define			CRG_FAIL									0x0000		
#define			CRG_SUCCESS									0x0001
#define			CRG_INVALID_PARAM								0x0002

// install result
#define			CRG_INSTALL_SUCCESS								0x0001		// install success

#define			CRG_NOT_START_SYSTEM								0x0005		// install failure: shut down
#define			CRG_NOT_ENOUGH_BUFF								0x0006		// install failure: no space
#define			CRG_CANCELED_BY_USER								0x0007		// install failure: canceled by user
#define			CRG_DEVICE_NOT_FOUND								0x0008		// install failure: device not found OR device removed
#define			CRG_DEVICE_OFF_LINE								0x0009		// install failure: device offline
#define			CRG_DEVICE_INVALID_APP								0x000A		// install failure: invalid package
#define			CRG_VERIFICATION_TIMEOUT_BY_USER						0x000B		// install failure: verification timeout by user
#define			CRG_VERIFICATION_FAILURE_BY_USER						0x000C		// install failure: non-formal channels to install and verification failure by user
#define			CRG_OTHER_FAILURE								0x0		// install failure: unknown failure

#define			MAX_BUFF_LEN									260

typedef struct _CRG_IOS_DEVICE_INFO_STRUCT_CRG_
{
	bool		b_access;
	uint8_t		dw_proc_state;
	char 		sz_plugin_times[MAX_BUFF_LEN];
	char		sz_unique_dev_id[MAX_BUFF_LEN];				// unique device id, eg: dfcd798427ea5c9f6aec0ccca732d501bb623d91
	char		sz_vid[MAX_BUFF_LEN];						// vendor id, eg: 05ac
	char		sz_pid[MAX_BUFF_LEN];						// products id, eg: 1289
	char		sz_state[MAX_BUFF_LEN];
	char		sz_name[MAX_BUFF_LEN];						// device name, eg: "changchong"'s iphone
	char		sz_serial_no[MAX_BUFF_LEN];					// serial number, eg: C7GK41LXDTWF
	char		sz_product_type[MAX_BUFF_LEN];				// product type, eg: iphone5,2
	char		sz_product_version[MAX_BUFF_LEN];			// product version, eg: 9.0.2
	char		sz_phone_number[MAX_BUFF_LEN];				// phone number, eg: 13910410123
	char		sz_icc_id[MAX_BUFF_LEN];					// IntegratedCircuitCardIdentity, eg: 89860002191249809751
	char		sz_imei[MAX_BUFF_LEN];						// InternationalMobileEquipmentIdentity: 012338001532340
	
}crg_ios_dev_info_stru, *p_crg_ios_dev_info_stru;

typedef struct _CRG_DEVICE_PROPPERTY_STRUCT_{
	char Name[MAX_BUFF_LEN];											// device name, eg: "changchong"'s iphone
	char SerialNumber[MAX_BUFF_LEN];										// serial number, eg: C7GK41LXDTWF
	char ProductType[MAX_BUFF_LEN];											// product type, eg: iphone5,2
	char ProductVersion[MAX_BUFF_LEN];										// product version, eg: 9.0.2
	char PhoneNumber[MAX_BUFF_LEN];											// phone number, eg: 13910410123
	char ICCID[MAX_BUFF_LEN];											// IntegratedCircuitCardIdentity, eg: 89860002191249809751
	char IMEI[MAX_BUFF_LEN];											// InternationalMobileEquipmentIdentity: 012338001532340
} CrgDeviceProperty;

typedef struct _CRG_IOS_DEVICE_INFO_STRUCT_{
	char UniqueDeviceID[MAX_BUFF_LEN];										// unique device id, eg: dfcd798427ea5c9f6aec0ccca732d501bb623d91
	char szVid[MAX_BUFF_LEN];											// vendor id, eg: 05ac
	char szPid[MAX_BUFF_LEN];											// products id, eg: 1289
	CrgDeviceProperty devProp;
}CrgIosDeviceInfo, *pCrgIosDeviceInfo;

uint32_t crg_init_log_model_for_charge();
uint32_t crg_init_log_model_for_xy();
uint32_t crg_destory_log_model();
uint32_t crg_send_install_app_result(uint32_t dw_result, uint32_t dw_device_type, uint32_t dw_app_source, char* p_app_id, uint32_t dw_interval, void* p_devinfo);
uint32_t crg_send_heartbeat_info_to_server(char* p_terminal, char* p_version, char* p_net_new);
uint32_t crg_send_device_info_to_server(int dw_device_type, void* p_devinfo);

int Crg_InitLogModel();
int Crg_DestoryLogModel();
int Crg_SendInstallIOSAppResult(CrgIosDeviceInfo* p_devinfo,  uint32_t dw_app_source_type, char* p_app_id, uint32_t dw_interval, uint32_t dw_install_result);




#endif // _LIB_LOGDATA_H_
