package com.changchong.site.mapper;


import com.changchong.site.model.MarketConfig;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

@Repository
public interface MarketConfigMapper
{
    Integer saveMarketConfig(MarketConfig marketConfig);

    List<MarketConfig> getMarketConfigList(Map<String,Object> param);

    void updateMarketConfig(MarketConfig marketConfig);
}