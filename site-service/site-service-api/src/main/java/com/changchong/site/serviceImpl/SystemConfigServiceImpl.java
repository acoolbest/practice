package com.changchong.site.serviceImpl;

import java.util.*;

import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.changchong.site.app.dto.SystemConfigDto;
import com.changchong.site.mapper.SystemConfigMapper;
import com.changchong.site.model.SystemConfig;
import com.changchong.site.service.SystemConfigService;


@Service(value="systemConfigService")
public class SystemConfigServiceImpl implements SystemConfigService{

	@Autowired
	private SystemConfigMapper systemConfigMapper;
	@Override
	public List<SystemConfigDto> getAllSystemConfig() {
		List<SystemConfigDto> systemConfigDtos = new ArrayList<SystemConfigDto>();
		List<SystemConfig> systemConfigs = systemConfigMapper.getAllSystemConfig();
		for(SystemConfig systemConfig:systemConfigs){
			SystemConfigDto systemConfigDto = new SystemConfigDto();
			BeanUtils.copyProperties(systemConfig, systemConfigDto);
			systemConfigDtos.add(systemConfigDto);
		}
		return systemConfigDtos;
	}

	@Override
	public void updateSystemConfig(String sys_key, String sys_value)
	{
		Map<String,Object> map  = new HashMap<String,Object>();
		map.put("key",sys_key);
		map.put("value",sys_value);
		map.put("updateTime",new Date());
		systemConfigMapper.updateSystemConfig(map);
	}

	@Override
	public SystemConfigDto getSystemConfigByKey(String sys_key)
	{
		Map<String,String> map  = new HashMap<String,String>();
		map.put("key",sys_key);
		List<SystemConfig> systemConfigs = systemConfigMapper.getSystemConfigByKey(map);
		if(systemConfigs!=null&&systemConfigs.size()==1){
			SystemConfigDto systemConfigDto = new SystemConfigDto();
			BeanUtils.copyProperties(systemConfigs.get(0), systemConfigDto);
			return systemConfigDto;
		}
		return null;
	}

}
