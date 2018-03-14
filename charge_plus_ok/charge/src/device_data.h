#ifndef _DATA_MANAGER_H_
#define _DATA_MANAGER_H_

uint32_t init_dev_data_model();
uint32_t destory_dev_data_model();

uint32_t find_dev_data_index_from_list(usb_dev_info_stru* in_usb_dev_info);
uint32_t get_usb_dev_data_from_list(uint8_t bus_num, char* port_num, uint32_t port_num_len, string vid, string pid, usb_dev_info_stru* out_usb_dev_info);
uint32_t get_dev_type_data_from_list(uint8_t bus_num, char* port_num, uint32_t port_num_len, string vid, string pid, uint32_t &out_dev_type);
uint32_t get_new_identify_dev_data_to_list(uint32_t dev_type, dev_all_info_stru* p_out_dev_all_info);
uint32_t pull_dev_data_from_list(uint8_t bus_num, char* port_num, uint32_t port_num_len, string vid, string pid);
uint32_t push_usb_dev_data_to_list(usb_dev_info_stru* usb_dev_info);
uint32_t push_identify_dev_data_to_list(uint32_t dev_type, void* identify_dev_info, usb_dev_info_stru* out_usb_dev_info);
uint32_t update_identify_dev_data_to_list(uint32_t dev_type, void* identify_dev_info);

uint32_t is_exist_identify_dev_data_from_list(uint32_t dev_type);
uint32_t is_exist_identify_dev_data_from_list_and_usb(uint32_t dev_type);

bool check_usb_position_compare(string src_dev_path, uint8_t dest_bus_num, uint8_t* dest_port_num, uint32_t dest_port_num_len);

uint32_t clear_identify_dev_data_from_list(uint32_t data_index);
uint32_t clear_dev_data(dev_all_info_stru* dev_all_info);
uint32_t clear_usb_dev_data(usb_dev_info_stru* usb_dev_info);

#endif // _DATA_MANAGER_H_
