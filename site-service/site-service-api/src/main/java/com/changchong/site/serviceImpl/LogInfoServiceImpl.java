package com.changchong.site.serviceImpl;

import java.util.ArrayList;
import java.util.List;

import javax.annotation.Resource;

import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.global.page.PageUtil;
import com.changchong.site.app.dto.LogInfoRepo;
import com.changchong.site.app.service.LogInfoService;
import com.changchong.site.mapper.LogInfoMapper;
import com.changchong.site.model.LogInfo;


@Service
public class LogInfoServiceImpl implements LogInfoService {

	@Resource
	private LogInfoMapper infoMapper;

	public Integer saveLogInfo(LogInfoRepo logInfoRepo) {
		LogInfo logInfo = new LogInfo();
		BeanUtils.copyProperties(logInfoRepo, logInfo);
		Integer id = infoMapper.saveLogInfo(logInfo);
		return id;
	}
	public PageList<LogInfoRepo> getList(PageProperty pp) {
		int count = infoMapper.getCount(pp.getParamMap());
		int start = PageUtil.getStart(pp.getNpage(),count, pp.getNpagesize());
		int end = pp.getNpagesize();
		pp.putParamMap("startRow", start);
		pp.putParamMap("endRow", end);
		List<LogInfo> list = infoMapper.getList(pp.getParamMap());
		
		List<LogInfoRepo> infoRepos = new ArrayList<LogInfoRepo>();
		LogInfoRepo logInfoRepo = null;
		for(LogInfo lofInfo : list){
			logInfoRepo = new LogInfoRepo();
			BeanUtils.copyProperties(lofInfo, logInfoRepo);
			infoRepos.add(logInfoRepo);
		}
		PageList<LogInfoRepo> pageList = new PageList<LogInfoRepo>(pp, count, infoRepos);
		return pageList;
	}
	public List<LogInfoRepo> getOperationLogAccount() {
		List<LogInfo> list = infoMapper.getOperationLogAccount();
		List<LogInfoRepo> infoRepos = new ArrayList<LogInfoRepo>();
		LogInfoRepo logInfoRepo = null;
		for(LogInfo lofInfo : list){
			logInfoRepo = new LogInfoRepo();
			BeanUtils.copyProperties(lofInfo, logInfoRepo);
			infoRepos.add(logInfoRepo);
		}
		return infoRepos;
	}

}
