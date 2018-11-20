package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.SystemConfig;


@Repository
public interface SystemConfigMapper {
	public List<SystemConfig> getAllSystemConfig();
	void updateSystemConfig(Map<String,Object> param);
	List<SystemConfig> getSystemConfigByKey(Map<String,String> param);
}
