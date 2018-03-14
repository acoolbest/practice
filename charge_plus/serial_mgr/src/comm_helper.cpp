/*
 * comm_helper.cpp
 *
 *  Created on: 2014-4-11
 *      Author: root
 */

#include <sys/epoll.h>
#include "comm_helper.h"

#define MAXEPOLLSIZE	10

pthread_mutex_t g_exit_mutex;

com_helper::com_helper() {
	strcpy(dev, "/dev/");
	baud_rate = 9600;
	rev_done = 0;
	transmission_mutex = PTHREAD_MUTEX_INITIALIZER;
	g_thread_running = true;
	g_thread_inited = false;
}

com_helper::~com_helper() {
	tcsetattr(fd, TCSANOW, &oldtio);
	if (fd > 0) close_com();
	pthread_mutex_destroy(&transmission_mutex);
	pthread_mutex_destroy(&g_exit_mutex);
}

int com_helper::close_com() {
    return close(fd)==-1?-1:1;
}

int com_helper::open_set_com() {

	struct termios newtio;

	fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		//perror("open ");
		write_log("[LOGERROR] open serial port : %s failed.\n", dev);
		return -1;
	} else
		//set to block;
		fcntl(fd, F_SETFL, 0);

	write_log("open serial port : %s success.\n", dev);

	tcgetattr(fd, &oldtio); //save current serial port settings

	bzero(&newtio, sizeof(newtio)); // clear struct for new port settings
	//configure the serial port;

	int speed_arr[] = { B115200, B38400, B19200, B9600, B4800, B2400, B1200,
		B300, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
	int name_arr[] = { 115200, 38400, 19200, 9600, 4800, 2400, 1200, 300,
		38400, 19200, 9600, 4800, 2400, 1200, 300, };
	int i;
	for (i = 0; i < (int)sizeof(speed_arr) / (int)sizeof(int); i++) {
		if (baud_rate == name_arr[i]) {
			cfsetispeed(&newtio, speed_arr[i]);
			cfsetospeed(&newtio, speed_arr[i]);
			i = -1;
			break;
		}
	}
	if (i != -1) {
		write_log("configure the serial port speed : %d failed.\n", baud_rate);
		return -1;
	}
	newtio.c_cflag |= CLOCAL | CREAD;
	/*8N1*/
	newtio.c_cflag &= ~CSIZE; /* Mask the character size bits */
	newtio.c_cflag |= CS8; /* Select 8 data bits */
	newtio.c_cflag &= ~PARENB;
	newtio.c_cflag |= CSTOPB;   //newtio.c_cflag &= ~CSTOPB;
	newtio.c_cflag &= ~CRTSCTS;//disable hardware flow control;
	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);/*raw input*/
	newtio.c_oflag &= ~OPOST; /*raw output*/
	tcflush(fd, TCIFLUSH);//clear input buffer
	newtio.c_cc[VTIME] = 100; /* inter-character timer unused */
	newtio.c_cc[VMIN] = 0; /* blocking read until 0 character arrives */
	tcsetattr(fd, TCSANOW, &newtio);
	/*
	   ioctl(fd,TIOCMGET,&status);
	   printf("the serial status is 0x%x\n",status);
	   status |=TIOCM_RTS;
	   ioctl(fd,TIOCMSET,&status);
	   status=0;
	   ioctl(fd,TIOCMGET,&status);
	   printf("the serial status now is 0x%x\n",status);
	   */
	return 1;
}

void com_helper::print_com_data_to_log(char *str, char *rev, uint16_t len)
{
	int i;
	char buffer[MAX_SERIAL_BUFFER_LENGHT] = { 0 };
	for (i = 0; i < len; i++) sprintf(buffer + i * 3, " %02X", rev[i]);
	write_log("%s %s %s", dev, str, buffer);
}

bool com_helper::recv_other_data(char * p_data, uint16_t len)
{
	print_com_data_to_log("rev[OTHER]:(Hex)", p_data, len);
	return true;
}

/* write the users command out the serial port */
int com_helper::send_cmd(char * str, uint32_t len)
{
	uint32_t try_times = 3;
	int result;

	rev_done = 0;

	print_com_data_to_log("send cmd to", str, len);

	while (try_times--) {
		result = write(fd, str, len);
		if (result < 0) {
			usleep(100 * 1000);
		}
		else {
			memset(write_cmd_save_buffer, 0, sizeof(write_cmd_save_buffer));
			memcpy(write_cmd_save_buffer, str, len);
			return CRG_SUCCESS;
		}
	}
	// write_log("send cmd to %s failed : %s\n", dev, buffer);
	return CRG_TIMEOUT;
}

int com_helper::read_data(char *str, uint32_t & len)
{
	uint32_t try_times = RESPONSE_TIMEOUT*1000;
	while (try_times--) {
		if (!rev_done) usleep(1000);
		else
		{
			rev_done = 0;
			memset(str, 0, len);
			memcpy(str, read_cmd_response_buffer, read_cmd_response_size);
			len = read_cmd_response_size;
			memset(read_cmd_response_buffer, 0, read_cmd_response_size);
			return CRG_SUCCESS;
		}
	}
	len = 0;
	return CRG_TIMEOUT;
}

bool com_helper::analysis_com_response(char * p_data, uint16_t len)
{
	if (len != 6)
		return false;

	bool bret = false;
	char query_cmd[6] = { 0x68, 0x02, 0x06, 0x00, 0x06, 0x16 };
	char query_ret_16port[6] = { 0x68, 0x02, 0x06, 0x01, 0x07, 0x16 };
	char query_ret_8port[6] = { 0x68, 0x02, 0x06, 0x02, 0x08, 0x16 };

	char ack_ok[6] = { 0x68, 0x02, 0x01, 0x00, 0x01, 0x16 };
	char ack_fail[6] = { 0x68, 0x02, 0x01, 0x01, 0x02, 0x16 };
	char no_support_ret[6] = { 0x68, 0x02, 0x01, 0x02, 0x03, 0x16 };

	if (memcmp(p_data, ack_ok, len) == 0)
	{
		print_com_data_to_log("rev[OK]:(Hex)", p_data, len);
		bret = true;
	}
	else if (memcmp(p_data, ack_fail, len) == 0 || memcmp(p_data, no_support_ret, len) == 0)
	{
		print_com_data_to_log("rev[ERR]:(Hex)", p_data, len);
		bret = true;
	}
	else
	{
		if (memcmp(write_cmd_save_buffer, query_cmd, 6) == 0)
		{
			if (memcmp(p_data, query_ret_16port, 6) == 0 || memcmp(p_data, query_ret_8port, 6) == 0)
			{
				print_com_data_to_log("rev[OK]:(Hex)", p_data, len);
				bret = true;
			}
		}
	}
	return bret;
}
#if 0
void *com_helper::com_read_thread_func(void * lparam) {

	com_helper *p_com_helper;
	//得到com_helper实例指针
	p_com_helper = (com_helper*) lparam;

	write_log("com_read_thread_func start in device fd %d.\n", p_com_helper->fd);
	int n, max_fd, len;
	fd_set input;
	max_fd = p_com_helper->fd + 1;
	/* Do the select */
	struct timeval timeout;
	//char buffer[256] = {0};
	//char * ptr = buffer;
	//char temp_buffer[256] = {0};
	//uint16_t chcount = 0, buffer_count = 0;

	char rev_data[MAX_SERIAL_BUFFER_LENGHT] = {0};
	char * p_rev_data = rev_data;
	uint16_t rev_data_len = 0;
	
	while (true) {
		if(g_exit.global_exit)
		{
			pthread_mutex_lock(&g_exit_mutex);
			g_exit_com++;
			pthread_mutex_unlock(&g_exit_mutex);
			write_log("exit com %s\n", p_com_helper->dev);
			break;
		}
		/* Initialize the timeout structure */
		timeout.tv_sec = 0;
		timeout.tv_usec = 30 * 1000; //10ms
		FD_ZERO(&input);
		FD_SET(p_com_helper->fd, &input);
		n = select(max_fd, &input, NULL, NULL, &timeout);	
		if (n < 0)
			write_log("%s select failed", p_com_helper->dev);
		else if (n == 0) {
			if(rev_data_len){//收到有效的数据

				if (p_com_helper->analysis_com_response(rev_data, rev_data_len))
				{
					memset(p_com_helper->read_cmd_response_buffer, 0, sizeof(p_com_helper->read_cmd_response_buffer));
					memcpy(p_com_helper->read_cmd_response_buffer, rev_data, rev_data_len);
					p_com_helper->read_cmd_response_size = rev_data_len;
					p_com_helper->rev_done = 1;
				}
				else
				{
					p_com_helper->recv_other_data(rev_data, rev_data_len);
				}
			}
			
			rev_data_len = 0;
			memset(rev_data,0,sizeof(rev_data));
			p_rev_data = rev_data;
			
			continue;
		} else {
			//printf ("data in...\n");
			ioctl(p_com_helper->fd, FIONREAD, &len);
			if (!len) {
				write_log("Communication closed by server\n");
				//return -1;
				break;
			}
			len = read(p_com_helper->fd, p_rev_data, len);
			p_rev_data += len;//循环保存
			rev_data_len += len;
		}
	}
	
	return 0;
}
#endif

#if 1 //change by zhzq @ 2016-7-21 19:33:12
void *com_helper::com_read_thread_func(void * lparam) {
	com_helper *p_com_helper;
	//得到com_helper实例指针
	p_com_helper = (com_helper*) lparam;

	write_log("com_read_thread_func start in device fd %d.\n", p_com_helper->fd);

	char rev_data[MAX_SERIAL_BUFFER_LENGHT] = {0};
	uint16_t rev_data_len = 0;
	
	int epollfd, nfds, n;
	struct epoll_event ev;
	struct epoll_event events[MAXEPOLLSIZE];
	epollfd = epoll_create(MAXEPOLLSIZE);
	ev.events = EPOLLIN;
    ev.data.fd = p_com_helper->fd;
	
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, p_com_helper->fd, &ev) < 0) { 
        fprintf(stderr, "epoll set insertion error: fd=%d\n", p_com_helper->fd); 
        return NULL; 
	}// else printf("put p_com_helper->fd into epoll listening OK!\n"); 
	while(p_com_helper->is_thread_running()) {
		/*等待有事件发生*/
        nfds = epoll_wait(epollfd, events, MAXEPOLLSIZE, 1000);
        if (nfds == -1) { 
            perror("epoll_wait"); 
            continue; 
        }
		/*处理所有事件*/
		for (n = 0; n < nfds; ++n) {
			if(events[n].events & EPOLLIN){
				ioctl(p_com_helper->fd, FIONREAD, &rev_data_len);//chage by zhzq @ 2016-7-23 13:39:39
				rev_data_len = read(p_com_helper->fd, rev_data, rev_data_len);
				if(rev_data_len)
				{
					if (p_com_helper->analysis_com_response(rev_data, rev_data_len))
					{
						memset(p_com_helper->read_cmd_response_buffer, 0, sizeof(p_com_helper->read_cmd_response_buffer));
						memcpy(p_com_helper->read_cmd_response_buffer, rev_data, rev_data_len);
						p_com_helper->read_cmd_response_size = rev_data_len;
						p_com_helper->rev_done = 1;
					}
				}
				rev_data_len = 0;
				memset(rev_data,0,sizeof(rev_data));
            }
        }
	}
	write_log("com_read_thread_func end in device fd %d.\n", p_com_helper->fd);
	return (void *)NULL;
}
#endif


uint32_t com_helper::init_com(char * device, int speed = 9600) {
	strcat(dev, device);
	this->baud_rate = speed;
	if (open_set_com() < 0)
		return -1;

	pthread_mutex_init(&transmission_mutex, NULL);
	pthread_mutex_init(&g_exit_mutex, NULL);
	
	uint32_t ret = CRG_FAIL;
	
	int err = pthread_create(&g_pthread_pid, NULL, com_read_thread_func, (void*) this);

	if (err != 0){
		write_log("[LOGERROR] CommRead work pthread create failed\n");
	}else{
		ret = CRG_SUCCESS;
	}
	g_thread_inited = (ret == CRG_SUCCESS ? true : false);
	return ret;
}
bool com_helper::is_thread_running()
{
	return g_thread_running;
}

void com_helper::set_thread_running_status(bool b_running)
{
	g_thread_running = b_running;
}

