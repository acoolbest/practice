#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define			CRG_FAIL							0x0000
#define			CRG_SUCCESS							0x0001
#define			CRG_INVALID_PARAM					0x0002

#define         MAX_COM_COUNT                       6

/////////////////////////////////////////////////////////////////


#define		MAX_SERIAL_BUFFER_LENGHT		0xff

#define		COM_CMD_GET_DEVICE_INFO			0x51
#define		COM_CMD_APP_INSTALL_FAIL		0x57
#define		COM_CMD_APP_INSTALL_SUCCESS		0x58
#define		COM_CMD_POWER_OFF				0x59
#define		COM_CMD_POWER_ON				0x5A
#define		COM_CMD_HUB_RESET				0x10
#define		COM_CMD_FILE_OPERATIONS_REQ		0x11
#define		COM_CMD_FILE_TRANSFER			0x12
#define		COM_CMD_FILE_CALLED				0x13
#define		COM_CMD_FILE_ERASE				0x14
#define		COM_CMD_HANDSHAKE				0x16
#define		COM_CMD_ABNORMAL_FEEDBACK		0x1E

typedef struct _COM_CMD_STRUCT_
{
	uint8_t cmd_type;
	uint8_t dst_addr;
	uint8_t port_num;
	uint16_t power_on_time;
	
	uint8_t file_operation_type;
	uint8_t file_area;
	uint8_t file_name;
	uint32_t file_size;

	uint8_t transfer_flag;
	uint8_t section_num;
	uint8_t * p_data;
	uint8_t data_len;
}com_cmd_stru, *p_com_cmd_stru;

enum com_file_operation_type
{
	write_request = 1
};
enum com_file_area
{
	qr_code_area = 1,
	font_area,
	bmp_pic_area
};

uint8_t get_checksum(uint8_t * p_frame, uint8_t frame_len)
{
	uint8_t sum = 0;
	for (int i = 1; i < frame_len - 2; i++) sum += p_frame[i];
	return sum;
}

static uint8_t sprintf_com_cmd(uint8_t * sz_cmd, com_cmd_stru com_cmd_info)
{
	uint8_t * p = sz_cmd;
	*p = 0x67;
	*(p += 2) = com_cmd_info.dst_addr;
	*(++p) = 0xF0;
	*(++p) = com_cmd_info.cmd_type;
	switch (com_cmd_info.cmd_type)
	{
		case COM_CMD_APP_INSTALL_FAIL:
		case COM_CMD_APP_INSTALL_SUCCESS:
		case COM_CMD_HUB_RESET:
		case COM_CMD_POWER_OFF:
			*(++p) = com_cmd_info.port_num;
			break;
		case COM_CMD_POWER_ON:
			*(++p) = com_cmd_info.port_num;
			*(++p) = (uint8_t)(com_cmd_info.power_on_time >> 8);
			*(++p) = (uint8_t)(com_cmd_info.power_on_time);
			break;
		case COM_CMD_FILE_OPERATIONS_REQ:
			*(++p) = com_cmd_info.file_operation_type;
			*(++p) = com_cmd_info.file_area;
			*(++p) = com_cmd_info.file_name;
			*(++p) = (uint8_t)(com_cmd_info.file_size >> 24);
			*(++p) = (uint8_t)(com_cmd_info.file_size >> 16);
			*(++p) = (uint8_t)(com_cmd_info.file_size >> 8);
			*(++p) = (uint8_t)(com_cmd_info.file_size);
			break;
		case COM_CMD_FILE_TRANSFER:
			*(++p) = com_cmd_info.transfer_flag;
			*(++p) = com_cmd_info.section_num;
			memcpy(p+1, com_cmd_info.p_data, com_cmd_info.data_len);
			p += com_cmd_info.data_len;
			break;
		case COM_CMD_FILE_CALLED:
			*(++p) = com_cmd_info.port_num;
			*(++p) = com_cmd_info.file_area;
			*(++p) = com_cmd_info.file_name;
			break;
		case COM_CMD_FILE_ERASE:
			*(++p) = com_cmd_info.file_area;
			break;
		default:
			//COM_CMD_GET_DEVICE_INFO
			//COM_CMD_HANDSHAKE
			break;
	}
	*(p += 2) = 0x99;
	sz_cmd[1] = p - sz_cmd + 1;
	*(p - 1) = get_checksum((uint8_t *)sz_cmd, sz_cmd[1]);
	
	return sz_cmd[1];
}

static uint32_t get_com_num_from_ini(uint8_t & u8_com_num)
{
	u8_com_num = 0;
	
	return u8_com_num < MAX_COM_COUNT ? CRG_SUCCESS:CRG_FAIL;
}

static uint32_t send_com_cmd(uint8_t * p_data, uint8_t data_len)
{
	uint32_t dw_ret = CRG_FAIL;
	uint8_t u8_com_port_index = 0;
	#if 0
	if (get_com_num_from_ini(u8_com_port_index) != CRG_SUCCESS)
	{
		write_log("get connect com port num failed. please check board config file%s%s", PRINT_POINT_STR, PRINT_NOTFOUND_STR);
		return CRG_FAIL;
	}
	enter_transmission_lock(u8_com_port_index);
	gp_serial_port[u8_com_port_index]->send_cmd((char *)p_data, data_len);
	
	uint8_t recv_data[MAX_SERIAL_BUFFER_LENGHT] = {0};
	uint8_t recv_data_len = 0;
	dw_ret = gp_serial_port[u8_com_port_index]->read_data((char *)recv_data, recv_data_len);

	if (dw_ret == CRG_SUCCESS)
	{
		if(is_success_cmd_return((char *)recv_data, recv_data_len))
			dw_ret = CRG_SUCCESS;
		else
			dw_ret = CRG_FAIL;
	}
	leave_transmission_lock(u8_com_port_index);
	#endif
	return dw_ret;
}

static uint32_t com_deal_len8_cmd(uint8_t cmd_type, uint8_t dst_addr, uint8_t port_num)
{
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0};
	com_cmd_stru com_cmd_info = {
		.cmd_type = cmd_type,
		.dst_addr = dst_addr,
		.port_num = port_num
	};
	
	uint8_t frame_len = sprintf_com_cmd(sz_cmd, com_cmd_info);
	return send_com_cmd(sz_cmd, frame_len);
}

uint32_t com_get_device_info()
{
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0x67, 0x07, 0xFF, 0xF0, 0x51, 0x47, 0x99};
	return send_com_cmd(sz_cmd, 0x07);
}

uint32_t com_send_app_install_fail(uint8_t dst_addr, uint8_t port_num)
{
	return com_deal_len8_cmd(COM_CMD_APP_INSTALL_FAIL, dst_addr, port_num);
}

uint32_t com_send_app_install_success(uint8_t dst_addr, uint8_t port_num)
{
	return com_deal_len8_cmd(COM_CMD_APP_INSTALL_SUCCESS, dst_addr, port_num);
}

uint32_t com_send_power_off(uint8_t dst_addr, uint8_t port_num)
{
	return com_deal_len8_cmd(COM_CMD_POWER_OFF, dst_addr, port_num);
}

uint32_t com_send_hub_reset(uint8_t dst_addr, uint8_t port_num)
{
	return com_deal_len8_cmd(COM_CMD_HUB_RESET, dst_addr, port_num);
}

uint32_t com_send_power_on(uint8_t dst_addr, uint8_t port_num, uint16_t power_on_time)
{
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0};
	com_cmd_stru com_cmd_info = {
		.cmd_type = COM_CMD_POWER_ON,
		.dst_addr = dst_addr,
		.port_num = port_num,
		.power_on_time = power_on_time
	};
	
	uint8_t frame_len = sprintf_com_cmd(sz_cmd, com_cmd_info);
	return send_com_cmd(sz_cmd, frame_len);
}

static uint32_t com_send_file_operations_request(uint8_t dst_addr, uint8_t file_name, uint32_t file_size, com_file_operation_type file_operation_type = write_request, com_file_area file_area = qr_code_area)
{
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0};
	com_cmd_stru com_cmd_info = {
		.cmd_type = COM_CMD_FILE_OPERATIONS_REQ,
		.dst_addr = dst_addr,
		.file_operation_type = file_operation_type
		
	};
	//
	//	file_name:file_name,
	//	file_size:file_size
	//	
	//.file_operation_type = file_operation_type,
	//		.file_area = file_area,

	uint8_t frame_len = sprintf_com_cmd(sz_cmd, com_cmd_info);
	return send_com_cmd(sz_cmd, frame_len);
}

uint32_t com_send_file_transfer(uint8_t dst_addr, uint8_t * p_data, uint32_t data_len)
{
	uint32_t ret = CRG_FAIL;
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0};
	uint32_t remaining_len = data_len;
	uint8_t frame_len = 0;
	uint8_t try_times = 3;
	
	com_cmd_stru com_cmd_info;
	#if 0
	= {
		.cmd_type = COM_CMD_FILE_TRANSFER,
		.dst_addr = dst_addr,
		.transfer_flag = (data_len <= (0xFF - 0x09) ? 0xFF : 0x00),
		.section_num = 0,
		.p_data = p_data,
		.data_len = (data_len <= (0xFF - 0x09) ? data_len : (0xFF - 0x09))
	};
	#endif
	do{
		frame_len = sprintf_com_cmd(sz_cmd, com_cmd_info);
		try_times = 3;
		while(try_times--){
			if(send_com_cmd(sz_cmd, frame_len) != CRG_SUCCESS)
			{
				usleep(100*1000);
				ret = CRG_FAIL;
			}
			else 
			{
				ret = CRG_SUCCESS;
				remaining_len -= (frame_len - 0x09);
				com_cmd_info.transfer_flag = (remaining_len <= (0xFF - 0x09) ? 0xFF : 0x00);
				com_cmd_info.section_num++;
				com_cmd_info.p_data = p_data + (frame_len - 0x09);
				com_cmd_info.data_len = (remaining_len <= (0xFF - 0x09) ? remaining_len : (0xFF - 0x09));
				break;
			}
		}
	}while(remaining_len && (ret == CRG_SUCCESS));
	return ret;
}

uint32_t com_send_file_called(uint8_t dst_addr, uint8_t port_num, com_file_area file_area, uint8_t file_name)
{
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0};
	com_cmd_stru com_cmd_info;
	#if 0
	= {
		.cmd_type = COM_CMD_FILE_CALLED,
		.dst_addr = dst_addr,
		.file_area = file_area,
		.file_name = file_name
	};
	#endif
	uint8_t frame_len = sprintf_com_cmd(sz_cmd, com_cmd_info);
	return send_com_cmd(sz_cmd, frame_len);
}

uint32_t com_send_file_erase(uint8_t dst_addr, com_file_area file_area)
{
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0};
	com_cmd_stru com_cmd_info;
	#if 0
	= {
		.cmd_type = COM_CMD_FILE_ERASE,
		.dst_addr = dst_addr,
		.file_area = file_area
	};
	#endif
	uint8_t frame_len = sprintf_com_cmd(sz_cmd, com_cmd_info);
	return send_com_cmd(sz_cmd, frame_len);
}

uint32_t com_send_handshake(uint8_t dst_addr)
{
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0};
	com_cmd_stru com_cmd_info = {
		.cmd_type = COM_CMD_HANDSHAKE,
		.dst_addr = dst_addr
	};
	
	uint8_t frame_len = sprintf_com_cmd(sz_cmd, com_cmd_info);
	return send_com_cmd(sz_cmd, frame_len);
}

int main(int argc, char *argv[])
{
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0};
	for(int i=0;i<MAX_SERIAL_BUFFER_LENGHT;i++) sz_cmd[i] = i;
	com_send_file_transfer(0x10, sz_cmd, 0xFF - 0x09);
	for(int i=0;i<sz_cmd[1];i++)
	{
		printf("%02X ", sz_cmd[i]);
	}
	printf("\n");
	return 0;
}


