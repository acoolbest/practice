package com.changchong.site.pay.dto;

import java.io.Serializable;
/**
 * 
 * @author LHL
 *
 */
public class LoadPageDto implements Serializable {
	
	private static final long serialVersionUID = 1L;
	private Integer portNumber;
	private Integer qrCodeType;//1微信扫描2其他扫描
	private Integer deviceType;//1安卓2IOS
	private String timeStr;
	private String authCode;//授权返回code用于获取openid和用户信息
	private String openId;//openId
	public String getAuthCode() {
		return authCode;
	}
	public void setAuthCode(String authCode) {
		this.authCode = authCode;
	}
	public String getTimeStr() {
		return timeStr;
	}
	public void setTimeStr(String timeStr) {
		this.timeStr = timeStr;
	}
	public Integer getPortNumber() {
		return portNumber;
	}
	public void setPortNumber(Integer portNumber) {
		this.portNumber = portNumber;
	}
	public Integer getQrCodeType() {
		return qrCodeType;
	}
	public void setQrCodeType(Integer qrCodeType) {
		this.qrCodeType = qrCodeType;
	}
	public Integer getDeviceType() {
		return deviceType;
	}
	public void setDeviceType(Integer deviceType) {
		this.deviceType = deviceType;
	}
	public String getOpenId() {
		return openId;
	}
	public void setOpendId(String openId) {
		this.openId = openId;
	}
}
