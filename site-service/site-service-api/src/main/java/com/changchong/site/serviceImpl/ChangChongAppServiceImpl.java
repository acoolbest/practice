package com.changchong.site.serviceImpl;

import com.changchong.global.page.PageList;
import com.changchong.global.page.PageProperty;
import com.changchong.global.page.PageUtil;
import com.changchong.site.app.dto.ChangChongAppDto;
import com.changchong.site.app.service.ChangChongAppService;
import com.changchong.site.mapper.ChangChongAppMapper;
import com.changchong.site.model.ChangChongApp;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import javax.annotation.Resource;

import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

@Service
public class ChangChongAppServiceImpl implements ChangChongAppService {

	@Resource
	private ChangChongAppMapper changChongAppMapper;

	public ChangChongAppDto findAppVersion(Integer versionCode, Integer osType) {
		List<ChangChongApp> changChongApps = this.changChongAppMapper.findAppVersion(versionCode, osType);
		ChangChongAppDto changChongAppDto = null;
		if (changChongApps.size() > 0) {
			changChongAppDto = new ChangChongAppDto();
			ChangChongApp changChongApp = (ChangChongApp) changChongApps.get(0);
			BeanUtils.copyProperties(changChongApp, changChongAppDto);
		}
		return changChongAppDto;
	}
	@Transactional(propagation = Propagation.REQUIRED, rollbackFor = RuntimeException.class)
	public Integer saveChangChongApp(ChangChongAppDto changChongAppDto) {
		ChangChongApp changChongApp = new ChangChongApp();
		BeanUtils.copyProperties(changChongAppDto, changChongApp);
		Integer id = this.changChongAppMapper.saveChangChongApp(changChongApp);
		return id;
	}

	public PageList<ChangChongAppDto> getList(PageProperty pp) {
		int count = this.changChongAppMapper.getCount(pp.getParamMap()).intValue();
		int start = PageUtil.getStart(pp.getNpage(), count, pp.getNpagesize());
		int end = pp.getNpagesize();
		pp.putParamMap("startRow", Integer.valueOf(start));
		pp.putParamMap("endRow", Integer.valueOf(end));
		List<ChangChongApp> list = this.changChongAppMapper.getList(pp.getParamMap());

		List<ChangChongAppDto> infoRepos = new ArrayList<ChangChongAppDto>();
		ChangChongAppDto changChongAppDto = null;
		for (ChangChongApp changChongApp : list) {
			changChongAppDto = new ChangChongAppDto();
			BeanUtils.copyProperties(changChongApp, changChongAppDto);
			infoRepos.add(changChongAppDto);
		}
		PageList<ChangChongAppDto> pageList = new PageList<ChangChongAppDto>(pp, count, infoRepos);
		return pageList;
	}
	@Transactional(propagation = Propagation.REQUIRED, rollbackFor = RuntimeException.class)
	public void updateChangChongApp(ChangChongAppDto changChongAppDto) {
		if (changChongAppDto != null) {
			ChangChongApp changChongApp = new ChangChongApp();
			BeanUtils.copyProperties(changChongAppDto, changChongApp);
			this.changChongAppMapper.updateChangChongApp(changChongApp);
		}
	}

	@Override
	public ChangChongAppDto findAppVersionByParam(Map<String, Object> param) {
		List<ChangChongApp> changChongApp = this.changChongAppMapper.findAppVersionByParam(param);
		ChangChongAppDto changChongAppDto = null;
		if (changChongApp != null && changChongApp.size() > 0) {
			changChongAppDto = new ChangChongAppDto();
			BeanUtils.copyProperties(changChongApp.get(0), changChongAppDto);
		}
		return changChongAppDto;
	}
}