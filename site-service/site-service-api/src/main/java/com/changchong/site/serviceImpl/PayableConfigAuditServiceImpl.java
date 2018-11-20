package com.changchong.site.serviceImpl;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import javax.annotation.Resource;

import org.springframework.beans.BeanUtils;


import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.global.page.PageUtil;
import com.changchong.site.app.dto.PayableConfigAuditDto;
import com.changchong.site.app.service.PayableConfigAuditService;
import com.changchong.site.mapper.PayableConfigAuditMapper;
import com.changchong.site.model.PayableConfigAudit;

public class PayableConfigAuditServiceImpl implements PayableConfigAuditService {
	@Resource
	private PayableConfigAuditMapper payableConfigAuditMapper;
	@Override
	public PageList<PayableConfigAuditDto> getAuditLogList(PageProperty pp) {
		int count = payableConfigAuditMapper.getCount(pp.getParamMap());
		int start = PageUtil.getStart(pp.getNpage(), count, pp.getNpagesize());
		int end = pp.getNpagesize();
		pp.putParamMap("startRow", start);
		pp.putParamMap("endRow", end);
		List<PayableConfigAudit> list = payableConfigAuditMapper.getAuditList(pp.getParamMap());
		PayableConfigAuditDto payableConfigAuditDto = null;
		List<PayableConfigAuditDto> list1 = new ArrayList<PayableConfigAuditDto>();
		for (PayableConfigAudit payableConfigAudit : list) {
			payableConfigAuditDto = new PayableConfigAuditDto();
			BeanUtils.copyProperties(payableConfigAudit, payableConfigAuditDto);
			list1.add(payableConfigAuditDto);
		}

		PageList<PayableConfigAuditDto> pageList = new PageList<PayableConfigAuditDto>(pp, count, list1);

		return pageList;
	}


	@Override
	public Integer savePayableConfigAudit(PayableConfigAuditDto payableConfigAuditDto) {
		PayableConfigAudit payableConfigAudit = null;
		if (payableConfigAuditDto != null) {
			payableConfigAudit = new PayableConfigAudit();
			BeanUtils.copyProperties(payableConfigAuditDto, payableConfigAudit);
			payableConfigAuditMapper.savePayableConfigAudit(payableConfigAudit);
		}
		return payableConfigAudit.getId();
	}

	@Override
	public void updatePayableConfigAudit(PayableConfigAuditDto payableConfigAuditDto) {
		if (payableConfigAuditDto != null) {
			PayableConfigAudit payableConfigAudit = new PayableConfigAudit();
			BeanUtils.copyProperties(payableConfigAuditDto, payableConfigAudit);
			payableConfigAuditMapper.updatePayableConfigAudit(payableConfigAudit);
		}

	}


	@Override
	public PayableConfigAuditDto getPayableConfigAuditDto(Map<String, Object> param) {
		PayableConfigAudit payableConfigAudit = payableConfigAuditMapper.getPayableConfigAuditDto(param);
		PayableConfigAuditDto payableConfigAuditDto = null;
		if(payableConfigAudit != null){
			payableConfigAuditDto = new PayableConfigAuditDto();
			BeanUtils.copyProperties(payableConfigAudit, payableConfigAuditDto);
		}
		return payableConfigAuditDto;
	}

}
