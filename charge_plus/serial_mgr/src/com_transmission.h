#ifndef _COMDLL_TRANSMISSION_H_
#define _COMDLL_TRANSMISSION_H_
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "inifile.h"
#include "enum_board.h"
#include "comm_helper.h"

uint32_t init_com_transmission();
uint32_t destory_com_transmission();
void set_com_transmission_exit_status();
void wait_com_transmission_exit();

uint32_t control_mobile_client(uint8_t board_type, const char* board_num, uint8_t port_num, statu_control statu, uint8_t n_hight = 3, uint8_t n_low = 10);
uint32_t control_indicateor_light_client(uint8_t board_type, const char* board_num, uint8_t port_num, statu_control statu);
uint32_t check_board_hub();
void set_board_status(uint8_t fun);


#endif // _COMDLL_TRANSMISSION_H_
