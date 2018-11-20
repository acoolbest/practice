package com.changchong.site.serviceImpl;

import java.util.List;
import java.util.Map;
import javax.annotation.Resource;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;
import com.changchong.site.app.dto.AppInstallDto;
import com.changchong.site.app.service.AppInstallService;
import com.changchong.site.mapper.AppInstallMapper;
import com.changchong.site.model.AppInstall;

@Service
public class AppInstallServiceImpl implements AppInstallService {

	@Resource
	private AppInstallMapper appInstallMapper;
	
	@Override
	public List<AppInstallDto> findAppInstallList(Map<String, Object> param) {
		return null;
	}

	@Override
	@Transactional(propagation = Propagation.REQUIRED, rollbackFor = RuntimeException.class)
	public Integer save(AppInstallDto appInstallDto) {
		AppInstall appInstall = new AppInstall();
		BeanUtils.copyProperties(appInstallDto, appInstall);
		appInstallMapper.save(appInstall);
		return appInstall.getId();
	}

}
