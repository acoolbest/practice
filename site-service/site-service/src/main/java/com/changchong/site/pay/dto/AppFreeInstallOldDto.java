package com.changchong.site.pay.dto;

import java.io.Serializable;

public class AppFreeInstallOldDto implements Serializable{

	private static final long serialVersionUID = 1L;
	private String portCode;
	private String powerStat;
	public String getPortCode() {
		return portCode;
	}
	public void setPortCode(String portCode) {
		this.portCode = portCode;
	}
	public String getPowerStat() {
		return powerStat;
	}
	public void setPowerStat(String powerStat) {
		this.powerStat = powerStat;
	}
}
