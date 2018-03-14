#ifndef _LISTEN_USB_DEVICE_H_
#define _LISTEN_USB_DEVICE_H_

uint32_t init_usb_model();
uint32_t destory_usb_model();
uint32_t run_usb_model();
void set_usb_model_exit_status();
void wait_usb_model_exit();

#endif // _LISTEN_USB_DEVICE_H_
