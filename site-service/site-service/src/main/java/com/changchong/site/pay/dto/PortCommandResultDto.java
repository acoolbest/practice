package com.changchong.site.pay.dto;

import java.io.Serializable;

//端口上电指令反馈结果
public class PortCommandResultDto implements Serializable{

	private static final long serialVersionUID = 1L;
	private String Id;//指令id
	private Byte Result;
	private String ErrorCode;
	
	public String getId() {
		return Id;
	}
	public void setId(String id) {
		Id = id;
	}
	public Byte getResult() {
		return Result;
	}
	public void setResult(Byte result) {
		Result = result;
	}
	public String getErrorCode() {
		return ErrorCode;
	}
	public void setErrorCode(String errorCode) {
		ErrorCode = errorCode;
	}
}
