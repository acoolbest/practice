#ifndef _USB_ACTION_DATA_H_
#define _USB_ACTION_DATA_H_

#define									ACTION_PLUG_IN					0x01
#define									ACTION_UNPLUG					0x02

typedef struct USB_DEVICE_PLUGINOUT_ACTION_STRUCT
{
	void * p_dev;
	char sz_dev_path[MAX_PORT_PATH_LEN];
	char sz_vid[MAX_BUFF_LEN];
	char sz_sz_pid[MAX_BUFF_LEN][MAX_BUFF_LEN];
	uint8_t sz_action_list[MAX_BUFF_LEN];			// 1: plugin, 2: unplug
	usb_dev_info_stru usb_dev_info;
}usb_action_stru;

uint32_t init_usb_action_data_lock();
uint32_t destory_usb_action_data_lock();
uint32_t save_usb_action_data_to_list(void *dev, string dev_path, string vid, string pid, usb_dev_info_stru* p_usb_dev_info, uint8_t action);
uint32_t pull_all_usb_action_data_from_list(vector<usb_action_stru> & vec_usb_action);
uint32_t clear_usb_action_for_list();
uint32_t clear_usb_action(vector<usb_action_stru> * vec_usb_action);

#endif // _USB_ACTION_DATA_H_