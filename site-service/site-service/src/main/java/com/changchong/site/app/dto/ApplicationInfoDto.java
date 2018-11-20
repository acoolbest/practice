package com.changchong.site.app.dto;

import java.io.Serializable;
import java.util.Date;

/**
 * 
 * @author lxg
 *
 * 2017年5月20日上午9:55:50
 */
@SuppressWarnings("serial")
public class ApplicationInfoDto implements Serializable {

	
	private Integer id;
	private String appName; //应用名称
	private String appPackageName;//包名
	private String icon;//图标
	private Long size;//大小
	private String type;//类型
	private String appType;//应用类型 0-Android 1-IOS
	private Integer version;//应用版本
	private String versionName;
	private Integer timeSlot;//充电时间
	private String appSource;//应用来源
	private String appDescribe;//应用描述
	private String appPicture;//应用截图URL
	private Integer hotRanking;//热门排名
	private Date createTime;
	private String appKey;
	
	public String getAppKey() {
		return appKey;
	}
	public void setAppKey(String appKey) {
		this.appKey = appKey;
	}
	public String getAppType() {
		return appType;
	}
	public void setAppType(String appType) {
		this.appType = appType;
	}
	public Integer getHotRanking() {
		return hotRanking;
	}
	public void setHotRanking(Integer hotRanking) {
		this.hotRanking = hotRanking;
	}
	public String getAppDescribe() {
		return appDescribe;
	}
	public void setAppDescribe(String appDescribe) {
		this.appDescribe = appDescribe;
	}
	public String getAppPicture() {
		return appPicture;
	}
	public void setAppPicture(String appPicture) {
		this.appPicture = appPicture;
	}
	public Integer getTimeSlot() {
		return timeSlot;
	}
	public void setTimeSlot(Integer timeSlot) {
		this.timeSlot = timeSlot;
	}
	public Date getCreateTime() {
		return createTime;
	}
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public String getAppName() {
		return appName;
	}
	public void setAppName(String appName) {
		this.appName = appName;
	}
	public String getAppPackageName() {
		return appPackageName;
	}
	public void setAppPackageName(String appPackageName) {
		this.appPackageName = appPackageName;
	}
	public String getIcon() {
		return icon;
	}
	public void setIcon(String icon) {
		this.icon = icon;
	}
	public Long getSize() {
		return size;
	}
	public void setSize(Long size) {
		this.size = size;
	}
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public Integer getVersion() {
		return version;
	}
	public void setVersion(Integer version) {
		this.version = version;
	}

	public String getVersionName() {
		return versionName;
	}

	public void setVersionName(String versionName) {
		this.versionName = versionName;
	}

	public String getAppSource() {
		return appSource;
	}

	public void setAppSource(String appSource) {
		this.appSource = appSource;
	}
}
