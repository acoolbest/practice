package com.changchong.site.app.serviceImpl;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.global.page.PageUtil;
import com.changchong.site.app.dto.ChargeProblemDto;
import com.changchong.site.app.service.ChargeProblemService;
import com.changchong.site.mapper.ChargeProblemMapper;
import com.changchong.site.model.ChargeProblem;

import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by cm on 2017/5/23.
 */
@Service("chargeProblemService")
public class ChargeProblemServiceImpl implements ChargeProblemService {
    @Resource
    private ChargeProblemMapper chargeProblemMapper;

    @Override
    public PageList<ChargeProblemDto> getChargeProblemList(PageProperty pageProperty) {
        int count = chargeProblemMapper.getWebChargeProblemListCount(pageProperty.getParamMap());
        int start = PageUtil.getStart(pageProperty.getNpage(),count,pageProperty.getNpagesize());
        int end = pageProperty.getNpagesize();
        pageProperty.putParamMap("startRow", start);
        pageProperty.putParamMap("endRow", end);
        List<ChargeProblem> list = chargeProblemMapper.getWebChargeProblemList(pageProperty.getParamMap());
        if(list==null){
            return null;
        }
        List<ChargeProblemDto> list1 = new ArrayList<ChargeProblemDto>();
        for(ChargeProblem chargeProblem : list){
            ChargeProblemDto chargeProblemDto = new ChargeProblemDto();
            BeanUtils.copyProperties(chargeProblem,chargeProblemDto);
            list1.add(chargeProblemDto);
        }
        PageList<ChargeProblemDto> result = new PageList<ChargeProblemDto>(pageProperty,count,list1);
        return result;
    }
    
    public List<ChargeProblemDto> getChargeProblemList(Map<String, Object> paraMap){
    	
    	List<ChargeProblem> list = chargeProblemMapper.getChargeProblemList(paraMap);
    	List<ChargeProblemDto> list1 = new ArrayList<ChargeProblemDto>();
        for(ChargeProblem chargeProblem : list){
            ChargeProblemDto chargeProblemDto = new ChargeProblemDto();
            BeanUtils.copyProperties(chargeProblem,chargeProblemDto);
            list1.add(chargeProblemDto);
        }
        return list1;
    }

    @Override
    public Integer saveChargeProblemDto(ChargeProblemDto chargeProblemDto) {
        ChargeProblem chargeProblem = new ChargeProblem();
        BeanUtils.copyProperties(chargeProblemDto,chargeProblem);
        return chargeProblemMapper.saveChargeProblem(chargeProblem);
    }

    @Override
    public Integer updateChargeProblemDto(ChargeProblemDto chargeProblemDto) {
       ChargeProblem chargeProblem = new ChargeProblem();
        BeanUtils.copyProperties(chargeProblemDto,chargeProblem);
        return chargeProblemMapper.updateChargeProblem(chargeProblem);
    }

    @Override
    public void deleteChargeProblemDto(Integer id) {
        chargeProblemMapper.deleteChargeProblem(id);
    }
}
