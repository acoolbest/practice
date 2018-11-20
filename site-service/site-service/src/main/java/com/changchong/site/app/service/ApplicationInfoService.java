package com.changchong.site.app.service;

import java.util.List;
import java.util.Map;

import com.changchong.site.app.dto.AppPublishDataDto;
import com.changchong.site.app.dto.ApplicationInfoDto;
import com.changchong.site.app.dto.ApplicationDownInfoDto;

public interface ApplicationInfoService {

	/**
	 * 获取应用列表
	 * @author lxg
	 * @param param
	 * @return
	 */
	public List<ApplicationInfoDto> findAppList(Map<String, Object> param);
	
	/**
	 * 获取设备已安装应用列表
	 * @author lxg
	 * @param param
	 * @return
	 */
	public List<ApplicationInfoDto> findAppInstallList(Map<String, Object> param);
	
	/**
	 * 根据应用id获取应用信息
	 * @author lxg
	 * @param param
	 * @return
	 */
	public ApplicationInfoDto getAppInfo(Map<String, Object> param);
	/**
	 * 工控机获取应用列表
	 * @author luhuiling
	 * @return
	 */
	public List<ApplicationDownInfoDto> findAppDownList();
	
	/**
	 * 应用推广数据统计
	 * @author lxg
	 * @param param
	 * @return
	 */
	public AppPublishDataDto getAppPublishData(Map<String, Object> param);

	/*
	获取应用推广最初开始时间
	 */
	Integer getPushStartTimeByAppId(Map<String, Object> param);
}
