package com.changchong.site.service;

import java.util.List;

import com.changchong.site.app.dto.SystemConfigDto;

public interface SystemConfigService {
	public List<SystemConfigDto> getAllSystemConfig();
	void updateSystemConfig(String sys_key,String sys_value);
	SystemConfigDto getSystemConfigByKey(String sys_key);
}
