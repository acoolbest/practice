package com.changchong.site.serviceImpl;

import com.changchong.site.mapper.AdBaseMapper;
import com.changchong.site.mapper.AdStatisticsMapper;
import com.changchong.site.model.AdBase;
import com.changchong.site.model.AdStatistics;
import com.changchong.site.pay.dto.AdDto;
import com.changchong.site.pay.dto.AdStatisticsDto;
import com.changchong.site.service.AdService;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

@Service("adService")
public class AdServiceImpl implements AdService {
    @Autowired
    private AdBaseMapper adBaseMapper;
    @Autowired
    private AdStatisticsMapper adStatisticsMapper;
    @Override
    public List<Integer> selectAvailableAdIds() {
        return adBaseMapper.selectAvailableAdIds();
    }

    @Override
    public List<AdDto> selectAdListByIds(List<Integer> ids) throws InvocationTargetException, IllegalAccessException {
        List<AdDto> resultDtoList = new ArrayList<>();
        if (ids != null && ids.size() > 0) {
            for (Integer id : ids) {
                AdDto adDto = new AdDto();
                AdBase adBase = adBaseMapper.selectByPrimaryKey(id);
                if (adBase != null) {
                    BeanUtils.copyProperties(adBase,adDto);
                    resultDtoList.add(adDto);
                }

            }
        }
        return resultDtoList;
    }

    @Override
    public void saveAdCount(List<AdStatisticsDto> adStatisticsDtoList) {

        if (adStatisticsDtoList != null) {
            for (AdStatisticsDto adStatisticsDto : adStatisticsDtoList) {
                AdStatistics adStatistics = new AdStatistics();
                BeanUtils.copyProperties(adStatisticsDto, adStatistics);

                adStatisticsMapper.insertSelective(adStatistics);
            }

        }
    }

}
