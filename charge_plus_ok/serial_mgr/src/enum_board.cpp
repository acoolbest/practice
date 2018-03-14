
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <map> 
#include <iostream>
#include <fstream>
#include <time.h>
#include <algorithm>

#include "public_func/src/def.h"
#include "third_party/libusbx/libusb/libusb.h"
#include "public_func/src/public_function.h"
#include "enum_board.h"

using namespace std;

// #define		usbi_mutex_t            HANDLE
#define 		usbi_mutex_t		pthread_mutex_t

struct list_head {
	struct list_head *prev, *next;
};

struct libusb_device {
	/* lock protects refcnt, everything else is finalized at initialization
	 * time */
	usbi_mutex_t lock;
	int refcnt;

	struct libusb_context *ctx;

	uint8_t bus_number;
	uint8_t port_number;
	struct libusb_device* parent_dev;
	uint8_t device_address;
	uint8_t num_configurations;
	enum libusb_speed speed;

	struct list_head list;
	unsigned long session_data;

	struct libusb_device_descriptor device_descriptor;
	int attached;

	uint8_t os_priv
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
		[] /* valid C99 code */
#else
		[0] /* non-standard, but usually working code */
#endif
			;
};

static int get_device_id_path(libusb_device *dev, string& strPath)
{
	int ret = CRG_FAIL;
	struct libusb_device_descriptor desc;
	int    r = libusb_get_device_descriptor(dev, &desc);
	char str_serial_num[128] = {0};
	if (r >= 0) 
	{
		struct libusb_device_handle *handle = NULL;
		int  rg = libusb_open(dev, &handle);
		//printf("get_str_serial_num libusb_open ret is %d\n",rg);
		if(rg == 0){
			if (libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, (uint8_t*)str_serial_num, 128) >= 0) {
				//printf("   String (0x%02X): \"%s\"\n",desc.iSerialNumber, str_serial_num);
				strPath = str_serial_num;
				ret = CRG_SUCCESS;
			}
			libusb_close (handle);
		}
	}
	return ret;
}
#if 0
static uint32_t GetUsbDeviceSerialNo(const char* pPath, string & strOutSerialNo)
{
	if (pPath == NULL || strlen(pPath) <= 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	string strPath(pPath);
	string::size_type  pid_index = 0, vid_index = 0, last_sharp_index = 0;
	char szSerialNo[MAX_LEN] = {0};

	if ((pid_index = strPath.find("PID")) != string::npos
			&& (vid_index = strPath.find("VID")) != string::npos
			&& (last_sharp_index = strPath.rfind("#")) != string::npos)
	{
		memcpy((void*)szSerialNo, (void*)strPath.substr(last_sharp_index + 1).c_str(), strPath.length() - last_sharp_index - 1);
		strOutSerialNo = szSerialNo;
		ret = CRG_SUCCESS;
	}
	return ret;
}

void printf_device_descriptor(libusb_device_descriptor *desc)
{
	printf("bLength:             %u\n", desc->bLength);
	printf("bDescriptorType:     %02Xh\n", desc->bDescriptorType);
	printf("bcdUSB:              %04Xh\n", desc->bcdUSB);
	printf("bDeviceClass:        %02Xh\n", desc->bDeviceClass);
	printf("bDeviceSubClass:     %02Xh\n", desc->bDeviceSubClass);
	printf("bDeviceProtocol:     %02Xh\n", desc->bDeviceProtocol);
	printf("bMaxPacketSize0:     %02Xh\n", desc->bMaxPacketSize0);
	printf("idVendor:            %04Xh\n", desc->idVendor);
	printf("idProduct:           %04Xh\n", desc->idProduct);
	printf("bcdDevice:           %04Xh\n", desc->bcdDevice);
	printf("iManufacturer:       %u\n", desc->iManufacturer);
	printf("iProduct:            %u\n", desc->iProduct);
	printf("iSerialNumber:       %u\n", desc->iSerialNumber);
	printf("bNumConfigurations:  %u\n", desc->bNumConfigurations);
	printf("\n\n");
}
#endif

static bool is_board_device(uint16_t id_vendor, string str_dev_id_path, uint16_t id_parent_vendor, string str_parent_dev_id_path, uint8_t & u8_board_type, bool &b_root_hub)
{
	bool bret = false;
	if (id_vendor == CCKJ_VID)
	{
		string tmp_path;
		tmp_path = strlwr((char *)str_parent_dev_id_path.c_str());
		if (id_parent_vendor == CCKJ_VID && (tmp_path.find(CCFL_L_PRODUCT) != string::npos || tmp_path.find(CCKJ_L_PRODUCT) != string::npos))
		{
			u8_board_type = CCKJ_BOARD_TYPE ; // Whether or not "CCFLHUB", it is always CCKJ_BOARD_TYPE
			b_root_hub = false;
			bret = true;
		}
		else
		{
			tmp_path = strlwr((char *)str_dev_id_path.c_str());
			if (tmp_path.find(CCFL_L_PRODUCT) != string::npos || tmp_path.find(CCKJ_L_PRODUCT) != string::npos)
			{
				u8_board_type = CCKJ_BOARD_TYPE; // Whether or not "CCFLHUB", it is always CCKJ_BOARD_TYPE
				b_root_hub = true;
				bret = true;
			}
			else
			{
				u8_board_type = DEFAULT_BOARD_TYPE;
				bret = false;
			}
		}

	}
	else
	{
		u8_board_type = DEFAULT_BOARD_TYPE;
		bret = false;
	}
	return bret;
}

static uint32_t get_CCKJ_board_device_num(string str_dev_id_path, string &str_board_num)
{
	uint32_t ret = CRG_FAIL;
	string::size_type dw_CCKJ_index = 0;
	string tmp_path;
	tmp_path = strlwr((char *)str_dev_id_path.c_str());
	if ((dw_CCKJ_index = tmp_path.find(CCKJ_L_PRODUCT)) != string::npos)
	{
		str_board_num = tmp_path.substr(dw_CCKJ_index + strlen(CCKJ_L_PRODUCT));
		ret = CRG_SUCCESS;
	}
	return ret;
}

static uint32_t get_FL_board_device_num(string str_dev_id_path, string &str_board_num)
{
	uint32_t ret = CRG_FAIL;
	string::size_type dwCCFLIndex = 0;
	string tmp_path;
	tmp_path = strlwr((char *)str_dev_id_path.c_str());
	if ((dwCCFLIndex = tmp_path.find(CCFL_L_PRODUCT)) != string::npos)
	{
		str_board_num = tmp_path.substr(dwCCFLIndex + strlen(CCFL_L_PRODUCT));
		ret = CRG_SUCCESS;
	}
	return ret;
}

uint32_t get_board_device_num_from_dev_path(uint8_t u8_board_type, string str_dev_id_path, string &str_board_num)
{
	uint32_t dw_ret = CRG_FAIL;
	if (u8_board_type == CCKJ_BOARD_TYPE)
	{
		dw_ret = get_CCKJ_board_device_num(str_dev_id_path, str_board_num);
		if (dw_ret != CRG_SUCCESS)
		{
			dw_ret = get_FL_board_device_num(str_dev_id_path, str_board_num);
		}
	}
	return dw_ret;
}

static uint32_t get_board_device_hub_info(libusb_device *dev, libusb_device_descriptor *p_desc, uint8_t u8_board_type, bool b_root_hub, board_device_info* p_board_info)
{
	if (dev == NULL || p_desc == NULL || p_board_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t dw_ret = CRG_FAIL;
	uint32_t dw_hub_port = 0;	
	if (u8_board_type == CCKJ_BOARD_TYPE)
	{
		dw_ret = CRG_SUCCESS;
		dw_hub_port = dev->port_number;

		p_board_info->u8_board_type = u8_board_type;
		
		if (b_root_hub)
		{
			get_device_id_path(dev, p_board_info->str_object);
			p_board_info->dw_hub_port_num[0] = dw_hub_port;
		}
		else
		{
			get_device_id_path(dev->parent_dev, p_board_info->str_object);
			if (dw_hub_port < BOARD_ALL_HUB_PORT_COUNT)
				p_board_info->dw_hub_port_num[dw_hub_port] = dw_hub_port;
		}	
	}
	return dw_ret;
}

uint32_t get_current_board_device_list(vector<board_device_info>* pvecDeviceInfo)
{
	if (pvecDeviceInfo == NULL)
		return CRG_INVALID_PARAM;

	libusb_device **devs = NULL;
	libusb_device *dev = NULL;
	int r;
	ssize_t cnt;
	board_device_info device_info = { 0 };
	string str_object;
	libusb_context *ctx = NULL;

	r = libusb_init(&ctx);
	if (r < 0)
		return CRG_FAIL;

	cnt = libusb_get_device_list(ctx, &devs);
	if (cnt >= 0)
	{
		int i = 0;
		while ((dev = devs[i++]) != NULL) 
		{
			bool b_found = false;
			uint8_t u8_board_type = 0;

			struct libusb_device_descriptor desc;
			r = libusb_get_device_descriptor(dev, &desc);
			if (r >= 0) 
			{
				bool b_root_hub = false;
				string parent_dev_id_path = "", dev_id_path = "";

				get_device_id_path(dev, dev_id_path);

				if(dev->parent_dev != NULL)
				{
					get_device_id_path(dev->parent_dev, parent_dev_id_path);
				}

				uint16_t parent_id_vendor = dev->parent_dev != NULL ? dev->parent_dev->device_descriptor.idVendor : 0x00;
				if (!is_board_device(desc.idVendor, dev_id_path.c_str(), parent_id_vendor, parent_dev_id_path, u8_board_type, b_root_hub))
				{
					continue;
				}

				memset(&device_info, 0, sizeof(board_device_info));
				if (get_board_device_hub_info(dev, &desc, u8_board_type, b_root_hub, &device_info) == CRG_SUCCESS)
				{
					for (uint32_t i = 0; i < pvecDeviceInfo->size(); i++)
					{
						if ((*pvecDeviceInfo)[i].str_object == device_info.str_object)
						{
							for (int m = 0; m < BOARD_ALL_HUB_PORT_COUNT; m++)
							{
								if (device_info.dw_hub_port_num[m] != 0)
									(*pvecDeviceInfo)[i].dw_hub_port_num[m] = device_info.dw_hub_port_num[m];
							}
							b_found = true;
							break;
						}
					}
					if (!b_found)
					{
						pvecDeviceInfo->push_back(device_info);
					}
				}
			}
		}

		libusb_free_device_list(devs, 1);
	}

	libusb_exit(ctx);

	return pvecDeviceInfo->size() > 0 ? CRG_SUCCESS : CRG_NOT_FOUND;
}
