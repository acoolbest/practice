package com.changchong.site.app.dto;

import java.io.Serializable;

/**
 * 应用推广数据统计
 * @author lxg
 *
 * 2017年6月7日下午3:36:57
 */
public class AppPublishDataDto implements Serializable {
	private static final long serialVersionUID = 6366781917386254370L;
	
	private Integer appTotal; //应用总数
	private Integer publishNum;//推广数
	public Integer getAppTotal() {
		return appTotal;
	}
	public void setAppTotal(Integer appTotal) {
		this.appTotal = appTotal;
	}
	public Integer getPublishNum() {
		return publishNum;
	}
	public void setPublishNum(Integer publishNum) {
		this.publishNum = publishNum;
	}
	
	
}
