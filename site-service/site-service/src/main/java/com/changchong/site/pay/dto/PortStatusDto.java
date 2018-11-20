package com.changchong.site.pay.dto;

import java.io.Serializable;

public class PortStatusDto implements Serializable {
	
	private static final long serialVersionUID = 1L;
	private String portCode;
	private String stat;
	private String serverPort;
	private String wifiSsid;
	private String wifiUsername;
	private String wifiPassword;
	private String chargeIP;
	private String chargePort;
	
	
	public String getWifiSsid() {
		return wifiSsid;
	}
	public void setWifiSsid(String wifiSsid) {
		this.wifiSsid = wifiSsid;
	}
	public String getWifiUsername() {
		return wifiUsername;
	}
	public void setWifiUsername(String wifiUsername) {
		this.wifiUsername = wifiUsername;
	}
	public String getWifiPassword() {
		return wifiPassword;
	}
	public void setWifiPassword(String wifiPassword) {
		this.wifiPassword = wifiPassword;
	}
	public String getChargeIP() {
		return chargeIP;
	}
	public void setChargeIP(String chargeIP) {
		this.chargeIP = chargeIP;
	}
	public String getChargePort() {
		return chargePort;
	}
	public void setChargePort(String chargePort) {
		this.chargePort = chargePort;
	}
	public String getPortCode() {
		return portCode;
	}
	public void setPortCode(String portCode) {
		this.portCode = portCode;
	}
	public String getStat() {
		return stat;
	}
	public void setStat(String stat) {
		this.stat = stat;
	}
	public String getServerPort() {
		return serverPort;
	}
	public void setServerPort(String serverPort) {
		this.serverPort = serverPort;
	}
	
	
}
