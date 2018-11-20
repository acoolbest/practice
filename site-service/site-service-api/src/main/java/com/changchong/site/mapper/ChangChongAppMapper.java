package com.changchong.site.mapper;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.changchong.site.model.ChangChongApp;
/**
 * 充电App版本信息
 * @author lxg
 *
 * 2017年5月24日下午4:29:07
 */
@Repository
public interface ChangChongAppMapper {

	 public abstract List<ChangChongApp> findAppVersion(Integer paramInteger1, Integer paramInteger2);

	  public abstract List<ChangChongApp> getList(Map<String, Object> paramMap);

	  public abstract void updateChangChongApp(ChangChongApp paramChangChongApp);

	  public abstract Integer saveChangChongApp(ChangChongApp paramChangChongApp);

	  public abstract Integer getCount(Map<String, Object> paramMap);
	  
	  public List<ChangChongApp> findAppVersionByParam(Map<String, Object> param);
}
