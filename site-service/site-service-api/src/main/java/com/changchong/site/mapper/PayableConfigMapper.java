package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.PayableConfig;

@Repository
public interface PayableConfigMapper {

	 List<PayableConfig> getPayableConfigList(Map<String, Object> param);

	 List<PayableConfig> getEnablePayableConfigList(Map<String, Object> param);

	 Integer savePayableConfig(PayableConfig payableConfig);
	
	 void updatePayableConfig(PayableConfig payableConfig);
	
	 int getCount(Map<String, Object> param);

	 List<PayableConfig> getAuditList(Map<String, Object> param);
}
