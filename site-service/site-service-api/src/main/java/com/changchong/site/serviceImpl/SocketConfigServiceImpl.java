package com.changchong.site.serviceImpl;

import com.changchong.site.mapper.SocketConfigMapper;
import com.changchong.site.model.SocketConfig;
import com.changchong.site.pay.dto.SocketConfigDto;
import com.changchong.site.service.SocketConfigService;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

@Service("socketConfigService")
public class SocketConfigServiceImpl implements SocketConfigService {
    @Autowired
    private SocketConfigMapper socketConfigMapper;
    @Override
    public List<SocketConfigDto> selectAll() {
        List<SocketConfigDto> socketConfigDtoList = new ArrayList<>();
        List<SocketConfig> socketConfigList = socketConfigMapper.selectAll();
        for (SocketConfig socketConfig : socketConfigList) {
            SocketConfigDto socketConfigDto = new SocketConfigDto();
            BeanUtils.copyProperties(socketConfig,socketConfigDto);
            socketConfigDto.setMicroUrlAndPort(socketConfig.getMicroChargeUrl()+":"+socketConfig.getMicroChargePort());
            socketConfigDtoList.add(socketConfigDto);
        }
        return socketConfigDtoList;
    }
}
