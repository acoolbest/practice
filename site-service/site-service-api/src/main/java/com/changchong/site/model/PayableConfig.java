package com.changchong.site.model;

import java.math.BigDecimal;
import java.util.Date;

public class PayableConfig {

	private Integer id;
	private Integer chargeDuration; // 充电时长
	private BigDecimal originalPrice; // 原价
	private BigDecimal presentPrice;// 现价
	private Integer appType; // 类型 1-H5 2-APP
	private Integer optUserId;// 操作人
	private Integer status;// 审核状态 1-未审核 2-初审通过 3-初审未通过 4-复审通过 5-复审未通过 6-删除未审核 7-删除初审通过 8-删除初审未通过 9-删除复审通过 10-删除复审未通过
	private Date createTime;
	private String typeName; // 服务类型
	private String userName; // 操作员名称
	private String auditResult; // 审核状态
	private String is_default;

	public Integer getId() {
		return id;
	}

	public void setId(Integer id) {
		this.id = id;
	}

	public Integer getChargeDuration() {
		return chargeDuration;
	}

	public void setChargeDuration(Integer chargeDuration) {
		this.chargeDuration = chargeDuration;
	}

	public BigDecimal getOriginalPrice() {
		return originalPrice;
	}

	public void setOriginalPrice(BigDecimal originalPrice) {
		this.originalPrice = originalPrice;
	}

	public BigDecimal getPresentPrice() {
		return presentPrice;
	}

	public void setPresentPrice(BigDecimal presentPrice) {
		this.presentPrice = presentPrice;
	}

	public Integer getAppType() {
		return appType;
	}

	public void setAppType(Integer appType) {
		this.appType = appType;
	}

	public Integer getOptUserId() {
		return optUserId;
	}

	public void setOptUserId(Integer optUserId) {
		this.optUserId = optUserId;
	}

	public Integer getStatus() {
		return status;
	}

	public void setStatus(Integer status) {
		this.status = status;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public String getTypeName() {
		return typeName;
	}

	public void setTypeName(String typeName) {
		this.typeName = typeName;
	}

	public String getUserName() {
		return userName;
	}

	public void setUserName(String userName) {
		this.userName = userName;
	}

	public String getAuditResult() {
		return auditResult;
	}

	public void setAuditResult(String auditResult) {
		this.auditResult = auditResult;
	}

	public String getIs_default()
	{
		return is_default;
	}

	public void setIs_default(String is_default)
	{
		this.is_default = is_default;
	}
}
