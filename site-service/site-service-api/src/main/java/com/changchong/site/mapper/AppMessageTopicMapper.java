package com.changchong.site.mapper;

import org.springframework.stereotype.Repository;
import com.changchong.site.model.AppMessageTopic;

@Repository
public interface AppMessageTopicMapper {
    int deleteByPrimaryKey(Integer id);

    int insert(AppMessageTopic record);

    int insertSelective(AppMessageTopic record);

    AppMessageTopic selectByPrimaryKey(Integer id);

    int updateByPrimaryKeySelective(AppMessageTopic record);

    int updateByPrimaryKey(AppMessageTopic record);
}