package com.changchong.site.app.service;

import java.util.Map;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.site.app.dto.PayableConfigAuditDto;

public interface PayableConfigAuditService {
	public PageList<PayableConfigAuditDto> getAuditLogList(PageProperty pp);

	public Integer savePayableConfigAudit(PayableConfigAuditDto payableConfigAuditDto);

	public void updatePayableConfigAudit(PayableConfigAuditDto payableConfigAuditDto);
	
	public PayableConfigAuditDto getPayableConfigAuditDto(Map<String,Object> param);
	
}
