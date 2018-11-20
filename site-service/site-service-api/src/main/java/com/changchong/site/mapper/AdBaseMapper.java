package com.changchong.site.mapper;

import com.changchong.site.model.AdBase;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;
@Repository
public interface AdBaseMapper {

    int deleteByPrimaryKey(Integer id);

    int insertSelective(AdBase record);

    AdBase selectByPrimaryKey(Integer id);

    int updateByPrimaryKeySelective(AdBase record);

    int updateByPrimaryKey(AdBase record);

    List<Integer> selectAvailableAdIds();
}