package com.changchong.site.mapper;

import com.changchong.site.model.DeviceTrouble;

import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;


@Repository
public interface DeviceTroubleMapper {
	List<DeviceTrouble> getDeviceTroubleById(Map<String,Object> map);
	List<DeviceTrouble> getDeviceTroubleList(Integer type);
}
