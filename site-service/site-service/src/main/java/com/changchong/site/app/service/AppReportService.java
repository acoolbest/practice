package com.changchong.site.app.service;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.site.app.dto.AppReportDto;
import com.changchong.site.app.dto.AreaDto;

import java.util.List;

/**
 * Created by cm on 2017/5/19.
 */
public interface AppReportService {
    /**
     * 保存APP举报
     * @param appReportDto
     * @return
     */
    Integer saveAppReport(AppReportDto appReportDto);

    /**
     * 更新App举报
     * @param appReportDto
     * @return
     */
   Integer updateAppReport(AppReportDto appReportDto);

    /**
     * 获取App举报列表
     * @param pageProperty
     * @return
     */
    PageList<AppReportDto> getAppReportList(PageProperty pageProperty);
}
