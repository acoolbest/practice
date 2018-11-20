package com.changchong.site.app.service;

import java.util.List;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.site.app.dto.LogInfoRepo;


public interface LogInfoService {

	public Integer saveLogInfo(LogInfoRepo logInfoRepo);
	
	/**
	 * 获取操作日志列表
	 * @param pp
	 * @return
	 */
	public PageList<LogInfoRepo> getList(PageProperty pp);
	
	/**
	 * 获取日志用户列表
	 * @return
	 */
	public List<LogInfoRepo> getOperationLogAccount();

}
