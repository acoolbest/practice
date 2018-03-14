#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <signal.h>
#include <iostream>
#include <pthread.h>
#include <fcntl.h>
#include "public_func/src/def.h"

using namespace std;

class socket_helper{
	public:
		socket_helper(uint16_t port_index, string device_path);
		~socket_helper();
		
		uint32_t deal_adb_cmd();
		uint32_t init_socket();
		uint32_t connect_socket();
		uint32_t deal_socket_rw(string send_data, string & recv_data);
	private:
		int sockfd;
		struct sockaddr_in server_addr;
		uint16_t port_ser;
		uint16_t port_mp;
		string server_ip;
		string dev_path;
};
uint32_t send_port_debug_state_by_usb(usb_dev_info_stru * p_usb_dev_info, uint8_t debug_state,uint16_t port_index);
uint32_t send_freecharge_result_by_usb(usb_dev_info_stru * p_usb_dev_info, uint8_t power_state, uint16_t port_index);
uint32_t init_app_transmission(uint16_t port_index, string device_path);
uint32_t destory_app_transmission(uint16_t port_index);
uint32_t init_app_transmission_model();
uint32_t destory_app_transmission_model();

