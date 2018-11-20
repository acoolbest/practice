package com.changchong.site.app.service;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.site.app.dto.DeviceTroubleDetailDto;
import com.changchong.site.app.dto.DeviceTroubleDto;

import java.util.List;

/**
 * Created by cm on 2017/5/22.
 */
public interface DeviceTroubleService {
    /**
     * 获取设备故障详细列表
     * @param pageProperty
     * @return
     */
    PageList<DeviceTroubleDetailDto> getDeviceTroubleDetailList(PageProperty pageProperty);

    /**
     * 更新设备故障
     * @param deviceTroubleDetailDto
     * @return
     */
    Integer updateDeviceTrouble(DeviceTroubleDetailDto deviceTroubleDetailDto);

    /**
     * 获取设备故障列表
     * @return
     */
    List<DeviceTroubleDto> getDeviceTroubleList(Integer type);

    DeviceTroubleDto getDeviceTroubleById(Integer id);
    Integer saveDeviceTroubleDetail(DeviceTroubleDetailDto deviceTroubleDetailDto);
}
