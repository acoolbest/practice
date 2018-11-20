package com.changchong.site.pay.dto;

import java.io.Serializable;
import java.util.List;

public class AppFreeInstallDto implements Serializable{

	private static final long serialVersionUID = 1L;
	private String type;//type=1 未识别设备，type=2 安装失败，type=3 拒绝本机访问，type=4 接入设备
	private String port_code;//端口号
	private String device_id;//设备号
	private String vid;//设备生产厂商ID
	private String pid;//设备产品ID
	private String terminal;//安装设备名
	private String description;//描述
	private String time;//生成时间
	private String appSource;//appSource=1或空 推广渠道为畅充；appSource=2 推广渠道为xy助手
	private List<AppInstallInfoDto> appsData;
	
	
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public String getPort_code() {
		return port_code;
	}
	public void setPort_code(String port_code) {
		this.port_code = port_code;
	}
	public String getDevice_id() {
		return device_id;
	}
	public void setDevice_id(String device_id) {
		this.device_id = device_id;
	}
	public String getVid() {
		return vid;
	}
	public void setVid(String vid) {
		this.vid = vid;
	}
	public String getPid() {
		return pid;
	}
	public void setPid(String pid) {
		this.pid = pid;
	}
	public String getTerminal() {
		return terminal;
	}
	public void setTerminal(String terminal) {
		this.terminal = terminal;
	}
	public String getDescription() {
		return description;
	}
	public void setDescription(String description) {
		this.description = description;
	}
	public String getTime() {
		return time;
	}
	public void setTime(String time) {
		this.time = time;
	}
	public String getAppSource() {
		return appSource;
	}
	public void setAppSource(String appSource) {
		this.appSource = appSource;
	}
	public List<AppInstallInfoDto> getAppsData() {
		return appsData;
	}
	public void setAppsData(List<AppInstallInfoDto> appsData) {
		this.appsData = appsData;
	}
	
	
}
