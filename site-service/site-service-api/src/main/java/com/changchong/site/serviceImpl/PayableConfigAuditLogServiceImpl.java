package com.changchong.site.serviceImpl;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import javax.annotation.Resource;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;
import com.changchong.site.app.dto.PayableConfigAuditLogDto;
import com.changchong.site.app.service.PayableConfigAuditLogService;
import com.changchong.site.mapper.PayableConfigAuditLogMapper;
import com.changchong.site.model.PayableConfigAuditLog;

@Service("payableConfigAuditLogService")
public class PayableConfigAuditLogServiceImpl implements PayableConfigAuditLogService {
	@Resource
	private PayableConfigAuditLogMapper payableConfigAuditLogMapper;

	@Override
	public List<PayableConfigAuditLogDto> getPayableConfigAuditLogList(Map<String, Object> param) {
		List<PayableConfigAuditLog> pclist = payableConfigAuditLogMapper.getPayableConfigAuditLogList(param);
		List<PayableConfigAuditLogDto> list = new ArrayList<PayableConfigAuditLogDto>();
		if (pclist.size() > 0) {
			for (PayableConfigAuditLog payableConfigAuditLog : pclist) {
				PayableConfigAuditLogDto payableConfigAuditLogDto = new PayableConfigAuditLogDto();
				BeanUtils.copyProperties(payableConfigAuditLog, payableConfigAuditLogDto);
				list.add(payableConfigAuditLogDto);
			}
		}
		return list;
	}

	@Override
	public Integer savePayableConfigAuditLog(PayableConfigAuditLogDto payableConfigAuditLogDto) {
		PayableConfigAuditLog payableConfigAuditLog = null;
		if (payableConfigAuditLogDto != null) {
			payableConfigAuditLog = new PayableConfigAuditLog();
			BeanUtils.copyProperties(payableConfigAuditLogDto, payableConfigAuditLog);
			payableConfigAuditLogMapper.savePayableConfigAuditLog(payableConfigAuditLog);
		}
		return payableConfigAuditLog.getId();
	}

	@Override
	public void updatePayableConfigAuditLog(PayableConfigAuditLogDto payableConfigAuditLogDto) {
		if (payableConfigAuditLogDto != null) {
			PayableConfigAuditLog payableConfigAuditLog = new PayableConfigAuditLog();
			BeanUtils.copyProperties(payableConfigAuditLogDto, payableConfigAuditLog);
			payableConfigAuditLogMapper.updatePayableConfigAuditLog(payableConfigAuditLog);
		}

	}

}
