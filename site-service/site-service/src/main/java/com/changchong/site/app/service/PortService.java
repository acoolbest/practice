package com.changchong.site.app.service;

import com.changchong.site.app.dto.PortDto;

import java.util.List;
import java.util.Map;

/**
 * Created by cm on 2017/5/22.
 */
public interface PortService {
    /**
     * 通过端口号获取信息
     * @param map
     * @return
     */
    PortDto getPortByCode(Map<String,Object> map);

    /**
     * 通过设备id查询所有信息
     * @param id
     * @return
     */
    List<PortDto> getPortByDeviceId(Integer id);

    /**
     * 通过设备唯一编号获取信息
     * @param id
     * @return
     */
    PortDto getPortById(Integer id);

    /**
     * 通过端口号判断数据库是否存在此端口号，并且拥有该端口的设备也存在 0不存在，1存在
     *
     * @param id
     * @return
     */
    Integer existenceById(Integer id);

}
