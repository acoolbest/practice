package com.changchong.site.model;

import lombok.Data;

import java.io.Serializable;

/**
 * @author 
 */
@Data
public class AdStatistics implements Serializable {
    private Integer id;

    /**
     * 广告ID
     */
    private Integer adId;

    /**
     * 观看次数
     */
    private Integer count;

}