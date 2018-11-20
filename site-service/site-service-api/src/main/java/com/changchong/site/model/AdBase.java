package com.changchong.site.model;

import lombok.Data;

import java.io.Serializable;
import java.util.Date;

@Data
public class AdBase implements Serializable {
    private Integer id;

    /**
     * 文件md5
     */
    private String md5;

    /**
     * 推广版本号
     */
    private String versionId;

    /**
     * 推广名称
     */
    private String adName;

    /**
     * 客户名称
     */
    private String customer;

    /**
     * 客户营业执照号/身份证号
     */
    private String certificate;

    /**
     * 推广内容
     */
    private String adImg;

    /**
     * 图片宽
     */
    private Integer adImgWidth;

    /**
     * 图片高
     */
    private Integer adImgHeight;

    /**
     * 图片大小(单位:byte)
     */
    private Long adImgSize;

    /**
     * 推广类型(0：扫码推广;1：开屏推广)
     */
    private Integer adType;

    /**
     * 推广时间限制(0：不限制;1：限制)
     */
    private Integer dateLimit;

    /**
     * 推广开始时间
     */
    private Date adStartTime;

    /**
     * 推广结束时间
     */
    private Date adEndTime;

    /**
     * 是否定向(0：否;1：是)
     */
    private Integer isDirect;

    /**
     * 审核状态(0:待审核;1:审核未通过;2：审核中;3：一审通过;4：二审通过;5：三审通过)
     */
    private Integer auditStatus;

    /**
     * 审核意见
     */
    private String auditView;

    /**
     * 推广状态(0：待发布;1：已发布;2：暂停推广;3：推广完成)
     */
    private Integer publishStatus;

    /**
     * 发布时间
     */
    private Date publishTime;

    private Date createTime;

    private Date modifyTime;

    /**
     * 0:已发布未推广 1：已发布已推广
     */
    private Integer isOnline;


}