package com.changchong.site.serviceImpl;

import com.changchong.global.util.encry.IdEncrypterForjava;
import com.changchong.sdk.constants.MarketEnum;
import com.changchong.site.app.dto.MarketConfigDto;
import com.changchong.site.mapper.MarketConfigMapper;
import com.changchong.site.model.MarketConfig;
import com.changchong.site.service.MarketConfigService;
import com.changchong.site.service.RedisService;
import com.google.gson.Gson;

import com.google.gson.reflect.TypeToken;
import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.util.*;

/**
 * Created by cm on 2017/8/7.
 */
@Service
public class MarketConfigServiceImpl implements MarketConfigService
{
	Logger log = Logger.getLogger(MarketConfigServiceImpl.class);
	@Resource
	private MarketConfigMapper marketConfigMapper;

	@Resource
	private RedisService redisService;

	@Override
	public MarketConfigDto getUsedMarketConfig(Map<String, Object> param){
		log.info("getUsedMarketConfig 传的参数为 =====================》》》》"+new Gson().toJson(param));
		//获取的列表是按照生效时间倒叙排列
		List<MarketConfigDto> MarketConfigDtoList = getMarketConfigList(param);
		log.info("getUsedMarketConfig 查询市场定价列表 =====================》》》》"+new Gson().toJson(MarketConfigDtoList));
		if(MarketConfigDtoList != null){
			for(MarketConfigDto marketConfigDto : MarketConfigDtoList){
				if(marketConfigDto.getEffect_time().getTime() <= new Date().getTime()){
					//如果生效时间在当前时间之前
					return marketConfigDto;
				}
			}
		}
		return null;
	}
	@Override
	public MarketConfigDto getUnUsedMarketConfig(Map<String, Object> param){
		log.info("getUnUsedMarketConfig 传的参数为 =====================》》》》"+new Gson().toJson(param));
		//获取的列表是按照生效时间倒叙排列
		List<MarketConfigDto> MarketConfigDtoList = getMarketConfigList(param);
		log.info("getUnUsedMarketConfig 查询市场定价列表 =====================》》》》"+new Gson().toJson(MarketConfigDtoList));
		if(MarketConfigDtoList != null){
			for(MarketConfigDto marketConfigDto : MarketConfigDtoList){
				if(marketConfigDto.getStatus() != MarketEnum.STATUS.TWO_ACCEPT.getValue()){
					return marketConfigDto;
				}
				if(marketConfigDto.getEffect_time().getTime() > new Date().getTime() && MarketEnum.STATUS.TWO_ACCEPT.getValue() == marketConfigDto.getStatus()){
					//如果生效时间在当前时间之后且是二审通过
					return marketConfigDto;
				}
			}
		}
		return null;
	}


	private  List<MarketConfigDto> getMarketConfigList(Map<String, Object> param) {
		String jsonParam = null;
		if(param == null){
			jsonParam = "";
		}else {
			jsonParam = new Gson().toJson(param);
		}
		String marketConfigJson = redisService.hget(MarketEnum.MARKET.LIST.getValue() , jsonParam);
		log.info("getMarketConfigList 传的参数为 =====================》》》》"+jsonParam);
		List<MarketConfigDto> result = new ArrayList<MarketConfigDto>();
		if(StringUtils.isNotBlank(marketConfigJson)&&!"[]".equals(marketConfigJson)){
			//如果redis中不等于空
			if(MarketEnum.MARKET.MULL.getValue().equals(marketConfigJson)){
				//如果数据库中本来就为空
				log.info("getMarketConfigList  =====================》》》》数据库中本来就为空");
				return null;
			}else{
				List<MarketConfigDto> marketConfigDtoList= new Gson().fromJson(marketConfigJson,new TypeToken<List<MarketConfigDto>>() {}.getType());
				log.info("getMarketConfigList  =====================从redis中获取》》》》"+marketConfigJson);
				return marketConfigDtoList;
			}
		}else{
			//如果redis中为空
			List<MarketConfig> marketConfigList = marketConfigMapper.getMarketConfigList(param);
			if(marketConfigList != null){
				for(MarketConfig marketConfig : marketConfigList){
					MarketConfigDto marketConfigDto = new MarketConfigDto();
					BeanUtils.copyProperties(marketConfig,marketConfigDto);
					result.add(marketConfigDto);
				}
				String listJson = new Gson().toJson(result);
				log.info("getMarketConfigList  =====================redis中为空，从数据库中查不为空》》》》"+listJson);
				//从数据库中查询出来后，保存到数据库中
				redisService.hset(MarketEnum.MARKET.LIST.getValue() , jsonParam,listJson);
				return result;
			}else{
				//数据库中本来就为空，设置redis为MULL
				log.info("getMarketConfigList  =====================redis中为空，从数据库中查为空》》》》");
				redisService.hset(MarketEnum.MARKET.LIST.getValue() , jsonParam,MarketEnum.MARKET.MULL.getValue());
				return null;
			}

		}
	}


	@Override
	@Transactional(propagation = Propagation.REQUIRED, rollbackFor = RuntimeException.class)
	public void updateMarketConfig(List<MarketConfigDto> marketConfigDtoList) {
		if(marketConfigDtoList != null && marketConfigDtoList.size() > 0){
			for(MarketConfigDto marketConfigDto : marketConfigDtoList){
				if(marketConfigDto != null){
					MarketConfig marketConfig = new MarketConfig();
					BeanUtils.copyProperties(marketConfigDto,marketConfig);
					marketConfigMapper.updateMarketConfig(marketConfig);
				}
			}
			redisService.del(MarketEnum.MARKET.LIST.getValue());
		}

	}

	@Override
	@Transactional(propagation = Propagation.REQUIRED, rollbackFor = RuntimeException.class)
	public List<MarketConfigDto> saveMarketConfig(List<MarketConfigDto> marketConfigDtoList) {
		List<MarketConfigDto> result = new ArrayList<MarketConfigDto>();
		if(marketConfigDtoList != null && marketConfigDtoList.size() > 0){
			for(MarketConfigDto marketConfigDto : marketConfigDtoList){
				if(marketConfigDto != null){
					MarketConfig marketConfig = new MarketConfig();
					BeanUtils.copyProperties(marketConfigDto,marketConfig);
					marketConfigMapper.saveMarketConfig(marketConfig);
					marketConfig.setMarket_code(IdEncrypterForjava.encodeVid(marketConfig.getId()));
					marketConfigMapper.updateMarketConfig(marketConfig);
					marketConfigDto.setId(marketConfig.getId());
					result.add(marketConfigDto);
				}
			}
			redisService.del(MarketEnum.MARKET.LIST.getValue());
		}
		return result;
	}

	@Override public MarketConfigDto findByIdOrCode(Map<String, Object> param)
	{
		List<MarketConfigDto> marketConfigDtoList = getMarketConfigList(param);
		if(marketConfigDtoList != null && marketConfigDtoList.size() == 1){
			return marketConfigDtoList.get(0);
		}
		return null;
	}

	//List<MarketConfigDto> marketConfigDtoList= new Gson().fromJson(marketConfigJson.toString(),List.class);转的对象有问题，需要将里面一层对象再转一遍
	private static List<MarketConfigDto> jsonListToNormalList(List<MarketConfigDto> marketConfigDtoList,List<MarketConfigDto> result){
		if(marketConfigDtoList != null&&result != null){
			Iterator marketConfigDtoIterator = marketConfigDtoList.iterator();
			while(marketConfigDtoIterator.hasNext()){
				String marketConfigJson = new Gson().toJson(marketConfigDtoIterator.next());
				MarketConfigDto jsonToMarketConfigDto = new Gson().fromJson(marketConfigJson,MarketConfigDto.class);
				result.add(jsonToMarketConfigDto);
			}
			return result;
		}
		return null;
	}
}
