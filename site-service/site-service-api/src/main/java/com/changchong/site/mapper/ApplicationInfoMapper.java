package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.AppPublishData;
import com.changchong.site.model.ApplicationDownInfo;
import com.changchong.site.model.ApplicationInfo;

@Repository
public interface ApplicationInfoMapper {

	public List<ApplicationInfo> findAppList(Map<String, Object> param);
	
	public List<ApplicationInfo> findAppInstallList(Map<String, Object> param);
	
	public ApplicationInfo getAppInfo(Map<String, Object> param);

	Integer getPushStartTimeByAppId(Map<String, Object> param);

	public List<ApplicationDownInfo> findAppDownList();
	
	public AppPublishData getAppPublishData(Map<String, Object> param);
}
