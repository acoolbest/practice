package com.changchong.site.pay.dto;

public class DeviceInfoResultDto extends DeviceInfoDto{

	private static final long serialVersionUID = 1L;
	private String CodeUrl;

	public String getCodeUrl() {
		return CodeUrl;
	}

	public void setCodeUrl(String codeUrl) {
		CodeUrl = codeUrl;
	}
}
