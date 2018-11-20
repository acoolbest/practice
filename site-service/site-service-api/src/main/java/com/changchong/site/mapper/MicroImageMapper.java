package com.changchong.site.mapper;

import com.changchong.site.model.MicroImage;
import org.springframework.stereotype.Repository;

import java.util.List;

/**
 * @Package: com.changchong.site.mapper.MicroImageMapper.java
 * @Description: 
 * @Company: null
 * @Copyright: null
 * Author zhoumin
 * @date 2018/03/19 11:55
 * version v1.0.0
 */
@Repository
public interface MicroImageMapper {
    /**
     * @Description: 根据主键删除数据库的记录
     * @Title deleteByPrimaryKey
     * @Author zhoumin
     * @Date 2018/03/19 11:55
     * @param id
     * @return int
     * @throws []
     */
    int deleteByPrimaryKey(Integer id);

    /**
     * @Description: 插入数据库记录
     * @Title insert
     * @Author zhoumin
     * @Date 2018/03/19 11:55
     * @param record
     * @return int
     * @throws []
     */
    int insert(MicroImage record);

    /**
     * @Description: 选择性插入数据库记录
     * @Title insertSelective
     * @Author zhoumin
     * @Date 2018/03/19 11:55
     * @param record
     * @return int
     * @throws []
     */
    int insertSelective(MicroImage record);

    /**
     * @Description: 根据主键获取一条数据库记录
     * @Title selectByPrimaryKey
     * @Author zhoumin
     * @Date 2018/03/19 11:55
     * @param id
     * @return com.changchong.site.model.MicroImage
     * @throws []
     */
    MicroImage selectByPrimaryKey(Integer id);

    /**
     * @Description: 根据主键来更新对应数据库字段
     * @Title updateByPrimaryKeySelective
     * @Author zhoumin
     * @Date 2018/03/19 11:55
     * @param record
     * @return int
     * @throws []
     */
    int updateByPrimaryKeySelective(MicroImage record);

    /**
     * @Description: 根据主键来更新数据库记录
     * @Title updateByPrimaryKey
     * @Author zhoumin
     * @Date 2018/03/19 11:55
     * @param record
     * @return int
     * @throws []
     */
    int updateByPrimaryKey(MicroImage record);

    List<String> queryMicroImages();
}