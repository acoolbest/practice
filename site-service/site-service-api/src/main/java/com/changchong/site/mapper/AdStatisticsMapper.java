package com.changchong.site.mapper;

import com.changchong.site.model.AdStatistics;
import org.springframework.stereotype.Repository;

@Repository
public interface AdStatisticsMapper {

    int deleteByPrimaryKey(Integer id);

    int insertSelective(AdStatistics record);

    AdStatistics selectByPrimaryKey(Integer id);

    int updateByPrimaryKeySelective(AdStatistics record);

    int updateByPrimaryKey(AdStatistics record);
}