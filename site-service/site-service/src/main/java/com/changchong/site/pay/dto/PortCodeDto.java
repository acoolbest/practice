package com.changchong.site.pay.dto;

import java.io.Serializable;

public class PortCodeDto implements Serializable {

	private static final long serialVersionUID = 1L;
	
	private String Code;

	public String getCode() {
		return Code;
	}

	public void setCode(String code) {
		Code = code;
	}
}
