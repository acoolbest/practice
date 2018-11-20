package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.AppInstall;

@Repository
public interface AppInstallMapper {

	public List<AppInstall> findAppInstallList(Map<String, Object> param);
	
	public Integer save(AppInstall appInstall);
}
