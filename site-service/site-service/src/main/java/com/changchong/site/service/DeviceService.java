package com.changchong.site.service;

import java.util.List;
import java.util.Map;

import com.changchong.site.pay.dto.*;

public interface DeviceService {
	public DeviceInfoResultDto insertDeviceInfo(DeviceInfoDto deviceInfoDto,Integer deviceId);//创建设备以及端口
	public DeviceInfoDto getDeviceByName(String deviceName);//根据设备名称查询设备及其端口信息
	public void updateDeviceInfo(DeviceInfoDto deviceInfoDto,Integer deviceId,List<PortCodeDto> devicePortCode);//更新设备信息，设备下端口信息
	public DeviceDto findDeviceByName(String code);//根据设备名称查询设备信息
	public String finadDeviceByPort(Integer portNumber);
	public List<DeviceDto> getAllDevice();
	/**
	 * 获取所有端口id
	 * @param param
	 * @return
	 */
	public List<Integer> selectId(Map<String, Object> param);

	DeviceDto getDeviceById(Integer id);


	/**
	 * 充电结束后把充电端口电流大小传输过来
	 * @param portElectricDto
	 */
	void savePortLog(PortElectricDto portElectricDto);
}
