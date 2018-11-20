package com.changchong.site.mapper;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.OperationLog;

@Repository
public interface OperationLogMapper {
	public Integer insertOperationLog(OperationLog operationLog);
}
