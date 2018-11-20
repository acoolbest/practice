package com.changchong.site.model;

import java.util.Date;

public class OperationLog {
	public Integer id;
	private String openId;//openid
	private String userName;//用户昵称或者邮箱名
	private Date createTime;//创建时间
	private Byte operationType;//操作类型
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public String getOpenId() {
		return openId;
	}
	public void setOpenId(String openId) {
		this.openId = openId;
	}
	public String getUserName() {
		return userName;
	}
	public void setUserName(String userName) {
		this.userName = userName;
	}
	public Date getCreateTime() {
		return createTime;
	}
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	public Byte getOperationType() {
		return operationType;
	}
	public void setOperationType(Byte operationType) {
		this.operationType = operationType;
	}
}
