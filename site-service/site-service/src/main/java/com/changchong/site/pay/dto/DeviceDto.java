package com.changchong.site.pay.dto;

import java.io.Serializable;

public class DeviceDto implements Serializable{

	private static final long serialVersionUID = 1L;
	private Integer id;
	private String code;
	private Byte status;
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public String getCode() {
		return code;
	}
	public void setCode(String code) {
		this.code = code;
	}
	public Byte getStatus() {
		return status;
	}
	public void setStatus(Byte status) {
		this.status = status;
	}

}
