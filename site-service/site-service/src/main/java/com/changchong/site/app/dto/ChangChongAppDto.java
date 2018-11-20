package com.changchong.site.app.dto;

import java.io.Serializable;
import java.util.Date;

public class ChangChongAppDto implements Serializable{
	
	private static final long serialVersionUID = 1L;
	private Integer id;
	private Float versionCode; //版本号
	private String versionName;//版本号（字符串类型）
	private String packageName;//包名
	private String updateDesc;//更新内容
	private String downUrl;//下载地址
	private Integer mustUpdate;//是否强制更新 0-强制更新  1-不强制更新
	private String osType; //系统类型 0-ios  1-安卓
	private String size;//包体大小 例：13.6M
	private Date createTime;
	
	
	public String getOsType() {
		return osType;
	}
	public void setOsType(String osType) {
		this.osType = osType;
	}
	public Integer getMustUpdate() {
		return mustUpdate;
	}
	public void setMustUpdate(Integer mustUpdate) {
		this.mustUpdate = mustUpdate;
	}
	public String getDownUrl() {
		return downUrl;
	}
	public void setDownUrl(String downUrl) {
		this.downUrl = downUrl;
	}
	public String getSize() {
		return size;
	}
	public void setSize(String size) {
		this.size = size;
	}
	public String getVersionName() {
		return versionName;
	}
	public void setVersionName(String versionName) {
		this.versionName = versionName;
	}
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	
	public Float getVersionCode() {
		return versionCode;
	}
	public void setVersionCode(Float versionCode) {
		this.versionCode = versionCode;
	}
	public String getPackageName() {
		return packageName;
	}
	public void setPackageName(String packageName) {
		this.packageName = packageName;
	}
	public String getUpdateDesc() {
		return updateDesc;
	}
	public void setUpdateDesc(String updateDesc) {
		this.updateDesc = updateDesc;
	}
	public Date getCreateTime() {
		return createTime;
	}
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	
	
	
}
