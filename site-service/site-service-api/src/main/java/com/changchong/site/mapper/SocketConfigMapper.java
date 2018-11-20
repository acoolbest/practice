package com.changchong.site.mapper;


import com.changchong.site.model.SocketConfig;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface SocketConfigMapper {

    int deleteByPrimaryKey(Integer id);

    int insertSelective(SocketConfig record);

    int updateByPrimaryKeySelective(SocketConfig record);

    List<SocketConfig> selectAll();
}