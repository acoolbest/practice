#include <string.h>
#include <string>
#include <algorithm>
#include <vector>
#include <map> 
#include <unistd.h>
#include <pthread.h>
#include "third_party/libusbx/libusb/libusb.h"
#include "third_party/libusbx/libusb/libusbi.h"

#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "usb_action_data.h"
#include "device_data.h"
#include "thread_pool.h"
#include "ios_mgr.h"
#include "android_mgr.h"
#include "power_control.h"
#include "log_mgr/src/libsendlog.h"
#include "listen_usb.h"

using namespace std;

static bool										g_inited = false;
static bool										g_usb_listen_inited = false;
static bool										g_thread_init_process = false;
static bool										g_thread_init_listen = false;

static bool										g_thread_running_process = true;
static bool										g_thread_running_listen = true;
static pthread_t 								g_pthread_pid_listen;
static pthread_t 								g_pthread_pid_process;

#define USB_PROCESS_PTHREAD						1
#define USB_LISTEN_PTHREAD						2

static bool is_thread_running(uint32_t thread_index)
{
	if(thread_index == USB_LISTEN_PTHREAD)
		return g_thread_running_listen;
	if(thread_index == USB_PROCESS_PTHREAD)
		return g_thread_running_process;
	else
		return false;
}

static void set_thread_running_status(uint32_t thread_index, bool b_running)
{
	if(thread_index == USB_LISTEN_PTHREAD) g_thread_running_listen = b_running;
	if(thread_index == USB_PROCESS_PTHREAD) g_thread_running_process = b_running;
}

static bool is_need_usb(libusb_device * dev)
{
	if (dev == NULL)
		return false;

	int r = 0;
	bool need = true;
	libusb_device_descriptor desc = {0};

	r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) // error
		return false;

	if (desc.idVendor == APPLE_INT_VID)
		return true;

	if ((desc.idVendor == 0x0 && desc.idProduct == 0x0)				// 无用设备
		|| (desc.idVendor == 0x0403 && desc.idProduct == 0x6001)	// USB转串口线
		|| (desc.idVendor == 0x0930 && desc.idProduct == 0x6544)    // USB转串口线
		|| (desc.idVendor == 0x0bda && desc.idProduct == 0x5756)	// USB转串口线
		|| (desc.idVendor == 0x0bda && desc.idProduct == 0x0184)	// 无线网卡
		|| (desc.idVendor == 0x12d1 && desc.idProduct == 0x15c1)	// USB转串口线
		)
	{
		return false;
	}

	for (int c = 0; c < desc.bNumConfigurations; c++)
	{
		struct libusb_config_descriptor* config_desc = NULL;

		if (libusb_get_config_descriptor(dev, c, &config_desc) == 0)
		{
			for (int m = 0; m < config_desc->bNumInterfaces; m++)
			{
				for (int n = 0; n < config_desc->interface[m].num_altsetting; n++)
				{
					/*	write_log("config_desc->interface[%d].altsetting[%d].bInterfaceClass: %02x, bInterfaceSubClass: %02x\n",
					m, n,
					config_desc->interface[m].altsetting[n].bInterfaceClass,
					config_desc->interface[m].altsetting[n].bInterfaceSubClass);
					*/
					if (config_desc->interface[m].altsetting[n].bInterfaceClass == LIBUSB_CLASS_HID
						|| config_desc->interface[m].altsetting[n].bInterfaceClass == LIBUSB_CLASS_HUB
						/*	|| config_desc->interface[m].altsetting[n].bInterfaceClass == LIBUSB_CLASS_WIRELESS */)
					{
						need = false;
						goto _ONE_CONFIG_DESC_END_;
					}
				}

			}
		_ONE_CONFIG_DESC_END_:
			libusb_free_config_descriptor(config_desc);
		}
		else
			need = false;
		
		if (!need)
			break;
	}

	return need;
}

static int get_cckj_board_num(string board_id_path, string &out_board_num)
{
	uint32_t ret = CRG_FAIL;
	string::size_type cckj_index = 0;
	string board_id_path_tmp = string(board_id_path);
	transform(board_id_path_tmp.begin(), board_id_path_tmp.end(), board_id_path_tmp.begin(), (int(*)(int))tolower);

	if ((cckj_index = board_id_path_tmp.find(CCKJ_L_PRODUCT)) != string::npos)
	{
		out_board_num = board_id_path_tmp.substr(cckj_index + strlen(CCKJ_L_PRODUCT));
		ret = CRG_SUCCESS;
	}
	return ret;
}

static uint32_t get_flkj_board_num(string board_id_path, string &out_board_num)
{
	uint32_t ret = CRG_FAIL;
	string::size_type ccfl_index = 0;
	string board_id_path_tmp = string(board_id_path);
	transform(board_id_path_tmp.begin(), board_id_path_tmp.end(), board_id_path_tmp.begin(), (int(*)(int))tolower);

	if ((ccfl_index = board_id_path_tmp.find(CCFL_L_PRODUCT)) != string::npos)
	{
		out_board_num = board_id_path_tmp.substr(ccfl_index + strlen(CCFL_L_PRODUCT));
		ret = CRG_SUCCESS;
	}
	return ret;
}

static uint32_t get_usb_serial_num(libusb_device *dev, string & out_serial_num)
{
	if (dev == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char serial_num[MAX_BUFF_LEN] = { 0 };
	struct libusb_device_descriptor desc = {0};
	struct libusb_device_handle *handle = NULL;

	int rn = libusb_open(dev, &handle);

	if (rn != LIBUSB_SUCCESS)
		return rn == LIBUSB_ERROR_NO_DEVICE ? CRG_DEVICE_NOT_FOUND : CRG_FAIL;

	libusb_get_device_descriptor(dev, &desc);
	if (libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, (unsigned char*)serial_num, MAX_BUFF_LEN) >= 0)
	{
		out_serial_num = serial_num;
		ret = CRG_SUCCESS;
	}
	libusb_close(handle);

	return ret;
}

static uint32_t get_usb_position(libusb_device *dev, libusb_device_descriptor * desc, uint8_t & out_board_type, string & out_board_num, uint8_t & out_board_port_num, string & out_port_num)
{
	if (dev == NULL || desc == NULL)
		return CRG_INVALID_PARAM;

	int rn = 0;
	uint32_t ret = CRG_SUCCESS;
	string board_id_path;
	string board_num;
	uint8_t port_num[MAX_PORT_PATH_LEN] = {0};
	char temp_port_num[MAX_PORT_PATH_LEN] = { 0 };

	rn = libusb_get_port_numbers(dev, port_num, sizeof(port_num));
	if (rn < 0)
		return CRG_FAIL;
	
	convert_uint_to_string(temp_port_num, sizeof(temp_port_num), port_num, rn);
	
	out_board_type = DEFAULT_BOARD_TYPE;
	out_board_port_num = dev->port_number;
	out_port_num = temp_port_num;
	out_board_num = "0";

	if (dev->parent_dev != NULL && dev->parent_dev->parent_dev != NULL)
	{
		ret = get_usb_serial_num(dev->parent_dev->parent_dev, board_id_path);
		if (ret == CRG_SUCCESS)
		{
			if (get_cckj_board_num(board_id_path, board_num) == CRG_SUCCESS ||
				get_flkj_board_num(board_id_path, board_num) == CRG_SUCCESS)
			{
				out_board_type = CCKJ_BOARD_TYPE;
				out_board_port_num = (port_num[rn - 2] - 1) * CCKJ_BOARD_PER_PORT_COUNT + port_num[rn - 1] - 1;
				out_board_num = board_num;
			}
		}
	}
	return ret;
}

static uint32_t get_usb_info(libusb_device *dev, usb_dev_info_stru* out_usb_dev_info)
{
	if (dev == NULL || out_usb_dev_info == NULL)
		return CRG_INVALID_PARAM;

	int r = 0;
	uint32_t ret = CRG_FAIL;
	struct libusb_device_descriptor desc = { 0 };
	uint8_t board_type = DEFAULT_BOARD_TYPE;
	uint8_t board_port_num = 0;
	string port_num;
	string board_num; 
	string dev_id;

	if (!dev->attached || dev->refcnt <= 0) {
		return CRG_DEVICE_NOT_FOUND;
	}

	r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) // error
	{
		ret = CRG_FAIL;
		goto _GET_USB_INFO_EXIT_;
	}
	ret = get_usb_position(dev, &desc, board_type, board_num, board_port_num, port_num);
	if (ret != CRG_SUCCESS)
	{
		if (ret != CRG_DEVICE_NOT_FOUND)
				ret = CRG_FAIL;
		
		goto _GET_USB_INFO_EXIT_;
	}

	// clear_usb_dev_data(out_usb_dev_info);

	snprintf(out_usb_dev_info->sz_vid, sizeof(out_usb_dev_info->sz_vid), "%04x", desc.idVendor);
	snprintf(out_usb_dev_info->sz_pid, sizeof(out_usb_dev_info->sz_pid), "%04x", desc.idProduct);
	out_usb_dev_info->u8_board_type = board_type;
	memcpy(out_usb_dev_info->sz_board_num, board_num.c_str(), board_num.length());
	out_usb_dev_info->u8_board_port_num = board_port_num;
	memcpy(out_usb_dev_info->sz_port_num, port_num.c_str(), port_num.length());
	out_usb_dev_info->u8_bus_num = libusb_get_bus_number(dev);
	out_usb_dev_info->u8_addr_num = libusb_get_device_address(dev);
	ret = get_usb_serial_num(dev, dev_id);
	if (ret == CRG_SUCCESS)
	{
		out_usb_dev_info->vec_object.push_back(dev_id);
		out_usb_dev_info->vec_dev_id.push_back(dev_id);
	}

_GET_USB_INFO_EXIT_:
	return ret;
}

static uint32_t judge_board_type(libusb_device *dev)
{
	if(dev == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	string board_id_path;
	string board_num;
	
	if (dev->parent_dev != NULL && dev->parent_dev->parent_dev != NULL)
	{
		if (get_usb_serial_num(dev->parent_dev->parent_dev, board_id_path) == CRG_SUCCESS)
		{
			if (get_cckj_board_num(board_id_path, board_num) == CRG_SUCCESS ||
				get_flkj_board_num(board_id_path, board_num) == CRG_SUCCESS)
			{
				ret = CRG_SUCCESS;
			}
		}
	}
	return ret;
}

static uint32_t make_usb_plug_action_data(libusb_device *dev, string & out_dev_path, string & out_vid, string & out_pid, usb_dev_info_stru* out_usb_dev_info)
{
	if (dev == NULL || out_usb_dev_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	string str_time;
	char dev_path[MAX_PORT_PATH_LEN] = { 0 };

	// get usb device infomation
	ret = get_usb_info(dev, out_usb_dev_info);
	if (ret != CRG_SUCCESS) // removed OR other failure
		return ret;
	
	// get current time
	str_time = get_current_time();
	memcpy(out_usb_dev_info->sz_plugin_times, str_time.c_str(), str_time.length());

	convert_uint_to_string(dev_path, sizeof(dev_path), &out_usb_dev_info->u8_bus_num, 1);
	memcpy(dev_path + strlen(dev_path), out_usb_dev_info->sz_port_num, sizeof(out_usb_dev_info->sz_port_num) - strlen(dev_path));

	out_vid = out_usb_dev_info->sz_vid;
	out_pid = out_usb_dev_info->sz_pid;
	out_dev_path = dev_path;

	return CRG_SUCCESS;
}

static uint32_t make_usb_unplug_action_data(libusb_device *dev, string & out_dev_path, string & out_vid, string & out_pid)
{
	if (dev == NULL)
		return CRG_INVALID_PARAM;

	int r = 0;

	char vid[5] = { 0 };
	char pid[5] = { 0 };
	uint8_t dev_path[MAX_PORT_PATH_LEN] = { 0 };
	char temp_dev_path[MAX_PORT_PATH_LEN] = { 0 };
	struct libusb_device_descriptor desc = { 0 };
	
	r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) // error
		return CRG_FAIL;
	
	dev_path[0] = libusb_get_bus_number(dev);
	r = libusb_get_port_numbers(dev, dev_path + 1, sizeof(dev_path)-1);
	if (r < 0)
		return CRG_FAIL;

	dev_path[r + 1] = '\0';
	snprintf(vid, sizeof(vid), "%04x", desc.idVendor);
	snprintf(pid, sizeof(pid), "%04x", desc.idProduct);

	convert_uint_to_string(temp_dev_path, sizeof(temp_dev_path), dev_path, r + 1);

	out_vid = vid;
	out_pid = pid;
	out_dev_path = temp_dev_path;

	return CRG_SUCCESS;
}

static uint32_t usb_hotplug_action(libusb_device *dev, uint8_t action)
{
	uint32_t ret = CRG_FAIL;

	if (dev == NULL || (action != ACTION_PLUG_IN && action != ACTION_UNPLUG))
		return CRG_INVALID_PARAM;
	
	if (!is_thread_running(USB_LISTEN_PTHREAD))
		return CRG_NO_WORK;

	if (!is_need_usb(dev))
	{
		write_log("%s new usb device action, vid: %04x, pid: %04x%s%s", PRINT_USB_STR, dev->device_descriptor.idVendor, dev->device_descriptor.idProduct, PRINT_POINT_STR, PRINT_NEGLECT_DEVICE_STR);
		return CRG_FAIL;
	}
#if _FILTER__INDUSTRIAL_PC_PERIPHERALS_
	if(judge_board_type(dev) != CRG_SUCCESS)
	{
		write_log("%s new usb device action, vid: %04x, pid: %04x%s%s", PRINT_USB_STR, dev->device_descriptor.idVendor, dev->device_descriptor.idProduct, PRINT_POINT_STR, PRINT_INDUSTRIAL_PC_PERIPHERALS_STR);
		return CRG_FAIL;
	}
#endif

	if (action == ACTION_PLUG_IN)
		write_log("%s new usb device action, vid: %04x, pid: %04x%s%s", PRINT_USB_STR, dev->device_descriptor.idVendor, dev->device_descriptor.idProduct, PRINT_POINT_STR, PRINT_FOUND_STR);
	else
		write_log("%s usb device action, vid: %04x, pid: %04x%s%s", PRINT_USB_STR, dev->device_descriptor.idVendor, dev->device_descriptor.idProduct, PRINT_POINT_STR, PRINT_REMOVED_STR);

	string dev_path, vid, pid;
	
	if (action == ACTION_PLUG_IN)
	{
		usb_dev_info_stru usb_dev_info;
		clear_usb_dev_data(&usb_dev_info);
		ret = make_usb_plug_action_data(dev, dev_path, vid, pid, &usb_dev_info);
		if (ret == CRG_SUCCESS)
			ret = save_usb_action_data_to_list(dev, dev_path, vid, pid, &usb_dev_info, action);
		else if (ret == CRG_DEVICE_NOT_FOUND)
			write_log("%s can't get device info and no need save it to the current usb list, because of device was removed, vid: %04x, pid: %04x%s%s",
				PRINT_USB_STR, dev->device_descriptor.idVendor, dev->device_descriptor.idProduct, PRINT_POINT_STR, PRINT_DEVICE_NOT_FOUND_STR);
		else
			write_log("%s can't get device info and no need save it to the current usb list, vid: %04x, pid: %04x%s%s",
				PRINT_USB_STR, dev->device_descriptor.idVendor, dev->device_descriptor.idProduct, PRINT_POINT_STR, PRINT_FAIL_STR);
		clear_usb_dev_data(&usb_dev_info);
	}
	else
	{
		if (make_usb_unplug_action_data(dev, dev_path, vid, pid) == CRG_SUCCESS
			&& save_usb_action_data_to_list(dev, dev_path, vid, pid, NULL, action) == CRG_SUCCESS)
		{
			ret = CRG_SUCCESS;
		}
	}

	return ret;
}

static int LIBUSB_CALL usb_hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
	switch (event)
	{
	case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:	
		usb_hotplug_action(dev, ACTION_PLUG_IN);
		break;
	case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
		usb_hotplug_action(dev, ACTION_UNPLUG);
		break;
	default:break;
	}

	return 0;
}

static uint32_t init_usb_listen()
{
	uint32_t ret = CRG_FAIL;
	
	if (libusb_init(NULL) != LIBUSB_SUCCESS)
	{
		write_log("%s init libusb%s%s", PRINT_USB_STR, PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}

	libusb_hotplug_callback_handle hp = 0;
	
	if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG) == 0)
	{
		goto _init_usb_listen_exit_;
	}

	if (libusb_hotplug_register_callback(NULL, (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT), LIBUSB_HOTPLUG_ENUMERATE, LIBUSB_HOTPLUG_MATCH_ANY,
		LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, usb_hotplug_callback, NULL, &hp) != LIBUSB_SUCCESS)
	{
		goto _init_usb_listen_exit_;
	}

	g_usb_listen_inited = true;
	ret = CRG_SUCCESS;

_init_usb_listen_exit_:
	if (ret != CRG_SUCCESS)
	{	
		write_log("%s init usb listen%s%s", PRINT_USB_STR, PRINT_POINT_STR, PRINT_FAIL_STR);
		g_usb_listen_inited = false;
		libusb_exit(NULL);
	}

	return ret;
}

static void destory_usb_listen()
{
	if (g_usb_listen_inited)
	{
		g_usb_listen_inited = false;
		libusb_exit(NULL);
	}
}

static void* listen_usb_work_thread(void *arg)
{
	write_log("listen usb work thread%s%s", PRINT_POINT_STR, PRINT_START_STR);
	int ret = 0;
	struct timeval tv;
	
	operator_thread_work_count(INCREASE_TYPE);

	while (is_thread_running(USB_LISTEN_PTHREAD)) {
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		ret = libusb_handle_events_timeout(NULL, &tv);
		if (ret < 0) write_log("%s listen usb event, error: %s%s%s", PRINT_USB_STR, libusb_error_name(ret), PRINT_POINT_STR, PRINT_FAIL_STR); 
		
	}

	operator_thread_work_count(DECREASE_TYPE);

	write_log("listen usb work thread%s%s", PRINT_POINT_STR, PRINT_END_STR);
	return (void*)NULL;
}

static uint32_t usb_plugin_action_process_work(usb_action_stru* usb_action, uint32_t action_index)
{
	if (usb_action == NULL || action_index >= strlen((const char*)usb_action->sz_action_list))
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	int rn = 0;
	char tmp_dev_id[MAX_BUFF_LEN] = {0};

	for (uint i = 0; i < usb_action->usb_dev_info.vec_dev_id.size(); i++)
	{
		rn += snprintf(tmp_dev_id + rn, sizeof(tmp_dev_id)-rn, "%s ", usb_action->usb_dev_info.vec_dev_id[i].c_str());
	}
	
	write_log("%s process new usb device plugin action, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, device id: %s, vid: %s, pid: %s%s%s", PRINT_USB_STR, 
		usb_action->usb_dev_info.u8_board_type, usb_action->usb_dev_info.sz_board_num, usb_action->usb_dev_info.u8_board_port_num,
		usb_action->usb_dev_info.u8_bus_num, usb_action->usb_dev_info.u8_addr_num, tmp_dev_id,
		usb_action->usb_dev_info.sz_vid, usb_action->usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_START_STR);

	ret = find_dev_data_index_from_list(&usb_action->usb_dev_info);
	if (ret == CRG_NOT_FOUND)
	{
		push_usb_dev_data_to_list(&usb_action->usb_dev_info);
		// set signal for new device
		if (memcmp(usb_action->usb_dev_info.sz_vid, APPLE_L_VID, strlen(APPLE_L_VID)) != 0)
			set_found_android_dev_mask();
		else
			set_found_ios_dev_mask();
		// send log
		crg_send_device_info_to_server(USB_DEVICE_TYPE, &usb_action->usb_dev_info);
	}
	else
	{
		// it's bug OR more dev id ?
		write_log("%s the new usb device has already exist in the current usb list, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, device id: %s, vid: %s, pid: %s%s%s", PRINT_USB_STR, 
			usb_action->usb_dev_info.u8_board_type, usb_action->usb_dev_info.sz_board_num, usb_action->usb_dev_info.u8_board_port_num,
			usb_action->usb_dev_info.u8_bus_num, usb_action->usb_dev_info.u8_addr_num, tmp_dev_id,
			usb_action->usb_dev_info.sz_vid, usb_action->usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_BUG_STR);
	}
	
	write_log("%s process new usb device plugin action, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, device id: %s, vid: %s, pid: %s%s%s", PRINT_USB_STR,
		usb_action->usb_dev_info.u8_board_type, usb_action->usb_dev_info.sz_board_num, usb_action->usb_dev_info.u8_board_port_num,
		usb_action->usb_dev_info.u8_bus_num, usb_action->usb_dev_info.u8_addr_num, tmp_dev_id,
		usb_action->usb_dev_info.sz_vid, usb_action->usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_END_STR);

	return ret;
}

static uint32_t usb_unplug_action_process_work(usb_action_stru* usb_action, uint32_t action_index, bool dev_exist)
{
	if (usb_action == NULL || action_index >= strlen((const char*)usb_action->sz_action_list))
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	uint32_t dev_type = UNKNOWN_DEVICE_TYPE;
	usb_dev_info_stru usb_dev_info;
	clear_usb_dev_data(&usb_dev_info);
	
	ret = get_usb_dev_data_from_list((uint8_t)(usb_action->sz_dev_path[0]-'0'), &usb_action->sz_dev_path[1], sizeof(usb_action->sz_dev_path-1), usb_action->sz_vid, usb_action->sz_sz_pid[action_index], &usb_dev_info);
	if (ret == CRG_SUCCESS)
	{	
		write_log("%s process usb device unplug action, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, vid: %s, pid: %s%s%s", PRINT_USB_STR,
			usb_dev_info.u8_board_type, usb_dev_info.sz_board_num, usb_dev_info.u8_board_port_num,
			usb_dev_info.u8_bus_num, usb_dev_info.u8_addr_num, 
			usb_dev_info.sz_vid, usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_START_STR);

		get_dev_type_data_from_list((uint8_t)(usb_action->sz_dev_path[0] - '0'), &usb_action->sz_dev_path[1], sizeof(usb_action->sz_dev_path) - 1, usb_action->sz_vid, usb_action->sz_sz_pid[action_index], dev_type);
		pull_dev_data_from_list((uint8_t)(usb_action->sz_dev_path[0] - '0'), &usb_action->sz_dev_path[1], sizeof(usb_action->sz_dev_path) - 1, usb_action->sz_vid, usb_action->sz_sz_pid[action_index]);

		if (!dev_exist)
		{
#if _POWER_CONTROL_TURN_ON_	
			write_log("%s usb device was removed, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, vid: %s, pid: %s%s%s", PRINT_USB_STR, 
				usb_dev_info.u8_board_type, usb_dev_info.sz_board_num, usb_dev_info.u8_board_port_num,
				usb_dev_info.u8_bus_num, usb_dev_info.u8_addr_num, usb_dev_info.sz_vid, usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_POWER_OFF_STR);
			// power off
			power_off_control(&usb_dev_info, PRINT_USB_STR);
			light_off_control(&usb_dev_info, PRINT_USB_STR);
#endif
		}
		else
		{
			write_log("%s usb device already exist, don't POWER OFF, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, vid: %s, pid: %s%s%s", PRINT_USB_STR,
				usb_dev_info.u8_board_type, usb_dev_info.sz_board_num, usb_dev_info.u8_board_port_num,
				usb_dev_info.u8_bus_num, usb_dev_info.u8_addr_num, usb_dev_info.sz_vid, usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_OK_STR);
		}

		if (dev_type != ANDROID_DEVICE_TYPE && dev_type != IOS_DEVICE_TYPE)
		{
			// send log
			crg_send_device_info_to_server(UNKNOWN_DEVICE_TYPE, &usb_dev_info);
		}
		
		write_log("%s process usb device unplug action, board type: %d, board num: %s, port: %d, bus num: %d, addr: %d, vid: %s, pid: %s%s%s", PRINT_USB_STR,
			usb_dev_info.u8_board_type, usb_dev_info.sz_board_num, usb_dev_info.u8_board_port_num,
			usb_dev_info.u8_bus_num, usb_dev_info.u8_addr_num,
			usb_dev_info.sz_vid, usb_dev_info.sz_pid, PRINT_POINT_STR, PRINT_END_STR);
	}
	else
	{
		write_log("%s process usb device unplug action, but the usb device has not exist in the current usb list, vid: %s, pid: %s, dev path: %s, err: %d%s%s",
			PRINT_USB_STR, usb_action->sz_vid, usb_action->sz_sz_pid[action_index], usb_action->sz_dev_path, ret, PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	clear_usb_dev_data(&usb_dev_info);

	return ret;
}

static uint32_t usb_action_process_work(vector<usb_action_stru>* vec_usb_action)
{
	if (vec_usb_action == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	for (uint32_t i = 0; i < vec_usb_action->size(); i++)
	{
		uint32_t action_count = strlen((const char*)(*vec_usb_action)[i].sz_action_list);
		if (action_count > 1)
		{
			for (uint32_t j = 0; j < action_count; j++)
			{
				if ((*vec_usb_action)[i].sz_action_list[j] == ACTION_UNPLUG)
				{
					if (j + 1 < action_count && (*vec_usb_action)[i].sz_action_list[j + 1] == ACTION_PLUG_IN)
					{
						// remove, don't poweroff
						ret = usb_unplug_action_process_work(&(*vec_usb_action)[i], j, true);
					}
					else
					{
						// remove
						ret = usb_unplug_action_process_work(&(*vec_usb_action)[i], j, false);
					}
				}
				else
				{
					// add
					ret = usb_plugin_action_process_work(&(*vec_usb_action)[i], j);
				}
			}
		}
		else if (action_count == 1)
		{
			if ((*vec_usb_action)[i].sz_action_list[0] == ACTION_PLUG_IN)
			{
				// add
				ret = usb_plugin_action_process_work(&(*vec_usb_action)[i], 0);
			}
			else
			{
				// remove
				ret = usb_unplug_action_process_work(&(*vec_usb_action)[i], 0, false);
			}
		}
		else
		{
			// action_count = 0, but data was exist, it's bug
			char tmp_pid[MAX_BUFF_LEN] = { 0 };

			if (strcmp((*vec_usb_action)[i].sz_sz_pid[1], "") != 0)
				snprintf(tmp_pid, sizeof(tmp_pid), "%s %s", (*vec_usb_action)[i].sz_sz_pid[0], (*vec_usb_action)[i].sz_sz_pid[1]);
			else
				snprintf(tmp_pid, sizeof(tmp_pid), "%s", (*vec_usb_action)[i].sz_sz_pid[0]);

			write_log("%s the usb action struct was exist, but action count was 0, vid: %s, pid: %s, port: %s%s%s", 
				PRINT_USB_STR, (*vec_usb_action)[i].sz_vid, tmp_pid, (*vec_usb_action)[i].sz_dev_path, PRINT_POINT_STR, PRINT_BUG_STR);
		}
	}

	return ret;
}

static void* usb_action_process_thread(void *arg)
{
	vector<usb_action_stru> vec_usb_action;
	write_log("process usb action thread%s%s", PRINT_POINT_STR, PRINT_START_STR);

	operator_thread_work_count(INCREASE_TYPE);
	while (is_thread_running(USB_PROCESS_PTHREAD))
	{
		pull_all_usb_action_data_from_list(vec_usb_action);
		if (vec_usb_action.size() > 0)
		{
			usb_action_process_work(&vec_usb_action);
			clear_usb_action(&vec_usb_action);
		}
		
		sleep(1);
	}
	operator_thread_work_count(DECREASE_TYPE);

	write_log("process usb action thread%s%s", PRINT_POINT_STR, PRINT_END_STR);
	return (void*)NULL;
}

static uint32_t create_listen_usb_thread()
{
	uint32_t ret = CRG_FAIL;
	
	int err = pthread_create(&g_pthread_pid_listen, NULL, listen_usb_work_thread, NULL);

	if (err == 0)
	{
		ret = CRG_SUCCESS;
	}
	else
	{
		write_log("%s create listen usb thread, err: %s%s%s", PRINT_USB_STR, strerror(err), PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	g_thread_init_listen = (ret == CRG_SUCCESS ? true : false);
	return ret;
}

static uint32_t create_usb_process_thread()
{
	uint32_t ret = CRG_FAIL;

	int err = pthread_create(&g_pthread_pid_process, NULL, usb_action_process_thread, NULL);

	if (err == 0)
	{
		ret = CRG_SUCCESS;
	}
	else
	{
		write_log("%s create process usb thread, err: %s%s%s", PRINT_USB_STR, strerror(err), PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	g_thread_init_process = (ret == CRG_SUCCESS ? true : false);
	return ret;
}

static uint32_t create_usb_work_thread()
{
	uint32_t ret = CRG_FAIL;
	if (create_usb_process_thread() == CRG_SUCCESS
		&& create_listen_usb_thread() == CRG_SUCCESS )
	{
		ret = CRG_SUCCESS;
	}

	return ret;
}

uint32_t run_usb_model()
{
	uint32_t ret = CRG_FAIL;
	write_log("run usb model%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if (create_usb_work_thread() == CRG_SUCCESS)
	{
		ret = CRG_SUCCESS;
		write_log("run usb model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	}
	else
		write_log("run usb model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);

	return ret;
}

uint32_t init_usb_model()
{
	uint32_t ret = CRG_FAIL;
	
	write_log("init usb model%s%s", PRINT_POINT_STR, PRINT_START_STR);

	if (init_thread_count_lock() == CRG_SUCCESS
		&& init_usb_action_data_lock() == CRG_SUCCESS 
		&& init_usb_listen() == CRG_SUCCESS)
	{
		write_log("init usb model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
		ret = CRG_SUCCESS;
	}
	else
	{
		destory_usb_listen();
		destory_usb_action_data_lock();
		destory_thread_count_lock();
		write_log("init usb model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	g_inited = (ret == CRG_SUCCESS ? true : false);

	return ret;
}

void set_usb_model_exit_status()
{
	set_thread_running_status(USB_LISTEN_PTHREAD, false);
	set_thread_running_status(USB_PROCESS_PTHREAD, false);
}

void wait_usb_model_exit()
{
	write_log("waiting usb model stop work%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if(g_thread_init_listen) pthread_join(g_pthread_pid_listen, NULL);
	if(g_thread_init_process) pthread_join(g_pthread_pid_process, NULL);
	g_thread_init_listen = false;
	g_thread_init_process = false;
	write_log("waiting usb model stop work%s%s", PRINT_POINT_STR, PRINT_END_STR);
}

uint32_t destory_usb_model()
{
	if (!g_inited)
		return CRG_SUCCESS;
	
	write_log("destory usb model%s%s", PRINT_POINT_STR, PRINT_START_STR);

	uint32_t ret = CRG_FAIL;

	destory_usb_listen();
	
	clear_usb_action_for_list();

	ret = destory_usb_action_data_lock();
	
	if (ret == CRG_SUCCESS)
		write_log("destory usb model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	else
		write_log("destory usb model%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);
	
	g_inited = false;
	return ret;
}

