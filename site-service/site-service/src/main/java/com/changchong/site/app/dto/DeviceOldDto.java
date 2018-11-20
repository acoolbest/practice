package com.changchong.site.app.dto;

import java.io.Serializable;

/**
 * Created by luhuiling on 2017/6/23.
 */
public class DeviceOldDto implements Serializable{
	private Integer id;//主键
	private Integer siteId;//站点id
	private String deviceCode;//设备编码
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public Integer getSiteId() {
		return siteId;
	}
	public void setSiteId(Integer siteId) {
		this.siteId = siteId;
	}
	public String getDeviceCode() {
		return deviceCode;
	}
	public void setDeviceCode(String deviceCode) {
		this.deviceCode = deviceCode;
	}

}
