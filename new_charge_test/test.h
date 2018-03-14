
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

uint32_t com_get_device_info();

uint32_t com_send_app_install_fail(uint8_t dst_addr, uint8_t port_num);

uint32_t com_send_app_install_success(uint8_t dst_addr, uint8_t port_num);

uint32_t com_send_power_off(uint8_t dst_addr, uint8_t port_num);

uint32_t com_send_hub_reset(uint8_t dst_addr, uint8_t port_num);

uint32_t com_send_power_on(uint8_t dst_addr, uint8_t port_num, uint16_t power_on_time);

uint32_t com_send_file_operations_request(uint8_t dst_addr, uint8_t file_name, uint32_t file_size, com_file_operation_type file_operation_type, com_file_area file_area);

uint32_t com_send_file_transfer(uint8_t dst_addr, uint8_t * p_data, uint32_t data_len);

uint32_t com_send_file_called(uint8_t dst_addr, uint8_t port_num, com_file_area file_area, uint8_t file_name);

uint32_t com_send_file_erase(uint8_t dst_addr, com_file_area file_area);

uint32_t com_send_handshake(uint8_t dst_addr);