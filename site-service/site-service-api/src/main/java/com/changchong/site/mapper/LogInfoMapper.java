package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.LogInfo;

@Repository
public interface LogInfoMapper {

	public Integer saveLogInfo(LogInfo logInfo);
	
	public List<LogInfo> getList(Map<String, Object> param);
	public List<LogInfo> getOperationLogAccount();
	public Integer getCount(Map<String, Object> param);
}
