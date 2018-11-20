package com.changchong.site.mapper;

import com.changchong.site.model.PortLog;
import org.springframework.stereotype.Repository;

@Repository
public interface PortLogMapper {


    int deleteByPrimaryKey(Integer id);

    int insertSelective(PortLog record);

    PortLog selectByPrimaryKey(Integer id);

    int updateByPrimaryKeySelective(PortLog record);

    int updateByPrimaryKey(PortLog record);
}