package com.changchong.site.serviceImpl;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.annotation.Resource;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import org.apache.log4j.Logger;
import org.springframework.dao.DataAccessException;
import org.springframework.data.redis.connection.RedisConnection;
import org.springframework.data.redis.core.RedisCallback;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Service;
import com.changchong.site.service.RedisService;


/**
 * 封装redis 缓存服务器服务接口
 */
@Service(value = "redisService")
public class RedisServiceImpl implements RedisService {

    Logger logger = Logger.getLogger(RedisServiceImpl.class);

    private static String redisCode = "utf-8";
    @Resource
    private RedisTemplate redisTemplate;

    /**
     * @param keys
     */
    public long del(final String keys) {
        return (Long) redisTemplate.execute(new RedisCallback() {
            public Long doInRedis(RedisConnection connection) throws DataAccessException {
                long result = 0;
                result = connection.del(keys.getBytes());
                return result;
            }
        });
    }

    /**
     * @param key
     * @param value
     * @param liveTime
     */
    public void set(final byte[] key, final byte[] value, final long liveTime) {
        redisTemplate.execute(new RedisCallback() {
            public Long doInRedis(RedisConnection connection) throws DataAccessException {
                connection.set(key, value);
                if (liveTime > 0) {
                    connection.expire(key, liveTime);
                }
                return 1L;
            }
        });
    }

    /**
     * @param key
     * @param value
     * @param liveTime
     */
    public void set(String key, String value, long liveTime) {
        try {
            this.set(key.getBytes(), value.getBytes(redisCode), liveTime);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }

    /**
     * @param key
     * @param value
     */
    public void set(String key, String value) {
        this.set(key, value, 0L);
    }

    /**
     * setList
     */
    public void setList(final String key, final List list) {
        redisTemplate.execute(new RedisCallback() {

            @Override
            public Object doInRedis(RedisConnection connection)
                    throws DataAccessException {
                connection.set(key.getBytes(), JSONArray.fromObject(list).toString().getBytes());
                return null;
            }

        });
    }

    /**
     * getList
     */
    public List getList(final String key, final Class clazz) {
        return (List) redisTemplate.execute(new RedisCallback() {

            @Override
            public Object doInRedis(RedisConnection connection) {
                try {
                    byte[] bs = connection.get(key.getBytes());
                    List list = JSONArray.toList(JSONArray.fromObject(new String(bs)), clazz);
                    if (list == null) {
                        list = new ArrayList();
                    }
                    return list;
                } catch (Exception e) {
                }
                return null;
            }

        });
    }

    /**
     * @param key
     * @param value
     */
    public void set(byte[] key, byte[] value) {
        this.set(key, value, 0L);
    }

    /**
     * @param key
     * @return
     */
    public String get(final String key) {
        logger.info("key:" + key);
        return (String) redisTemplate.execute(new RedisCallback() {
            public String doInRedis(RedisConnection connection) throws DataAccessException {
                try {
                    byte[] bytes = connection.get(key.getBytes());
                    if (bytes != null && bytes.length > 0) {
                        return new String(bytes, redisCode);
                    }


                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
                return "";
            }
        });
    }

    /**
     * @param pattern
     * @return
     */
    public Set keys(String pattern) {
        return redisTemplate.keys(pattern);

    }

    /**
     * @param key
     * @return
     */
    public boolean exists(final String key) {
        return (Boolean) redisTemplate.execute(new RedisCallback() {
            public Boolean doInRedis(RedisConnection connection) throws DataAccessException {
                return connection.exists(key.getBytes());
            }
        });
    }

    /**
     * @return
     */
    public String flushDB() {
        return (String) redisTemplate.execute(new RedisCallback() {
            public String doInRedis(RedisConnection connection) throws DataAccessException {
                connection.flushDb();
                return "ok";
            }
        });
    }

    /**
     * @return
     */
    public long dbSize() {
        return (Long) redisTemplate.execute(new RedisCallback() {
            public Long doInRedis(RedisConnection connection) throws DataAccessException {
                return connection.dbSize();
            }
        });
    }

    /**
     * @return
     */
    public String ping() {
        return (String) redisTemplate.execute(new RedisCallback() {
            public String doInRedis(RedisConnection connection) throws DataAccessException {

                return connection.ping();
            }
        });
    }

    /**
     * hset
     */
    public void hset(final String key, final String field, final String value) {
        redisTemplate.execute(new RedisCallback() {

            @Override
            public Object doInRedis(RedisConnection connection)
                    throws DataAccessException {
                connection.hSet(key.getBytes(), field.getBytes(), value.getBytes());
                return null;
            }

        });
    }

    /**
     * hget
     */
    public String hget(final String key, final String field) {
        return (String) redisTemplate.execute(new RedisCallback() {

            @Override
            public Object doInRedis(RedisConnection connection) {
                try {
                    byte[] bs = connection.hGet(key.getBytes(), field.getBytes());
                    return new String(bs);
                } catch (Exception e) {
                }
                return null;
            }

        });
    }

    /**
     * hGetAll
     */
    public Map<String, String> hGetAll(final String key) {

        return (Map<String, String>) redisTemplate.execute(new RedisCallback() {

            @Override
            public Object doInRedis(RedisConnection connection) {
                Map<String, String> rmap = new HashMap<String, String>();
                try {
                    Map<byte[], byte[]> map = connection.hGetAll(key.getBytes());
                    for (Map.Entry<byte[], byte[]> entry : map.entrySet()) {
                        rmap.put(new String(entry.getKey()), new String(entry.getValue()));
                    }
                    return rmap;
                } catch (Exception e) {
                }
                return null;
            }

        });
    }

    /**
     * hdel
     */
    public boolean hdel(final String key, final String field) {
        return (Boolean) redisTemplate.execute(new RedisCallback() {

            @Override
            public Object doInRedis(RedisConnection connection) {
                try {
                    connection.hDel(key.getBytes(), field.getBytes());
                    return true;
                } catch (Exception e) {
                }
                return false;
            }

        });
    }

    /**
     * @param key
     * @param obj
     * @param liveTime
     */
    public void set(String key, Object obj, long liveTime) {
        this.set(key.getBytes(), JSONObject.fromObject(obj).toString().getBytes(), liveTime);
    }

    public long incr(String key, long value) {
        return redisTemplate.opsForValue().increment(key, value);
    }

    /**
     * getObject
     */
    public Object getObject(final String key, final Class clazz) {
        return redisTemplate.execute(new RedisCallback() {

            @Override
            public Object doInRedis(RedisConnection connection) {
                try {
                    byte[] bs = connection.get(key.getBytes());
                    Object o = JSONObject.toBean(JSONObject.fromObject(new String(bs)), clazz);
                    return o;
                } catch (Exception e) {
                }
                return null;
            }

        });
    }
}