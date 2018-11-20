package com.changchong.site.mapper;

import com.changchong.site.model.DeviceOld;
import com.changchong.site.model.SiteInfo;
import com.changchong.site.model.SitePo;
import org.springframework.stereotype.Repository;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Repository
public interface SiteMapper {


	List<SiteInfo> getSiteInfoList(Map<String,Object> map);


    DeviceOld getDeviceInfoByCode(String deviceCode);

    void updatePriceById(HashMap hashMap);

    SitePo findById(int siteId);

    SitePo findByPortNumber(int portNumber);


    SiteInfo findByPk(Integer id);

    /**
     * 根据端口号查询站点信息
     * @param portCode
     * @return
     */
    SiteInfo findByPortCode(Integer portCode);
}
