package com.changchong.site.app.util;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.google.common.base.Predicate;
import com.google.common.collect.Maps;

import java.util.Map;

public class BeanUtil {

    static final ObjectMapper objectMapper = new ObjectMapper();

    /**
     * 将Map转换为Bean
     */
    public static <T> T mapToBean(Map map, Class<T> clazz) {
        return objectMapper.convertValue(map, clazz);
    }

    /**
     * 将Bean转换为Map(保持Bean属性的类型)
     */
    public static Map beanToMap(Object bean) {
        ObjectMapper objectMapper = new ObjectMapper();
        return objectMapper.convertValue(bean, Map.class);
    }

    /**
     * 将Bean转换为Map
     * 1.不保持Bean属性的类型，都转换为String
     * 2.进行空值过滤
     */
    public static Map<String, String> beanToMapForRedis(Object bean) {
        ObjectMapper objectMapper = new ObjectMapper();
        Map<String, String> props = objectMapper.convertValue(bean, new TypeReference<Map<String, String>>() {
        });
        Predicate<? super Object> nullFilter = new Predicate<Object>() {
            @Override
            public boolean apply(Object input) {
                return null != input;
            }
        };
        return Maps.filterValues(props, nullFilter);
    }
}
