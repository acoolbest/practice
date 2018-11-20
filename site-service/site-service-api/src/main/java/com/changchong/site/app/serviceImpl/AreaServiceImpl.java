package com.changchong.site.app.serviceImpl;

import com.changchong.site.app.dto.AreaDto;
import com.changchong.site.app.service.AreaService;
import com.changchong.site.mapper.AreaMapper;
import com.changchong.site.model.Area;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;
import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by cm on 2017/5/19.
 */
@Service(value = "areaService")
public class AreaServiceImpl implements AreaService {
    @Resource
    private AreaMapper areaMapper;
    @Override
    public List<AreaDto> getProvinceList() {
        Map<String,Object> map = new HashMap<String,Object>();
        map.put("pid",0);
        List<Area> list = areaMapper.getAreaList(map);
        if(list==null){
            return null;
        }else{
            List<AreaDto> list1 = new ArrayList<AreaDto>();
            for(Area area : list){
                AreaDto areaDto = new AreaDto();
                BeanUtils.copyProperties(area,areaDto);
                list1.add(areaDto);
            }
            return list1;
        }
    }

    @Override
    public List<AreaDto> getCityListByProvince(Integer id) {
       Map<String,Object> map = new HashMap<String,Object>();
        map.put("pid",id);
        List<Area> list = areaMapper.getAreaList(map);
        if(list==null){
            return null;
        }else{
            List<AreaDto> list1 = new ArrayList<AreaDto>();
            for(Area area : list){
                AreaDto areaDto = new AreaDto();
                BeanUtils.copyProperties(area,areaDto);
                list1.add(areaDto);
            }
            return list1;
        }
    }
}
