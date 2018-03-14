/* ************************************************************************
 *       Filename:  main.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年05月05日 09时27分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include "app_transmission.h"

using namespace std;

int main(int argc, char *argv[])
{
	init_app_transmission_model();
	
	init_app_transmission(uint16_t port_index, string device_path);
	
	send_port_debug_state_by_usb(usb_dev_info_stru * p_usb_dev_info, uint8_t debug_state,uint16_t port_index);
	
	send_freecharge_result_by_usb(usb_dev_info_stru * p_usb_dev_info, uint8_t power_state, uint16_t port_index);
	
	destory_app_transmission(uint16_t port_index);
	
	destory_app_transmission_model();

	return 0;
}


