package com.changchong.site.mapper;

import org.springframework.stereotype.Repository;
import com.changchong.site.model.AppMessage;

@Repository
public interface AppMessageMapper {
    int deleteByPrimaryKey(Integer id);

    int insert(AppMessage record);

    int insertSelective(AppMessage record);

    AppMessage selectByPrimaryKey(Integer id);

    int updateByPrimaryKeySelective(AppMessage record);

    int updateByPrimaryKey(AppMessage record);
}