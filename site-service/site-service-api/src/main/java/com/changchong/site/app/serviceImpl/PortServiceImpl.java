package com.changchong.site.app.serviceImpl;

import com.changchong.site.app.dto.PortDto;
import com.changchong.site.app.service.PortService;
import com.changchong.site.mapper.DeviceMapper;
import com.changchong.site.mapper.PortMapper;
import com.changchong.site.model.Port;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * Created by cm on 2017/5/22.
 */
@Service(value = "portService")
public class PortServiceImpl implements PortService {
    @Resource
    private PortMapper portMapper;
    @Resource
    private DeviceMapper deviceMapper;


    public PortDto getPortByCode(Map<String, Object> map) {
        Port port = portMapper.getPortByCode(map);
        if(port == null){
            return null;
        }
        PortDto portDto = new PortDto();
        BeanUtils.copyProperties(port,portDto);
        return portDto;
    }

    @Override
    public List<PortDto> getPortByDeviceId(Integer id) {
        List<Port> list = portMapper.getPortByDeviceId(id);
        if(list==null){
            return null;
        }
        List<PortDto> list1 = new ArrayList<PortDto>();
        for(Port port : list){
            PortDto portDto = new PortDto();
            BeanUtils.copyProperties(port,portDto);
            list1.add(portDto);
        }
        return list1;
    }

    @Override
    public PortDto getPortById(Integer id) {
        Port port = portMapper.getPortById(id);
        if(port==null){
            return null;
        }
        PortDto portDto = new PortDto();
        BeanUtils.copyProperties(port,portDto);
        return portDto;
    }
    @Override
    public Integer existenceById(Integer id) {
        Port port = portMapper.getPortById(id);
        if(port==null){
            return 0;
        }
        Integer deviceId = port.getDeviceId();
        Integer integer = deviceMapper.existenceByDeviceId(deviceId);
        if(integer==null){
            return 0;
        }
        return 1;
    }


}
