package com.changchong.site.service;

import com.changchong.site.pay.dto.AdDto;
import com.changchong.site.pay.dto.AdStatisticsDto;

import java.lang.reflect.InvocationTargetException;
import java.util.List;

public interface AdService {
    /**
     * 查询可用广告ID列表
     * @param
     * @return
     */
    List<Integer> selectAvailableAdIds();

    List<AdDto> selectAdListByIds(List<Integer> ids) throws InvocationTargetException, IllegalAccessException;

    /**
     * 广告观看次数结果统计
     * @param adStatisticsDtoList
     */
    void saveAdCount(List<AdStatisticsDto> adStatisticsDtoList);

}
