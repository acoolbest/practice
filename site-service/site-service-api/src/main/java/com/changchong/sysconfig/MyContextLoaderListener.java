package com.changchong.sysconfig;

import com.google.gson.Gson;
import org.apache.log4j.Logger;
import org.springframework.http.converter.FormHttpMessageConverter;
import org.springframework.http.converter.HttpMessageConverter;
import org.springframework.http.converter.StringHttpMessageConverter;
import org.springframework.http.converter.json.MappingJackson2HttpMessageConverter;
import org.springframework.web.client.RestTemplate;
import org.springframework.web.context.ContextLoaderListener;
import org.springframework.web.context.WebApplicationContext;


import javax.servlet.ServletContext;

import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by cm on 2017/8/9.
 */

public class MyContextLoaderListener extends ContextLoaderListener
{

	private static Logger log = Logger.getLogger(MyContextLoaderListener.class);
	private static final String url = "http://127.0.0.1:8088/configApi/projectconfig/v1/config/get?project=SITE_SERVICE";

	@Override public WebApplicationContext initWebApplicationContext(ServletContext servletContext)
	{
		RestTemplate restTemplate = new RestTemplate();
		List<HttpMessageConverter<?>> list = new ArrayList<>();
		list.add(new FormHttpMessageConverter());
		list.add(new MappingJackson2HttpMessageConverter());
		StringHttpMessageConverter stringHttpMessageConverter = new StringHttpMessageConverter(Charset.forName("UTF-8"));
		list.add(stringHttpMessageConverter);
		restTemplate.setMessageConverters(list);

		ResultDo resultDo = restTemplate.getForObject(url,ResultDo.class);
		log.info("MyContextLoaderListener    getConfig远程调用的返回值================》"+new Gson().toJson(resultDo));
		if(resultDo != null){
			Object configObject = resultDo.getResult();
			String json = new Gson().toJson(configObject);
			log.info("MyContextLoaderListener    getConfig远程调用返回result================》"+json);
			if(json != null){
				ConfigDto configDto = new Gson().fromJson(json,ConfigDto.class);
				if(configDto!=null){
					String configString = configDto.getConfig();
					log.info("MyContextLoaderListener    远程调用返回config================》"+configString);
					Map<String,Object> keyValue = new Gson().fromJson(configString, HashMap.class);
					log.info("MyContextLoaderListener    远程调用返回config解析成map================》"+new Gson().toJson(keyValue));
					PropertiesUtil.configToProperties(keyValue,configDto.getEnv_type());
				}
			}

		}
		return super.initWebApplicationContext(servletContext);
	}
}
