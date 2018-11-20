package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.PayableConfigAudit;

@Repository
public interface PayableConfigAuditMapper {
	public int getCount(Map<String, Object> param);

	public List<PayableConfigAudit> getAuditList(Map<String, Object> param);
	
	public Integer savePayableConfigAudit(PayableConfigAudit payableConfigAudit);
	
	public void updatePayableConfigAudit(PayableConfigAudit payableConfigAudit);
	
	public PayableConfigAudit getPayableConfigAuditDto(Map<String, Object> param);
}
