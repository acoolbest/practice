package com.changchong.site.app.serviceImpl;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.global.page.PageUtil;
import com.changchong.site.app.dto.AppReportDto;
import com.changchong.site.app.service.AppReportService;
import com.changchong.site.mapper.AppReportMapper;
import com.changchong.site.model.AppReport;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by cm on 2017/5/20.
 */
@Service("appReportService")
public class AppReportServiceImp implements AppReportService {
    @Resource
    AppReportMapper appReportMapper ;

    @Override
    public Integer saveAppReport(AppReportDto appReportDto) {
        AppReport appReport = new AppReport();
        BeanUtils.copyProperties(appReportDto,appReport);
        Integer number = appReportMapper.saveAppReport(appReport);
        return number;
    }

    @Override
    public Integer updateAppReport(AppReportDto appReportDto) {
        AppReport appReport = new AppReport();
        BeanUtils.copyProperties(appReportDto,appReport);
        Integer number = appReportMapper.updateAppReport(appReport);
        return number;
    }

    @Override
    public PageList<AppReportDto> getAppReportList(PageProperty pageProperty) {
        String address = (String) pageProperty.getParamMap().get("address");
        String province = (String) pageProperty.getParamMap().get("province");
        String city = (String) pageProperty.getParamMap().get("city");
        pageProperty
                .putParamMap("likecondition","AND INSTR(CONCAT(IFNULL(t.province,''),IFNULL(t.city,''),IFNULL(t.address,'')), '"
                        +province+city+address+"')>0");
        int count = appReportMapper.getAppReportListCount(pageProperty.getParamMap());
        int start = PageUtil.getStart(pageProperty.getNpage(),count,pageProperty.getNpagesize());
        int end = pageProperty.getNpagesize();
        pageProperty.putParamMap("startRow", start);
        pageProperty.putParamMap("endRow", end);
        List<AppReport> list = appReportMapper.getAppReportList(pageProperty.getParamMap());
        if(list == null){
            return null;
        }
       List<AppReportDto> list1 = new ArrayList<AppReportDto>();
        for(AppReport appReport : list){
            AppReportDto appReportDto = new AppReportDto();
            BeanUtils.copyProperties(appReport,appReportDto);
            list1.add(appReportDto);
        }
        PageList<AppReportDto> result = new PageList<AppReportDto>(pageProperty,count,list1);
        return result;
    }
}
