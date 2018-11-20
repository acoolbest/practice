package com.changchong.site.mapper;


import com.changchong.site.model.DeviceTroubleDetail;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

@Repository
public interface DeviceTroubleDetailMapper {
    List<DeviceTroubleDetail> getDeviceTroubleDetailList(Map<String,Object> map);
    Integer getDeviceTroubleDetailListCount(Map<String,Object> map);
    Integer updateDeviceTrouble(DeviceTroubleDetail deviceTroubleDetail);
    Integer saveDeviceTroubleDetail(DeviceTroubleDetail deviceTroubleDetail);
}
