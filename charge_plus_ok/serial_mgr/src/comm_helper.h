/*
 * CommHelper.h
 *
 *  Created on: 2014-4-11
 *      Author: root
 */

#ifndef COMMHELPER_H_
#define COMMHELPER_H_

#endif /* COMMHELPER_H_ */

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"

using namespace std;

/* These are the hash definitions */
#define USERBAUD1200 '1'+'2'
#define USERBAUD2400 '2'+'4'
#define USERBAUD9600 '9'+'6'
#define USERBAUD1920 '1'+'9'
#define USERBAUD3840 '3'+'8'
//#define DEV "/dev/ttyUSB1"


class com_helper {

	public:
		com_helper();
		~com_helper();
		
		char dev[15];
		int fd;
		int baud_rate;
		int rev_done;
		pthread_mutex_t transmission_mutex;
		bool g_thread_running;
		char read_cmd_response_buffer[MAX_SERIAL_BUFFER_LENGHT];
		uint16_t read_cmd_response_size;
		char write_cmd_save_buffer[MAX_SERIAL_BUFFER_LENGHT];
		pthread_t g_pthread_pid;
		bool g_thread_inited;

		int send_cmd(char * str , uint32_t len);
		int read_data(char * str, uint32_t & len);
		int open_set_com();
		uint32_t init_com(char * device , int speed);
		int close_com();
		bool analysis_com_response(char * p_data, uint16_t len);
		void print_com_data_to_log(char *str, char *rev, uint16_t len);
		bool recv_other_data(char * p_data, uint16_t len);
		bool is_thread_running();
		void set_thread_running_status(bool b_running);
	private:
		struct termios oldtio;
		static void *com_read_thread_func(void * lparam);

};

typedef map <int, com_helper*> com_map;



