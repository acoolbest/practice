package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.Port;

@Repository
public interface PortMapper {
	public Integer insertPort(Port port);
	public List<Port> getPortByDeviceId(Integer deviceId);
	public void deletePort(Port port);
	public void deletePortByDevice(Integer deviceId);
	public Port getPortById(Integer portId);
	public void deletePortFromDevice(Port port);
	public List<Integer> selectId(Map<String, Object> param);

	Port getPortByCode(Map<String, Object> param);
}
