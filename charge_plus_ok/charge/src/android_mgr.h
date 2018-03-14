#ifndef _ANDROID_MANAGER_H_
#define _ANDROID_MANAGER_H_

uint32_t init_android_model();
uint32_t destory_android_model();

uint32_t run_android_model();
void set_android_model_exit_status();
void wait_android_model_exit();

void set_found_android_dev_mask();

#endif // _ANDROID_MANAGER_H_
