package com.changchong.site.app.service;


import java.util.Map;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.site.app.dto.ChangChongAppDto;

public interface ChangChongAppService {

	public ChangChongAppDto findAppVersion(Integer versionCode,Integer osType);
	
	public Integer saveChangChongApp(ChangChongAppDto changChongAppDto);
	
	public PageList<ChangChongAppDto> getList(PageProperty pp);
	
	public void updateChangChongApp(ChangChongAppDto changChongAppDto);
	
	public ChangChongAppDto findAppVersionByParam(Map<String, Object> param);
}
