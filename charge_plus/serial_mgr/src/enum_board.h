#ifndef _USB_CONTROL_H_
#define _USB_CONTROL_H_

uint32_t get_board_device_num_from_dev_path(uint8_t u8_board_type, string str_dev_id_path, string &str_board_num);
uint32_t get_current_board_device_list(vector<board_device_info>* pvecDeviceInfo);

#endif // _USB_CONTROL_H_
