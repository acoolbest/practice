package com.changchong.sysconfig;


import com.google.gson.Gson;
import org.apache.log4j.Logger;


import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.net.URL;

import java.util.Map;
import java.util.Properties;

/**
 * Created by cm on 2017/8/9.
 */
public class PropertiesUtil
{
	private static Logger log = Logger.getLogger(PropertiesUtil.class);
	private static final String fileName = "configs.properties";
	private static final String sourceFile = "applicationContext-dubbo.xml";
	public  static void configToProperties(Map<String,Object> param,Byte env_type){

		log.info("configToProperties 传的参数   param ====>"+new Gson().toJson(param)+"========   env_type========>"+env_type);
		URL url = PropertiesUtil.class.getClassLoader().getResource(sourceFile);
		if(url != null){
			String path = url.getPath();
			log.info("configToProperties sourece路径 ====>"+path);
			String realPath = path.replace(sourceFile,fileName);
			File file = new File(realPath);
			if(file.exists()){
				//如果文件存在
				file.delete();
				log.info("文件存在  path=========>"+path);
			}
			try{
				file.createNewFile();
				Properties  properties = new Properties();
				properties.load(PropertiesUtil.class.getClassLoader().getResourceAsStream(fileName));
				OutputStream out = new FileOutputStream(realPath);
				for(String key : param.keySet()){
					properties.put(key,param.get(key));
				}
				String envDesc = null;
				if(env_type == 1){
					envDesc = "dev";
				}
				if(env_type == 2){
					envDesc = "pro";
				}
				properties.store(out,envDesc);
			}catch (Exception e){
				e.printStackTrace();
			}
		}
	}
}
