package com.changchong.site.pay.dto;

import java.io.Serializable;
import java.util.List;

public class DeviceInfoDto implements Serializable {

	private static final long serialVersionUID = 1L;
	
	private String DeviceName;
	private String PortCount;
	private String province;
	private List<PortCodeDto> PortCode;
	public String getProvince() {
		return province;
	}
	public void setProvince(String province) {
		this.province = province;
	}
	public String getDeviceName() {
		return DeviceName;
	}
	public void setDeviceName(String deviceName) {
		DeviceName = deviceName;
	}
	public String getPortCount() {
		return PortCount;
	}
	public void setPortCount(String portCount) {
		PortCount = portCount;
	}
	public List<PortCodeDto> getPortCode() {
		return PortCode;
	}
	public void setPortCode(List<PortCodeDto> portCode) {
		PortCode = portCode;
	}
}
