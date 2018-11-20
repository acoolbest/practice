package com.changchong.site.service;

import com.changchong.site.app.dto.DeviceOldDto;

import com.changchong.site.app.dto.SiteInfoDto;

import com.changchong.site.app.dto.SiteDTO;


import java.util.List;

public interface SiteService {


	List<SiteInfoDto> getSiteInfoList(String ids);


	DeviceOldDto getDeviceInfoByCode(String deviceCode);

	void setPrices(List<String> ids, int siteId);

	SiteDTO findById(int siteId);


	SiteDTO findByPortNumber(int portNumber);


	SiteInfoDto findByPk(Integer id);

	/**
	 * 根据端口号查询站点信息
	 * @param portCode
	 * @return
	 */
	SiteInfoDto findByPortCode(Integer portCode);
}
