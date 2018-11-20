package com.changchong.site.model;

import lombok.Data;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.Date;

/**
 * @author 
 */
@Data
public class PortLog implements Serializable {
    private Integer id;

    /**
     * 端口表ID
     */
    private Integer portId;

    /**
     * 当前电流大小ID
     */
    private BigDecimal currentElectric;

    /**
     * 上传端口电流信息时间
     */
    private Date createTime;

}