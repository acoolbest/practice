package com.changchong.site.app.dto;

import java.io.Serializable;

public class AppDownInfoDto implements Serializable{

	private static final long serialVersionUID = 1L;
	private String version;
	private String type;
	private String province;
	private String terminal;
	public String getVersion() {
		return version;
	}
	public void setVersion(String version) {
		this.version = version;
	}
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public String getProvince() {
		return province;
	}
	public void setProvince(String province) {
		this.province = province;
	}
	public String getTerminal() {
		return terminal;
	}
	public void setTerminal(String terminal) {
		this.terminal = terminal;
	}

}
