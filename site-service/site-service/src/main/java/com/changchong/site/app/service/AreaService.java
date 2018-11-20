package com.changchong.site.app.service;

import com.changchong.site.app.dto.AreaDto;

import java.util.List;
import java.util.Map;

/**
 * Created by cm on 2017/5/19.
 */
public interface AreaService {
    /**
     * 获取所有省列表
     * @return
     */
    List<AreaDto> getProvinceList( );

    /**
     * 通过省获取所有市列表
     * @param id
     * @return
     */
    List<AreaDto> getCityListByProvince(Integer id);
}
