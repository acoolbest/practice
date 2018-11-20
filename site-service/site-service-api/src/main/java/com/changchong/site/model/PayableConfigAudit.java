package com.changchong.site.model;

import java.math.BigDecimal;
import java.sql.Timestamp;

public class PayableConfigAudit {

	private Integer id;
	private Integer config_id; // 审核表主键
	private Byte status; // 审核状态 1-未审核 2-初审通过 3-初审未通过 4-复审通过 5-复审未通过
	private Integer opt_user_id; // 提交审核人
	private Timestamp create_time; // 创建时间
	private String type_name; // 类型名称
	private String user_name; // 操作员名称
	private String audit_result; // 审核状态
	private Integer chargeDuration; // 充电时长
	private BigDecimal payAmount; // 付费金额
	private Integer operator; // 提交者ID

	public Integer getId() {
		return id;
	}

	public void setId(Integer id) {
		this.id = id;
	}

	public Byte getStatus() {
		return status;
	}

	public void setStatus(Byte status) {
		this.status = status;
	}

	public Integer getConfig_id() {
		return config_id;
	}

	public void setConfig_id(Integer config_id) {
		this.config_id = config_id;
	}

	public Timestamp getCreate_time() {
		return create_time;
	}

	public void setCreate_time(Timestamp create_time) {
		this.create_time = create_time;
	}

	public Integer getOpt_user_id() {
		return opt_user_id;
	}

	public void setOpt_user_id(Integer opt_user_id) {
		this.opt_user_id = opt_user_id;
	}

	public String getType_name() {
		return type_name;
	}

	public void setType_name(String type_name) {
		this.type_name = type_name;
	}

	public String getUser_name() {
		return user_name;
	}

	public void setUser_name(String user_name) {
		this.user_name = user_name;
	}

	public String getAudit_result() {
		return audit_result;
	}

	public void setAudit_result(String audit_result) {
		this.audit_result = audit_result;
	}

	public Integer getChargeDuration() {
		return chargeDuration;
	}

	public void setChargeDuration(Integer chargeDuration) {
		this.chargeDuration = chargeDuration;
	}

	public BigDecimal getPayAmount() {
		return payAmount;
	}

	public void setPayAmount(BigDecimal payAmount) {
		this.payAmount = payAmount;
	}

	public Integer getOperator() {
		return operator;
	}

	public void setOperator(Integer operator) {
		this.operator = operator;
	}

}
