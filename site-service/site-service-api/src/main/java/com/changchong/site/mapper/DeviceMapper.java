package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.Device;

@Repository
public interface DeviceMapper {
	public Device getDeviceByName(String name);
	public Integer insertDevice(Device device);
	public void deleteDevice(String name);//删除设备为逻辑删除并非物理删除将状态变为不可用
	public void updateDevice(Device device);//暂不需要
	public String finadDeviceByPort(Integer portNumber);
	public List<Device> getAllDevice();
	public List<Integer> selectId(Map<String, Object> param);
	Device getDeviceById(Integer id); //通过id查找单条记录
	/**
	 * 查询该设备号的设备是否存在
	 * @param deviceId
	 * @return
	 */
	Integer existenceByDeviceId(Integer deviceId);

}
