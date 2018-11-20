package com.changchong.site.serviceImpl;

import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import com.changchong.site.mapper.OperationLogMapper;
import com.changchong.site.model.OperationLog;
import com.changchong.site.pay.dto.OperationLogDto;
import com.changchong.site.service.OperationService;

@Service(value = "operationService")
public class OperationServiceImpl implements OperationService{
	
	@Autowired
	private OperationLogMapper operationLogMapper;
	
	@Override
	@Transactional(propagation = Propagation.REQUIRED, rollbackFor = RuntimeException.class)
	public void insertOperation(OperationLogDto operationLogDto) {
		OperationLog operationLog = new OperationLog();
		BeanUtils.copyProperties(operationLogDto, operationLog);
		operationLogMapper.insertOperationLog(operationLog);
	}

}
