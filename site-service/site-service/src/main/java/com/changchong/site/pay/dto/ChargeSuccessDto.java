package com.changchong.site.pay.dto;

import java.io.Serializable;

public class ChargeSuccessDto implements Serializable{
	private static final long serialVersionUID = 4529859944742730789L;

	private String portCode;
	
	private String powerStat;//1、开始充电服务		2、断电

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
