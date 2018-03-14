/* ************************************************************************
 *       Filename:  test2.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2016年11月10日 11时33分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
	
	uint8_t transfer_flag;
	uint8_t section_num;
	uint8_t * p_data;
	uint8_t data_len;
	
	uint8_t file_operation_type;
	uint8_t file_area;
	uint8_t file_name;
	uint32_t file_size;
}com_cmd_stru, *p_com_cmd_stru;

static uint8_t sprintf_com_cmd(uint8_t * sz_cmd, com_cmd_stru com_cmd_info)
{
	uint8_t * p = sz_cmd;
	*p = 0x67;
	// frame_len
	*(p += 2) = dst_addr;
	*(++p) = 0xF0;
	*(++p) = cmd_type;
	switch (cmd_type)
	{
		case COM_CMD_FILE_OPERATIONS_REQ:
			*(++p) = ;
			*(++p) = 
			break;
		default:
			break;
	}
	*(p += 2) = 0x99;
	sz_cmd[1] = p - sz_cmd + 1;
	*(p - 1) = get_checksum((uint8_t *)sz_cmd, sz_cmd[1]);
	
	return sz_cmd[1];
}

uint32_t com_send_file_transfer(uint8_t dst_addr, uint8_t * p_data, uint32_t data_len)
{
	uint32_t ret = CRG_FAIL;
	uint8_t sz_cmd[MAX_SERIAL_BUFFER_LENGHT] = {0};
	uint32_t remaining_len = data_len;
	uint8_t frame_len = 0;
	uint8_t try_times = 3;
	com_cmd_stru com_cmd_info = {
		.cmd_type = COM_CMD_FILE_TRANSFER,
		.dst_addr = dst_addr,
		.transfer_flag = (data_len <= (0xFF - 0x09) ? 0xFF : 0x00),
		.section_num = 0,
		.p_data = p_data,
		.data_len = (data_len <= (0xFF - 0x09) ? data_len : (0xFF - 0x09)),
	};
	do
	{
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
				com_cmd_info.data_len = (remaining_len <= (0xFF - 0x09) ? remaining_len : (0xFF - 0x09)),
				break;
			}
		}
	}while(remaining_len && (ret == CRG_SUCCESS));
	return ret;
}

int main(int argc, char *argv[])
{
	com_send_file_transfer(uint8_t dst_addr, uint8_t * p_data, uint32_t data_len);
	return 0;
}


