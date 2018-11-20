package com.changchong.site.mapper;

import com.changchong.site.model.Area;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;

/**
 * Created by cm on 2017/5/19.
 */
@Repository
public interface AreaMapper {
    /**
     * 获取区域列表
     * @param map
     * @return
     */
    List<Area> getAreaList(Map<String,Object> map);
}
