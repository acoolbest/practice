package com.changchong.site.model;

import java.util.Date;

/**
 * @Package: com.changchong.site.model.MicroImage.java
 * @Description: 
 * @Company: null
 * @Copyright: null
 * Author zhoumin
 * @date 2018/03/19 11:55
 * version v1.0.0
 */
public class MicroImage {
    private Integer id;

    private String imgUrl;

    /**
     * 删除0 正常1
     */
    private Integer status;

    private Integer creater;

    private Date createTime;

    private Integer modifier;

    private Date updateTime;

    /**
     * @return id
     */
    public Integer getId() {
        return id;
    }

    /**
     * @param id
     */
    public void setId(Integer id) {
        this.id = id;
    }

    /**
     * @return img_url
     */
    public String getImgUrl() {
        return imgUrl;
    }

    /**
     * @param imgUrl
     */
    public void setImgUrl(String imgUrl) {
        this.imgUrl = imgUrl == null ? null : imgUrl.trim();
    }

    /**
     * 获取删除0 正常1
     *
     * @return status
     */
    public Integer getStatus() {
        return status;
    }

    /**
     * 设置删除0 正常1
     *
     * @param status
     */
    public void setStatus(Integer status) {
        this.status = status;
    }

    /**
     * @return creater
     */
    public Integer getCreater() {
        return creater;
    }

    /**
     * @param creater
     */
    public void setCreater(Integer creater) {
        this.creater = creater;
    }

    /**
     * @return create_time
     */
    public Date getCreateTime() {
        return createTime;
    }

    /**
     * @param createTime
     */
    public void setCreateTime(Date createTime) {
        this.createTime = createTime;
    }

    /**
     * @return modifier
     */
    public Integer getModifier() {
        return modifier;
    }

    /**
     * @param modifier
     */
    public void setModifier(Integer modifier) {
        this.modifier = modifier;
    }

    /**
     * @return update_time
     */
    public Date getUpdateTime() {
        return updateTime;
    }

    /**
     * @param updateTime
     */
    public void setUpdateTime(Date updateTime) {
        this.updateTime = updateTime;
    }
}