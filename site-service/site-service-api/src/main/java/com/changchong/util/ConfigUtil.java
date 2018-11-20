package com.changchong.util;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class ConfigUtil {
	private static Properties pay = new Properties(); 
    static {  
        InputStream pay_in = null;  
        try {  
            pay_in = ConfigUtil.class.getClassLoader().getResourceAsStream("env.properties");
            if(pay!=null){  
                pay.load(pay_in);  
            } 
        } catch (IOException e) {  
            e.printStackTrace();  
        }
    }  
      
  
    
    
    public static String getPayValue(String key) {

		String value = "";
		synchronized (pay) {
			value = pay.getProperty(key);
		}
		return value;
	}
	
}
