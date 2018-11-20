package com.changchong.site.service;

import java.util.List;
import java.util.Map;
import java.util.Set;

@SuppressWarnings("rawtypes")
public
interface RedisService {

    /**
     * 通过key删除
     */
    long del(String keys);

    /**
     * 添加key value 并且设置存活时间(byte)
     */
    void set(byte[] key, byte[] value, long liveTime);

    /**
     * 添加key value 并且设置存活时间
     *
     * @param liveTime 单位秒
     */
    void set(String key, String value, long liveTime);

    /**
     * 添加key value
     */
    void set(String key, String value);


    void setList(String key, List list);

    List getList(String key, Class clazz);

    /**
     * 添加key value (字节)(序列化)
     */
    void set(byte[] key, byte[] value);

    /**
     * 获取redis value (String)
     */
    String get(String key);

    /**
     * 通过正则匹配keys
     */
    Set keys(String pattern);

    /**
     * 检查key是否已经存在
     */
    boolean exists(String key);

    /**
     * 清空redis 所有数据
     */
    String flushDB();

    /**
     * 查看redis里有多少数据
     */
    long dbSize();

    /**
     * 检查是否连接成功
     */
    String ping();

    void hset(final String key, final String field, final String value);

    String hget(final String key, final String field);


    Map<String, String> hGetAll(final String key);

    boolean hdel(final String key, final String field);

    void set(String key, Object obj, long liveTime);

    long incr(String key, long value);

    /**
     * getObject
     *
     * 无：返回null
     */
    Object getObject(final String key, final Class clazz);

}
