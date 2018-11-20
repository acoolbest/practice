package com.changchong.site.app.service;

import java.util.List;
import java.util.Map;

import com.changchong.site.app.dto.AppInstallDto;


public interface AppInstallService {

	public List<AppInstallDto> findAppInstallList(Map<String, Object> param);
	
	public Integer save(AppInstallDto appInstallDto);
}
