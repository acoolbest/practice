#include "public_func/src/def.h"
#include "logdata.h"
#include "libsendlog.h"
#include "log_mgr.h"


#define XY_LOGDATADB_FILE	"CrgInstallAppLogData.db3"
#define	CRG_LOGDATADB_FILE	"LogData.db3"

static uint32_t get_internal_app_source_type(uint32_t dw_app_source_type, uint32_t &dw_internal_app_source_type)
{
	uint32_t ret = CRG_FAIL;
	if (dw_app_source_type == XY_APP_SOURCE_TYPE)
	{
		dw_internal_app_source_type = APP_SOURCE_FROM_XY;
		ret = CRG_SUCCESS;
	}
	return ret;
}

uint32_t crg_init_log_model_for_charge()
{
	return init_log_model(CRG_LOGDATADB_FILE);
}
uint32_t crg_init_log_model_for_xy()
{
	return init_log_model(XY_LOGDATADB_FILE);
}

uint32_t crg_destory_log_model()
{
	return destory_log_model();
}

uint32_t crg_send_install_app_result(uint32_t dw_result, uint32_t dw_device_type, uint32_t dw_app_source, char* p_app_id, uint32_t dw_interval, void* p_devinfo)
{
	return send_install_app_result(dw_result, dw_device_type, dw_app_source, p_app_id, dw_interval, p_devinfo);
}

uint32_t crg_send_heartbeat_info_to_server(char* p_terminal, char* p_version, char* p_net_new)
{
	return send_heartbeat_info_to_server(p_terminal, p_version, p_net_new);
}

uint32_t crg_send_device_info_to_server(int dw_device_type, void* p_devinfo)
{
	return send_device_info(dw_device_type, p_devinfo);
}

int Crg_InitLogModel()
{
	return crg_init_log_model_for_xy();
}

int Crg_DestoryLogModel()
{
	return destory_log_model();
}

int Crg_SendInstallIOSAppResult(CrgIosDeviceInfo* p_devinfo,  uint32_t dw_app_source_type, char* p_app_id, uint32_t dw_interval, uint32_t dw_install_result)
{
	uint32_t dw_internal_app_source_type = APP_SOURCE_FROM_XY;

	if (p_devinfo == NULL || p_app_id == NULL || dw_interval == 0 || get_internal_app_source_type(dw_app_source_type, dw_internal_app_source_type) != CRG_SUCCESS)
		return CRG_INVALID_PARAM;

	printf("LIB: Send install app result to service, get param, app source: %02x, app id: %s, install result: %d, install interval: %d, vid: %s, pid: %s\n", XY_APP_SOURCE_TYPE, p_app_id, dw_install_result, dw_interval, p_devinfo->szVid, p_devinfo->szPid);

	ios_dev_info_stru devinfo = {0};
	devinfo.b_access = true;
	memcpy(devinfo.sz_unique_dev_id, p_devinfo->UniqueDeviceID, MAX_BUFF_LEN);
	memcpy(devinfo.sz_vid, p_devinfo->szVid, MAX_BUFF_LEN);
	memcpy(devinfo.sz_pid, p_devinfo->szPid, MAX_BUFF_LEN);
	memset(devinfo.sz_plugin_times, 0, MAX_BUFF_LEN);
				
	memcpy(devinfo.sz_name, p_devinfo->devProp.Name, MAX_BUFF_LEN);
	memcpy(devinfo.sz_serial_no, p_devinfo->devProp.SerialNumber, MAX_BUFF_LEN);
	memcpy(devinfo.sz_product_type, p_devinfo->devProp.ProductType, MAX_BUFF_LEN);
	memcpy(devinfo.sz_product_version, p_devinfo->devProp.ProductVersion, MAX_BUFF_LEN);
	memcpy(devinfo.sz_phone_number, p_devinfo->devProp.PhoneNumber, MAX_BUFF_LEN);
	memcpy(devinfo.sz_icc_id, p_devinfo->devProp.ICCID, MAX_BUFF_LEN);
	memcpy(devinfo.sz_imei, p_devinfo->devProp.IMEI, MAX_BUFF_LEN);

	snprintf(devinfo.sz_state, MAX_BUFF_LEN*sizeof(char), "{ \"SerialNumber\": \"%s\",  \"DeviceName\": \"%s\",  \"PhoneNumber\": \"%s\",  \"ProductVersion\": \"%s\",  \"ProductType\": \"%s\",  \"ICCID\": \"%s\",  \"IMEI\": \"%s\",  \"UniqueDeviceID\": \"%s\" }", devinfo.sz_serial_no, devinfo.sz_name, devinfo.sz_phone_number, devinfo.sz_product_version, devinfo.sz_product_type, devinfo.sz_icc_id, devinfo.sz_imei, devinfo.sz_unique_dev_id);

	return send_install_app_result(dw_install_result, IOS_DEVICE_TYPE, dw_internal_app_source_type, p_app_id, dw_interval, (void*)&devinfo);
}


