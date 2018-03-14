#ifndef _POWER_CONTROL_H_
#define _POWER_CONTROL_H_

uint32_t init_power_model();
uint32_t destory_power_model();
uint32_t run_power_model();

uint32_t power_on_control(usb_dev_info_stru * p_usb_dev_info, char* p_device_type);
uint32_t power_off_control(usb_dev_info_stru * p_usb_dev_info, char* p_device_type);
uint32_t light_on_control(usb_dev_info_stru * p_usb_dev_info, char* p_device_type);
uint32_t light_off_control(usb_dev_info_stru * p_usb_dev_info, char* p_device_type);

void set_power_model_exit_status();
void wait_power_model_exit();

#endif // _POWER_CONTROL_H_
