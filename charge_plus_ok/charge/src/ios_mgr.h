#ifndef _IOS_MANAGER_H_
#define _IOS_MANAGER_H_

uint32_t init_ios_model();
uint32_t destory_ios_model();

uint32_t run_ios_model();
void set_ios_model_exit_status();
void wait_ios_model_exit();

void set_found_ios_dev_mask();

#endif // _IOS_MANAGER_H_
