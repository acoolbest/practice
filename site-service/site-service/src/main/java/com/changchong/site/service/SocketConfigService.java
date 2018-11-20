package com.changchong.site.service;

import com.changchong.site.pay.dto.SocketConfigDto;

import java.util.List;

public interface SocketConfigService {
    List<SocketConfigDto> selectAll();
}
