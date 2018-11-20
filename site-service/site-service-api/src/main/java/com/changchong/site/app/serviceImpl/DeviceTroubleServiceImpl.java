package com.changchong.site.app.serviceImpl;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.global.page.PageUtil;
import com.changchong.site.app.dto.DeviceTroubleDetailDto;
import com.changchong.site.app.dto.DeviceTroubleDto;
import com.changchong.site.app.service.DeviceTroubleService;
import com.changchong.site.mapper.DeviceTroubleDetailMapper;
import com.changchong.site.mapper.DeviceTroubleMapper;
import com.changchong.site.model.DeviceTrouble;
import com.changchong.site.model.DeviceTroubleDetail;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by cm on 2017/5/22.
 */
@Service("deviceTroubleService")
public class DeviceTroubleServiceImpl implements DeviceTroubleService {
    @Resource
    private DeviceTroubleMapper deviceTroubleMapper;
    @Resource
    private DeviceTroubleDetailMapper deviceTroubleDetailMapper;

    @Override
    public PageList<DeviceTroubleDetailDto> getDeviceTroubleDetailList(PageProperty pageProperty) {
        String address = (String) pageProperty.getParamMap().get("address");
        String province = (String) pageProperty.getParamMap().get("province");
        String city = (String) pageProperty.getParamMap().get("city");
        pageProperty.putParamMap("likecondition","AND INSTR(CONCAT(IFNULL(t.province,''),IFNULL(t.city,''),IFNULL(t.address,''),IFNULL(t.phone,''),IFNULL(t.trouble_detail,'')), '"
                        +province+city+address+"')>0");
        int count = deviceTroubleDetailMapper.getDeviceTroubleDetailListCount(pageProperty.getParamMap());
        int start = PageUtil.getStart(pageProperty.getNpage(), count, pageProperty.getNpagesize());
        int end = pageProperty.getNpagesize();
        pageProperty.putParamMap("startRow", start);
        pageProperty.putParamMap("endRow", end);
        List<DeviceTroubleDetail> list = deviceTroubleDetailMapper.getDeviceTroubleDetailList(pageProperty.getParamMap());
        if (list == null) {
            return null;
        }
        List<DeviceTroubleDetailDto> list1 = new ArrayList<DeviceTroubleDetailDto>();
        for (DeviceTroubleDetail deviceTroubleDetail : list) {
            DeviceTroubleDetailDto deviceTroubleDetailDto = new DeviceTroubleDetailDto();
            BeanUtils.copyProperties(deviceTroubleDetail, deviceTroubleDetailDto);
            Map<String, Object> map = new HashMap<String, Object>();
            if (deviceTroubleDetail.getTrouble_id() == null || deviceTroubleDetail.getTrouble_id().equals("")) {
                deviceTroubleDetailDto.setTrouble_name("");
            } else {
                List<String> list2 = new ArrayList<String>();
                for (String id : deviceTroubleDetail.getTrouble_id().split(";")) {
                    list2.add(id);
                }
                map.put("ids", list2);
                List<DeviceTrouble> deviceTrouble = deviceTroubleMapper.getDeviceTroubleById(map);
                if (deviceTrouble == null) {
                    deviceTroubleDetailDto.setTrouble_name("");
                } else {
                    StringBuilder sb = new StringBuilder();
                    for (DeviceTrouble deviceTrouble1 : deviceTrouble) {
                        sb.append(deviceTrouble1.getName()).append(";");
                    }
                    deviceTroubleDetailDto.setTrouble_name(sb.toString());
                }

            }
            list1.add(deviceTroubleDetailDto);
        }
        PageList<DeviceTroubleDetailDto> result = new PageList<DeviceTroubleDetailDto>(pageProperty, count, list1);
        return result;
    }

    @Override
    public Integer updateDeviceTrouble(DeviceTroubleDetailDto deviceTroubleDetailDto) {
        DeviceTroubleDetail deviceTroubleDetail = new DeviceTroubleDetail();
        BeanUtils.copyProperties(deviceTroubleDetailDto, deviceTroubleDetail);
        return deviceTroubleDetailMapper.updateDeviceTrouble(deviceTroubleDetail);
    }

    @Override
    public List<DeviceTroubleDto> getDeviceTroubleList(Integer type) {
        List<DeviceTrouble> list = deviceTroubleMapper.getDeviceTroubleList(type);
        if (list == null) {
            return null;
        }
        List<DeviceTroubleDto> list1 = new ArrayList<DeviceTroubleDto>();
        for (DeviceTrouble deviceTrouble : list) {
            DeviceTroubleDto deviceTroubleDto = new DeviceTroubleDto();
            BeanUtils.copyProperties(deviceTrouble, deviceTroubleDto);
            list1.add(deviceTroubleDto);
        }
        return list1;
    }

    @Override
    public DeviceTroubleDto getDeviceTroubleById(Integer id) {
        Map<String, Object> map = new HashMap<String, Object>();
        List<Integer> list2 = new ArrayList<Integer>();
        list2.add(id);
        map.put("ids", list2);
        List<DeviceTrouble> deviceTrouble = deviceTroubleMapper.getDeviceTroubleById(map);
        if (deviceTrouble != null && deviceTrouble.size() > 0) {
            DeviceTroubleDto deviceTroubleDto = new DeviceTroubleDto();
            BeanUtils.copyProperties(deviceTrouble.get(0), deviceTroubleDto);
            return deviceTroubleDto;
        }
        return null;
    }

    @Override
    public Integer saveDeviceTroubleDetail(DeviceTroubleDetailDto deviceTroubleDetailDto) {
        DeviceTroubleDetail deviceTroubleDetail = new DeviceTroubleDetail();
        BeanUtils.copyProperties(deviceTroubleDetailDto, deviceTroubleDetail);
        return deviceTroubleDetailMapper.saveDeviceTroubleDetail(deviceTroubleDetail);
    }
}
