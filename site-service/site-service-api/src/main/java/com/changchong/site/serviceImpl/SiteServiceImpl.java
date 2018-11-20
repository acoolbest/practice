package com.changchong.site.serviceImpl;


import com.changchong.site.app.dto.SiteInfoDto;
import com.changchong.site.model.SiteInfo;

import com.google.gson.Gson;


import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.changchong.site.app.dto.DeviceOldDto;
import com.changchong.site.app.dto.SiteDTO;
import com.changchong.site.app.util.BeanUtil;
import com.changchong.site.mapper.SiteMapper;
import com.changchong.site.model.DeviceOld;
import com.changchong.site.model.SitePo;
import com.changchong.site.service.SiteService;
import org.springframework.util.StringUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import java.util.HashMap;
import java.util.List;

@Service(value="siteService")
public class SiteServiceImpl implements SiteService{
	
	@Autowired
	private SiteMapper siteMapper;
	
	@Override
	public DeviceOldDto getDeviceInfoByCode(String deviceCode) {
		DeviceOldDto deviceOldDto = new DeviceOldDto();
		DeviceOld deviceOld = siteMapper.getDeviceInfoByCode(deviceCode);
		if(deviceOld!=null){
			BeanUtils.copyProperties(deviceOld, deviceOldDto);
		}
		return deviceOldDto;
	}

	@Override

	public List<SiteInfoDto> getSiteInfoList(String ids)
	{
		Map<String, Object> map = new HashMap<String, Object>();
		List<Integer> idList = new ArrayList<Integer>();
		if (org.apache.commons.lang.StringUtils.isNotBlank(ids))
		{
			for (String id : ids.split(","))
			{
				idList.add(Integer.parseInt(id));
			}
			map.put("ids", idList);
		}
		List<SiteInfo> siteInfoList = siteMapper.getSiteInfoList(map);
		List<SiteInfoDto> siteInfoDtoList = new ArrayList<SiteInfoDto>();
		if (siteInfoList == null)
		{
			return siteInfoDtoList;
		}
		for (SiteInfo siteInfo : siteInfoList)
		{
			SiteInfoDto siteInfoDto = new SiteInfoDto();
			BeanUtils.copyProperties(siteInfo, siteInfoDto);
			siteInfoDtoList.add(siteInfoDto);
		}
		return siteInfoDtoList;
	}
	public void setPrices(final List<String> ids, final int siteId) {
		siteMapper.updatePriceById(new HashMap(){{ put("payableCfgJson", new Gson().toJson(ids)); put("siteId", siteId);}})	;
	}

	@Override
	public SiteDTO findById(int siteId) {
		SitePo sitePo = siteMapper.findById(siteId);
		if(sitePo == null)
			return  null;
		SiteDTO siteDTO = new SiteDTO();
		BeanUtils.copyProperties(sitePo, siteDTO);
		return siteDTO;

	}

	@Override
	public SiteDTO findByPortNumber(int portNumber) {
		SitePo sitePo = siteMapper.findByPortNumber(portNumber);
		if(sitePo == null)
			return  null;
		SiteDTO siteDTO = new SiteDTO();
		BeanUtils.copyProperties(sitePo, siteDTO);
		return siteDTO;
	}

	@Override
	public SiteInfoDto findByPk(Integer id) {
		SiteInfo siteInfo = siteMapper.findByPk(id);
		if (siteInfo == null) {
			return null;
		}
		SiteInfoDto siteInfoDto = new SiteInfoDto();
		BeanUtils.copyProperties(siteInfo, siteInfoDto);
		return siteInfoDto;
	}

	@Override
	public SiteInfoDto findByPortCode(Integer portCode) {
		SiteInfo siteInfo = siteMapper.findByPortCode(portCode);
		if (siteInfo == null) {
			return null;
		}
		SiteInfoDto siteInfoDto = new SiteInfoDto();
		BeanUtils.copyProperties(siteInfo, siteInfoDto);
		return siteInfoDto;
	}

}
