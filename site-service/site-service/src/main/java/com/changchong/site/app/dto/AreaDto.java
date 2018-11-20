package com.changchong.site.app.dto;

import java.io.Serializable;

/**
 * Created by cm on 2017/5/19.
 */
public class AreaDto implements Serializable{
    private Integer id;
    private String name;
    private Integer pid;


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

    public Integer getPid() {
        return pid;
    }

    public void setPid(Integer pid) {
        this.pid = pid;
    }


}
