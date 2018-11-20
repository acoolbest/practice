package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.PayableConfigAuditLog;

@Repository
public interface PayableConfigAuditLogMapper {

	public List<PayableConfigAuditLog> getPayableConfigAuditLogList(Map<String, Object> param);
	
	public Integer savePayableConfigAuditLog(PayableConfigAuditLog payableConfigAuditLog);
	
	public void updatePayableConfigAuditLog(PayableConfigAuditLog payableConfigAuditLog);
}
