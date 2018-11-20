package com.changchong.site.model;

import java.io.Serializable;
import java.sql.Timestamp;

public class PayableConfigAuditLog implements Serializable {

	private static final long serialVersionUID = 1L;
	private Integer id;
	private Integer config_id; // 付费配置表主键
	private String audit_reason; // 审核结果
	private Byte status; // 审核状态 0-未审核1-提交审核 2-初审通过 3-初审未通过 4-复审通过 5-复审未通过 6-删除未审核 7-删除初审通过 8-删除初审未通过 9-删除复审通过 10-删除复审未通过
	private Integer opt_user_id; // 提交审核人
	private Timestamp create_time; // 创建时间
	private String description; // 操作描述
	private String auditResult; // 操作
	private String userName; // 操作员名称

	public Integer getId() {
		return id;
	}

	public void setId(Integer id) {
		this.id = id;
	}

	public Integer getConfig_id() {
		return config_id;
	}

	public void setConfig_id(Integer config_id) {
		this.config_id = config_id;
	}

	public String getAudit_reason() {
		return audit_reason;
	}

	public void setAudit_reason(String audit_reason) {
		this.audit_reason = audit_reason;
	}

	public Byte getStatus() {
		return status;
	}

	public void setStatus(Byte status) {
		this.status = status;
	}

	public Integer getOpt_user_id() {
		return opt_user_id;
	}

	public void setOpt_user_id(Integer opt_user_id) {
		this.opt_user_id = opt_user_id;
	}

	public Timestamp getCreate_time() {
		return create_time;
	}

	public void setCreate_time(Timestamp create_time) {
		this.create_time = create_time;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public String getAuditResult() {
		return auditResult;
	}

	public void setAuditResult(String auditResult) {
		this.auditResult = auditResult;
	}

	public String getUserName() {
		return userName;
	}

	public void setUserName(String userName) {
		this.userName = userName;
	}

}
