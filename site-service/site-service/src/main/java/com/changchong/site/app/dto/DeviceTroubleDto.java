package com.changchong.site.app.dto;

import java.io.Serializable;

/**
 * Created by cm on 2017/5/23.
 */
public class DeviceTroubleDto implements Serializable {
    private Integer id;
    private String name;

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
