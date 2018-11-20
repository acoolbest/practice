package com.changchong.site.app.dto;

import java.io.Serializable;
import java.util.Date;


/**
 * Created by luhuiling on 2017/6/23.
 */
public class SystemConfigDto implements Serializable{
	private Integer id;
	private String key;//系统设置项标志
	private String value;//系统设置项具体内容
	private Date createTime;//创建时间
	private Date updateTime;//更新时间
	private String future;//扩展字段
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public String getKey() {
		return key;
	}
	public void setKey(String key) {
		this.key = key;
	}
	public String getValue() {
		return value;
	}
	public void setValue(String value) {
		this.value = value;
	}
	public Date getCreateTime() {
		return createTime;
	}
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	public Date getUpdateTime() {
		return updateTime;
	}
	public void setUpdateTime(Date updateTime) {
		this.updateTime = updateTime;
	}
	public String getFuture() {
		return future;
	}
	public void setFuture(String future) {
		this.future = future;
	}

}
