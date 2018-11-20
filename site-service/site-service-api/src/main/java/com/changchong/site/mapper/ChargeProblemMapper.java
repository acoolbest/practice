package com.changchong.site.mapper;


import com.changchong.site.model.ChargeProblem;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Map;


@Repository
public interface ChargeProblemMapper {

    Integer updateChargeProblem(ChargeProblem chargeProblem);

    Integer getChargeProblemListCount(Map<String,Object> map);

    Integer getWebChargeProblemListCount(Map<String,Object> map);

    List<ChargeProblem> getChargeProblemList(Map<String,Object> map);

    List<ChargeProblem> getWebChargeProblemList(Map<String,Object> map);

    Integer saveChargeProblem(ChargeProblem chargeProblem);

    void deleteChargeProblem(Integer id);
}
