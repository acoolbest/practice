package com.changchong.site.app.dto;

import java.io.Serializable;
import java.sql.Timestamp;

/**
 * Created by lxg on 2017/4/12 15:32. 操作日志
 */
public class LogInfoRepo implements Serializable {

	private static final long serialVersionUID = 1L;
	private Integer id;
	private Integer user_id; // 用户账号
	private String phoneNumber;// 手机号
	private String email;// 邮箱地址
	private Timestamp create_time;// 创建时间
	private String ip;
	private String role;// 角色名称
	private String operation;// 操作
	private String type;// 用户类型 1-管理平台

	public Integer getId() {
		return id;
	}

	public void setId(Integer id) {
		this.id = id;
	}

	public Integer getUser_id() {
		return user_id;
	}

	public void setUser_id(Integer user_id) {
		this.user_id = user_id;
	}

	public String getPhoneNumber() {
		return phoneNumber;
	}

	public void setPhoneNumber(String phoneNumber) {
		this.phoneNumber = phoneNumber;
	}

	public String getEmail() {
		return email;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public Timestamp getCreate_time() {
		return create_time;
	}

	public void setCreate_time(Timestamp create_time) {
		this.create_time = create_time;
	}

	public String getIp() {
		return ip;
	}

	public void setIp(String ip) {
		this.ip = ip;
	}

	public String getRole() {
		return role;
	}

	public void setRole(String role) {
		this.role = role;
	}

	public String getOperation() {
		return operation;
	}

	public void setOperation(String operation) {
		this.operation = operation;
	}

	public String getType() {
		return type;
	}

	public void setType(String type) {
		this.type = type;
	}

}
