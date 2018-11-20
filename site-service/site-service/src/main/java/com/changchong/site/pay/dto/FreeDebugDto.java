package com.changchong.site.pay.dto;

import java.io.Serializable;

public class FreeDebugDto implements Serializable {

	private static final long serialVersionUID = 1L;
	private String portCode;
	private String freeChargeDebugStat;
	public String getPortCode() {
		return portCode;
	}
	public void setPortCode(String portCode) {
		this.portCode = portCode;
	}
	public String getFreeChargeDebugStat() {
		return freeChargeDebugStat;
	}
	public void setFreeChargeDebugStat(String freeChargeDebugStat) {
		this.freeChargeDebugStat = freeChargeDebugStat;
	}
}
