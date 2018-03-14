#include "app_transmission.h"
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "log_mgr/src/libsendlog.h"
#include "serial_mgr/src/com_transmission.h"

static socket_helper * gp_socket_helper[MAX_MOBILE_COUNT]={NULL};

socket_helper::socket_helper(uint16_t port_index, string device_path)
{
	port_ser = SERVER_PORT_START+port_index;
	port_mp = MOBILE_PORT_START+port_index;
	dev_path = device_path;
	server_ip = "127.0.0.1";
}

socket_helper::~socket_helper(){
	port_ser = 0;
	port_mp = 0;
	close(sockfd);
}

uint32_t socket_helper::deal_adb_cmd()
{
	uint32_t ret = CRG_SUCCESS;
	char adb_cmd[3][MAX_PATH] = {0};
	char current_path[MAX_PATH] = { 0 };
	string out_str = "";
	get_current_path(current_path, MAX_PATH);
	snprintf(adb_cmd[0], MAX_PATH, "%s/%s -s %s shell am broadcast -a changchong.action.NotifySocketServiceStart", current_path, ADB_PROCESS_NAME, dev_path.c_str());
	snprintf(adb_cmd[1], MAX_PATH, "%s/%s -s %s forward tcp:%d tcp:%d", current_path, ADB_PROCESS_NAME, dev_path.c_str(), port_ser, port_mp);
	snprintf(adb_cmd[2], MAX_PATH, "%s/%s -s %s shell am broadcast -a changchong.action.NotifySocketServiceStop", current_path, ADB_PROCESS_NAME, dev_path.c_str());

	for(int retry = 0;retry<3;retry++)
	{
		ret = CRG_SUCCESS;
		for(int i=0; i<3; i++) 
		{
			write_log("%s", adb_cmd[i]);
			if(run_cmd(adb_cmd[i], out_str) == CRG_SUCCESS)
			{
				vector<string> vec_ret_list;
				string_split(out_str, string("\n"), &vec_ret_list);
				if((*(vec_ret_list.end())).find(ADB_ERROR_DEVICE_NOT_FOUND) != string::npos)
				{
					write_log("%s deal adb cmd error!",PRINT_APP_TRANSMISSION_STR);
					ret = CRG_FAIL;
				}
				out_str.clear();
				vec_ret_list.clear();
				vector<string>(vec_ret_list).swap(vec_ret_list);
				
				if(ret == CRG_FAIL)
					break;
			}
			else
			{
				ret = CRG_FAIL;
				break;
			}
		}
		if(ret == CRG_FAIL) sleep(3);
		else break;
	}
	return ret;
}

uint32_t socket_helper::init_socket()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		write_log("%s init socket %s%s",PRINT_APP_TRANSMISSION_STR, PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_ser);
	inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);
	
	struct timeval tv;
	tv.tv_sec = 1;
    tv.tv_usec = 0;
    int err = 0;
	err = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	if(err != 0)
	{
		write_log("%s set socket write timeout, error:%s%s%s",PRINT_APP_TRANSMISSION_STR, strerror(err), PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}
    err = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if(err != 0)
	{
		write_log("%s set socket read timeout, error:%s%s%s",PRINT_APP_TRANSMISSION_STR, strerror(err), PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}
	
	write_log("%s init socket, set socket read&write %d seconds timeout%s%s",PRINT_APP_TRANSMISSION_STR, tv.tv_sec, PRINT_POINT_STR, PRINT_OK_STR);
	
	return CRG_SUCCESS;
}

uint32_t socket_helper::connect_socket()
{
	int flags = fcntl(sockfd, F_GETFL, 0);				//获取文件的flags值
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);			//设置成非阻塞模式
    
	int ret_connect = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret_connect == 0)
	{
		write_log("%s connect socket %s%s",PRINT_APP_TRANSMISSION_STR, PRINT_POINT_STR, PRINT_OK_STR);
		return CRG_SUCCESS;
	}
	else if(ret_connect < 0 && errno == EINPROGRESS)		//errno == EINPROGRESS表示正在建立链接
	{
		
		fd_set rset, wset;
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(sockfd, &rset);
		FD_SET(sockfd, &wset);

		time_t timeout = 10;          //(超时时间设置为10毫秒)
		struct timeval tv;
		tv.tv_sec = timeout / 1000; 
		tv.tv_usec = (timeout % 1000) * 1000;

		int ret_select = select(sockfd + 1, &rset, &wset, NULL, &tv);//监听套接的可读和可写条件
		if(ret_select < 0)
		{
			write_log("%s connect socket [select] error:%s%s%s", PRINT_APP_TRANSMISSION_STR, strerror(ret_select), PRINT_POINT_STR, PRINT_FAIL_STR);
			return CRG_FAIL;
        }
		else if(ret_select == 0)
		{
			write_log("%s connect socket [select] error:%s%s%s", PRINT_APP_TRANSMISSION_STR, strerror(ret_select),PRINT_POINT_STR, PRINT_TIMEOUT_STR);
			return CRG_FAIL;
		}
		if(FD_ISSET(sockfd, &rset) && FD_ISSET(sockfd, &wset)) //如果套接口及可写也可读，需要进一步判断
		{
			int error_code = 0;
			socklen_t len = sizeof(error_code);
			if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error_code, &len) < 0)//获取SO_ERROR属性选项，当然getsockopt也有可能错误返回
			{
				write_log("%s connect socket error:%s%s%s", PRINT_APP_TRANSMISSION_STR, strerror(error_code), PRINT_POINT_STR, PRINT_FAIL_STR);
				return CRG_FAIL;
			}
			if(error_code != 0)//如果error不为0， 则表示链接到此没有建立完成
			{
				write_log("%s connect socket error:%s%s%s", PRINT_APP_TRANSMISSION_STR, strerror(error_code), PRINT_POINT_STR, PRINT_FAIL_STR);
				return CRG_FAIL;
			}
			//如果error为0，则说明链接建立完成
        }
		if(FD_ISSET(sockfd, &wset) && !FD_ISSET(sockfd, &rset))  //如果套接口可写不可读,则链接完成
		{
			write_log("%s connect socket %s%s",PRINT_APP_TRANSMISSION_STR, PRINT_POINT_STR, PRINT_OK_STR);
			return CRG_SUCCESS;
		}
	}
	else
	{
		write_log("%s connect socket error:%s%s%s",PRINT_APP_TRANSMISSION_STR, strerror(ret_connect), PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}
	return CRG_FAIL;
}

uint32_t socket_helper::deal_socket_rw(string send_data, string & recv_data)
{
	if(!send_data.length())
	{
		write_log("%s send data to app[%s:%d] send_data is empty!", PRINT_APP_TRANSMISSION_STR);
	}
	
	int ret_send = send(sockfd, send_data.c_str(), send_data.length(), 0);

	if(ret_send <= 0)
	{
		write_log("%s send data to app[%s:%d]:error:%s %s%s", PRINT_APP_TRANSMISSION_STR, server_ip.c_str(), port_mp, strerror(ret_send), PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}
	else
	{
		write_log("%s send data to app[%s:%d]:%s %s%s", PRINT_APP_TRANSMISSION_STR, server_ip.c_str(), port_mp, send_data.c_str(), PRINT_POINT_STR, PRINT_OK_STR);
	}

	char recv_buf[MAX_LEN] = "";
	
	int ret_recv = recv(sockfd, recv_buf, MAX_LEN, 0);
	if(ret_recv <=0)
	{
		write_log("%s recv data from app[%s:%d] error:%s %s%s", PRINT_APP_TRANSMISSION_STR, server_ip.c_str(), port_mp, strerror(ret_recv), PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}
	else
	{
		recv_data = recv_buf;
		// 判断 recv_data 是否为回复数据
		write_log("%s recv data from app[%s:%d]:%s %s%s", PRINT_APP_TRANSMISSION_STR, server_ip.c_str(), port_mp, recv_buf, PRINT_POINT_STR, PRINT_OK_STR);
	}
	return CRG_SUCCESS;
}

static uint32_t send_data_to_app_by_usb(uint16_t port_index, string send_data, string & recv_data)
{
	if(gp_socket_helper[port_index] == NULL)
		return CRG_FAIL;
	return gp_socket_helper[port_index]->deal_socket_rw(send_data, recv_data);
}

uint32_t send_port_debug_state_by_usb(usb_dev_info_stru * p_usb_dev_info, uint8_t debug_state,uint16_t port_index)
{
#if _POWER_CONTROL_TURN_ON_
	char qr_code[20] = "";
	if(com_get_qr_code_by_usb_device_info(p_usb_dev_info, qr_code) == CRG_SUCCESS)
	{
		string send_data = crg_make_port_mobil_freecharge_debug_state_json(qr_code, debug_state);
		string recv_data = "";
		return send_data_to_app_by_usb(port_index, send_data, recv_data);
	}
	return CRG_FAIL;
#else
	return CRG_NO_SUPPORT;
#endif
}

uint32_t send_freecharge_result_by_usb(usb_dev_info_stru * p_usb_dev_info, uint8_t power_state, uint16_t port_index)
{
#if _POWER_CONTROL_TURN_ON_
	char qr_code[20] = "";
	if(com_get_qr_code_by_usb_device_info(p_usb_dev_info, qr_code) == CRG_SUCCESS)
	{
		string send_data = crg_make_freecharge_report_json(qr_code, power_state);
		string recv_data = "";
		return send_data_to_app_by_usb(port_index, send_data, recv_data);
	}
	return CRG_FAIL;
#else
	return CRG_NO_SUPPORT;
#endif
}

uint32_t init_app_transmission(uint16_t port_index, string device_path)
{	
	gp_socket_helper[port_index] = new socket_helper(port_index, device_path);

	if(gp_socket_helper[port_index]->deal_adb_cmd() != CRG_SUCCESS)
		return CRG_FAIL;
	if(gp_socket_helper[port_index]->init_socket() != CRG_SUCCESS)
		return CRG_FAIL;
	if(gp_socket_helper[port_index]->connect_socket() != CRG_SUCCESS)
		return CRG_FAIL;
	
	return CRG_SUCCESS;
}

uint32_t destory_app_transmission(uint16_t port_index)
{
	if(gp_socket_helper[port_index] != NULL)
		delete gp_socket_helper[port_index];
	gp_socket_helper[port_index] = NULL;
	return CRG_SUCCESS;
}

static void rev_pipe_signal(int signum)
{
	write_log("receive signal of exit is %d, but do nothing\n",signum);
}

static uint32_t init_pipe_signal()
{
	write_log("init pipe signal model%s%s", PRINT_POINT_STR, PRINT_START_STR);
	if(signal(SIGPIPE,rev_pipe_signal)<0)
	{
		write_log("init pipe signal model, errno: %d%s%s", errno, PRINT_POINT_STR, PRINT_FAIL_STR);
		return CRG_FAIL;
	}
	write_log("init pipe signal model%s%s", PRINT_POINT_STR, PRINT_OK_STR);
	return CRG_SUCCESS;
}

uint32_t init_app_transmission_model()
{
	uint32_t ret = CRG_FAIL;
	write_log("init app transmission model%s%s\n", PRINT_POINT_STR,PRINT_START_STR);
	//屏蔽 SIGPIPE
	ret = init_pipe_signal(); 
	if (ret == CRG_SUCCESS){
		write_log("init app_transmission model%s%s\n",PRINT_POINT_STR,PRINT_OK_STR);
	}else{
		write_log("init app_transmission model%s%s\n",PRINT_POINT_STR,PRINT_FAIL_STR);
	}
	return ret;
}

uint32_t destory_app_transmission_model()
{
	write_log("destory app_transmission model%s%s", PRINT_POINT_STR,PRINT_START_STR);
	for(uint32_t i=0;i<MAX_MOBILE_COUNT;i++){
 		if(gp_socket_helper[i] != NULL)
			delete gp_socket_helper[i];
	}
	write_log("destory app_transmission model%s%s\n",PRINT_POINT_STR,PRINT_OK_STR);
	
	return CRG_SUCCESS;
}