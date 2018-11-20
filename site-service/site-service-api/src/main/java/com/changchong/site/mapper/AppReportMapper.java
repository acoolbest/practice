package com.changchong.site.mapper;

import org.springframework.stereotype.Repository;
import com.changchong.site.model.AppReport;

import java.util.List;
import java.util.Map;

@Repository
public interface AppReportMapper {

    List<AppReport> getAppReportList(Map<String,Object> map);

    Integer saveAppReport(AppReport appReport);

    Integer getAppReportListCount(Map<String,Object> map);

    Integer updateAppReport(AppReport appReport);

}