package com.changchong.site.service;



import com.changchong.site.app.dto.MarketConfigDto;

import java.util.List;
import java.util.Map;

public interface MarketConfigService
{
	//获取正在使用中的市场定价
	MarketConfigDto getUsedMarketConfig(Map<String, Object> param);

	//获取该用户提交中未使用的市场定价
	MarketConfigDto getUnUsedMarketConfig(Map<String, Object> param);

	void updateMarketConfig(List<MarketConfigDto> marketConfigDtoList);

	List<MarketConfigDto> saveMarketConfig(List<MarketConfigDto> marketConfigDtoList);

	MarketConfigDto findByIdOrCode(Map<String, Object> param);

}
