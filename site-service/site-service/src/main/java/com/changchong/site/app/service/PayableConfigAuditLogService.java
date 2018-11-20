package com.changchong.site.app.service;

import java.util.List;
import java.util.Map;

import com.changchong.site.app.dto.PayableConfigAuditLogDto;

public interface PayableConfigAuditLogService {

	public List<PayableConfigAuditLogDto> getPayableConfigAuditLogList(Map<String, Object> param);

	public Integer savePayableConfigAuditLog(PayableConfigAuditLogDto payableConfigAuditLogDto);

	public void updatePayableConfigAuditLog(PayableConfigAuditLogDto payableConfigAuditLogDto);
}
