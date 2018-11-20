package com.changchong.site.app.dto;

import java.io.Serializable;

import lombok.Data;

/**
 * Created by zhangyaohai on 2017/7/24.
 */

@Data
public class SiteDTO implements Serializable {
    private String payableCfgJson;
    private int id;
}
