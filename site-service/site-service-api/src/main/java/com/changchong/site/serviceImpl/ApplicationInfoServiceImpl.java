package com.changchong.site.serviceImpl;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import javax.annotation.Resource;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.google.gson.Gson;
import org.apache.commons.lang.StringUtils;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;

import com.changchong.site.app.dto.AppPublishDataDto;
import com.changchong.site.app.dto.ApplicationDownInfoDto;
import com.changchong.site.app.dto.ApplicationInfoDto;
import com.changchong.site.app.service.ApplicationInfoService;
import com.changchong.site.mapper.ApplicationInfoMapper;
import com.changchong.site.model.AppPublishData;
import com.changchong.site.model.ApplicationDownInfo;
import com.changchong.site.model.ApplicationInfo;

@Service
public class ApplicationInfoServiceImpl implements ApplicationInfoService {

	@Resource
	private ApplicationInfoMapper applicationInfoMapper;
	

	@Override
	public List<ApplicationInfoDto> findAppList(Map<String, Object> param) {
		List<ApplicationInfo> applicationInfos = applicationInfoMapper.findAppList(param);
		List<ApplicationInfoDto> applicationInfoDtos = new ArrayList<ApplicationInfoDto>();
		ApplicationInfoDto applicationInfoDto = null;

		if(applicationInfos.size() == 0){
			return applicationInfoDtos;
		}
		for(ApplicationInfo applicationInfo : applicationInfos){
			applicationInfoDto = new ApplicationInfoDto();
			BeanUtils.copyProperties(applicationInfo, applicationInfoDto);
			applicationInfoDtos.add(applicationInfoDto);
		}
		return applicationInfoDtos;
	}


	@Override
	public List<ApplicationInfoDto> findAppInstallList(Map<String, Object> param) {
		List<ApplicationInfo> applicationInfos = applicationInfoMapper.findAppInstallList(param);
		List<ApplicationInfoDto> applicationInfoDtos = new ArrayList<ApplicationInfoDto>();
		ApplicationInfoDto applicationInfoDto = null;
		
		for(ApplicationInfo applicationInfo : applicationInfos){
			applicationInfoDto = new ApplicationInfoDto();
			BeanUtils.copyProperties(applicationInfo, applicationInfoDto);
			applicationInfoDtos.add(applicationInfoDto);
		}
		return applicationInfoDtos;
	}


	@Override
	public ApplicationInfoDto getAppInfo(Map<String, Object> param) {
		ApplicationInfo applicationInfo = applicationInfoMapper.getAppInfo(param);
		
		ApplicationInfoDto applicationInfoDto = new ApplicationInfoDto();
		if(applicationInfo != null){
			BeanUtils.copyProperties(applicationInfo, applicationInfoDto);
		}
		return applicationInfoDto;
	}


	@Override
	public List<ApplicationDownInfoDto> findAppDownList() {
		List<ApplicationDownInfo> applicationDownInfos = applicationInfoMapper.findAppDownList();
		List<ApplicationDownInfoDto> applicationDownInfoDtos = new ArrayList<ApplicationDownInfoDto>();
		
		for(ApplicationDownInfo applicationDownInfo : applicationDownInfos){
			ApplicationDownInfoDto applicationDownInfoDto = new ApplicationDownInfoDto();

			BeanUtils.copyProperties(applicationDownInfo, applicationDownInfoDto);

			applicationDownInfoDto.setMd5Icon(getIconMd5(applicationDownInfo.getIcon()));

			applicationDownInfoDtos.add(applicationDownInfoDto);
		}
		return applicationDownInfoDtos;
	}

	private String getIconMd5(String iconUrl) {
		if (StringUtils.isEmpty(iconUrl)) {
			return null;
		}
		if (!iconUrl.contains("/")) {
			return null;
		}
		String fileName = iconUrl.substring(iconUrl.lastIndexOf("/") + 1);

		return fileName.contains(".")?fileName.substring(0,fileName.lastIndexOf(".")):fileName;

	}

	@Override
	public AppPublishDataDto getAppPublishData(Map<String, Object> param) {
		AppPublishData appPublishData = applicationInfoMapper.getAppPublishData(param);
		AppPublishDataDto appPublishDataDto = new AppPublishDataDto();
		if(appPublishData != null){
			BeanUtils.copyProperties(appPublishData, appPublishDataDto);
		}
		return appPublishDataDto;
	}

	@Override
	public Integer getPushStartTimeByAppId(Map<String, Object> param) {
		return applicationInfoMapper.getPushStartTimeByAppId(param);
	}

}
