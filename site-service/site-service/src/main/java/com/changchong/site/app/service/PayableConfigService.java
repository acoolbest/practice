package com.changchong.site.app.service;

import java.util.List;
import java.util.Map;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.site.app.dto.PayableConfigDto;


public interface PayableConfigService {

	/**
	 * 获取充电加个列表
	 * @author lxg
	 * @param param
	 * @return
	 */
	List<PayableConfigDto> getPayableConfigList(Map<String, Object> param);

	Integer savePayableConfig(PayableConfigDto payableConfigDto);

	void updatePayableConfig(PayableConfigDto payableConfigDto);

	PageList<PayableConfigDto> getAuditLogList(PageProperty pp);

	List<PayableConfigDto> getEnablePayableConfigList(Map<String, Object> param);

}
