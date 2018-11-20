package com.changchong.site.pay.dto;

import java.io.Serializable;
import java.util.List;

public class DeviceUploadDto implements Serializable {

	private static final long serialVersionUID = 1L;
	private String DeviceName;
	private List<PortStatusDto> PortList;
	public String getDeviceName() {
		return DeviceName;
	}
	public void setDeviceName(String deviceName) {
		DeviceName = deviceName;
	}
	public List<PortStatusDto> getPortList() {
		return PortList;
	}
	public void setPortList(List<PortStatusDto> portList) {
		PortList = portList;
	}

}
