package com.changchong.site.app.util;

import java.util.HashMap;

public class NotNullHashMap<K> extends HashMap<K, String> {

	private static final long serialVersionUID = 4931790060307908679L;

    public String putIfNotNull(K key, Object value) {
        if(value == null){
			return null;
		}
        return super.put(key, value.toString());
    }
	
}
