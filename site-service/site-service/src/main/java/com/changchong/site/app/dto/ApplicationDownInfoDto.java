package com.changchong.site.app.dto;

import java.io.Serializable;


/**
 * 
 * @author luhuiling
 * 工控机获取下载app
 * 2017年6月2日
 */
public class ApplicationDownInfoDto implements Serializable {

	private static final long serialVersionUID = 1L;
	private Integer appId;
	private String appName;
	private String appPkgName;
	private String fileName;
	private String md5;
	private String downUrl;
	private String version;
	private String appType;
	private String isAutoOpen;
	private String cmdLine;
	private String icon;
	private String md5Icon;
	private Integer timeSlot;//充电时间
	public Integer getAppId() {
		return appId;
	}
	public void setAppId(Integer appId) {
		this.appId = appId;
	}
	public String getAppName() {
		return appName;
	}
	public void setAppName(String appName) {
		this.appName = appName;
	}
	public String getAppPkgName() {
		return appPkgName;
	}
	public void setAppPkgName(String appPkgName) {
		this.appPkgName = appPkgName;
	}
	public String getFileName() {
		return fileName;
	}
	public void setFileName(String fileName) {
		this.fileName = fileName;
	}
	public String getMd5() {
		return md5;
	}
	public void setMd5(String md5) {
		this.md5 = md5;
	}
	public String getDownUrl() {
		return downUrl;
	}
	public void setDownUrl(String downUrl) {
		this.downUrl = downUrl;
	}
	public String getVersion() {
		return version;
	}
	public void setVersion(String version) {
		this.version = version;
	}
	public String getAppType() {
		return appType;
	}
	public void setAppType(String appType) {
		this.appType = appType;
	}
	public String getIsAutoOpen() {
		return isAutoOpen;
	}
	public void setIsAutoOpen(String isAutoOpen) {
		this.isAutoOpen = isAutoOpen;
	}
	public String getCmdLine() {
		return cmdLine;
	}
	public void setCmdLine(String cmdLine) {
		this.cmdLine = cmdLine;
	}
	public String getIcon() {
		return icon;
	}
	public void setIcon(String icon) {
		this.icon = icon;
	}

	public String getMd5Icon() {
		return md5Icon;
	}

	public void setMd5Icon(String md5Icon) {
		this.md5Icon = md5Icon;
	}

	public Integer getTimeSlot() {
		return timeSlot;
	}

	public void setTimeSlot(Integer timeSlot) {
		this.timeSlot = timeSlot;
	}
}
