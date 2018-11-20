package com.changchong.site.app.service;

import java.util.List;
import java.util.Map;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.site.app.dto.ChargeProblemDto;

/**
 * Created by cm on 2017/5/23.
 */
public interface ChargeProblemService {
    /**
     * 获取热门问题列表
     * @param pageProperty
     * @return
     */
    PageList<ChargeProblemDto> getChargeProblemList(PageProperty pageProperty);

    /**
     * APP端获取热门问题列表
     * @author lxg
     * @return
     */
    public List<ChargeProblemDto> getChargeProblemList(Map<String, Object> paraMap);
    /**
     * 保存热门问题
     * @param chargeProblemDto
     * @return
     */
    Integer saveChargeProblemDto(ChargeProblemDto chargeProblemDto);

    /**
     * 更新热门问题
     * @param chargeProblemDto
     * @return
     */
    Integer updateChargeProblemDto(ChargeProblemDto chargeProblemDto);

    /**
     * 删除热门问题
     * @param id
     */
    void deleteChargeProblemDto(Integer id);
}
