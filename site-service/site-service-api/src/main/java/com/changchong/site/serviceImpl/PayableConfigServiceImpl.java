package com.changchong.site.serviceImpl;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import javax.annotation.Resource;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;
import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.global.page.PageUtil;
import com.changchong.site.app.dto.PayableConfigDto;
import com.changchong.site.app.service.PayableConfigService;
import com.changchong.site.mapper.PayableConfigMapper;
import com.changchong.site.model.PayableConfig;

@Service
public class PayableConfigServiceImpl implements PayableConfigService {

	@Resource
	private PayableConfigMapper payableConfigMapper;
	
	@Override
	public List<PayableConfigDto> getPayableConfigList(Map<String, Object> param) {
		List<PayableConfig> payableConfigs = payableConfigMapper.getPayableConfigList(param);
		List<PayableConfigDto> payableConfigDtos = new ArrayList<PayableConfigDto>();
		PayableConfigDto payableConfigDto = null;
		if(payableConfigs.size() == 0){
			return null;
		}
		for(PayableConfig payableConfig : payableConfigs){
			payableConfigDto = new PayableConfigDto();
			BeanUtils.copyProperties(payableConfig, payableConfigDto);
			payableConfigDtos.add(payableConfigDto);
		}
		return payableConfigDtos;
	}



	@Override
	public Integer savePayableConfig(PayableConfigDto payableConfigDto) {
		PayableConfig payableConfig = null;
		if(payableConfigDto != null){
			payableConfig = new PayableConfig();
			BeanUtils.copyProperties(payableConfigDto, payableConfig);
			payableConfigMapper.savePayableConfig(payableConfig);
		}
		return payableConfig.getId();
	}

	@Override
	public void updatePayableConfig(PayableConfigDto payableConfigDto) {
		if(payableConfigDto != null){
			PayableConfig payableConfig = new PayableConfig();
			BeanUtils.copyProperties(payableConfigDto, payableConfig);
			payableConfigMapper.updatePayableConfig(payableConfig);
		}
		
	}
	@Override
	public PageList<PayableConfigDto> getAuditLogList(PageProperty pp) {
		int count = payableConfigMapper.getCount(pp.getParamMap());
		int start = PageUtil.getStart(pp.getNpage(), count, pp.getNpagesize());
		int end = pp.getNpagesize();
		pp.putParamMap("startRow", start);
		pp.putParamMap("endRow", end);
		List<PayableConfig> list = payableConfigMapper.getAuditList(pp.getParamMap());
		PayableConfigDto payableConfigDto = null;
		List<PayableConfigDto> list1 = new ArrayList<PayableConfigDto>();
		for (PayableConfig payableConfig : list) {
			payableConfigDto = new PayableConfigDto();
			BeanUtils.copyProperties(payableConfig, payableConfigDto);
			list1.add(payableConfigDto);
		}

		PageList<PayableConfigDto> pageList = new PageList<PayableConfigDto>(pp, count, list1);

		return pageList;
	}

	@Override
	public List<PayableConfigDto> getEnablePayableConfigList(Map<String, Object> param) {
		List<PayableConfig> payableConfigs = payableConfigMapper.getEnablePayableConfigList(param);
		List<PayableConfigDto> payableConfigDtos = new ArrayList<PayableConfigDto>();
		if(payableConfigs != null && payableConfigs.size() > 0){
			for( PayableConfig payableConfig : payableConfigs) {
				PayableConfigDto payableConfigDto = new PayableConfigDto();
				BeanUtils.copyProperties(payableConfig, payableConfigDto);
				payableConfigDtos.add(payableConfigDto);
			}
		}
		return payableConfigDtos;
	}
}
