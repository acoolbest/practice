package com.changchong.site.pay.dto;

public class AppInstallInfoDto {

	private Integer app_id; //应用ID
	private Integer time_interval; //安装时长（s）
	public Integer getApp_id() {
		return app_id;
	}
	public void setApp_id(Integer app_id) {
		this.app_id = app_id;
	}
	public Integer getTime_interval() {
		return time_interval;
	}
	public void setTime_interval(Integer time_interval) {
		this.time_interval = time_interval;
	}
	
	
}
