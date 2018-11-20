package com.changchong.site.pay.dto;

import lombok.Data;

import java.io.Serializable;
@Data
public class AdStatisticsDto implements Serializable {
    private Integer adId;
    private Integer count;
}
