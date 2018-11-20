package com.changchong.site.model;

import java.util.Date;

/**
 * Created by cm on 2017/5/22.
 */
public class DeviceTroubleDetail {
    private Integer id;
    private Integer user_id;
    private String phone;
    private String device_code;
    private String trouble_id;
    private String trouble_picture;
    private String trouble_detail;
    private String province;
    private String city;
    private String address;
    private Byte status;
    private String remark;
    private Date create_time;
    private Integer update_user;
    private Date update_time;

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public Integer getUser_id() {
        return user_id;
    }

    public void setUser_id(Integer user_id) {
        this.user_id = user_id;
    }

    public String getPhone() {
        return phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

    public String getDevice_code() {
        return device_code;
    }

    public void setDevice_code(String device_code) {
        this.device_code = device_code;
    }

    public String getTrouble_id() {
        return trouble_id;
    }

    public void setTrouble_id(String trouble_id) {
        this.trouble_id = trouble_id;
    }

    public String getTrouble_picture() {
        return trouble_picture;
    }

    public void setTrouble_picture(String trouble_picture) {
        this.trouble_picture = trouble_picture;
    }

    public String getTrouble_detail() {
        return trouble_detail;
    }

    public void setTrouble_detail(String trouble_detail) {
        this.trouble_detail = trouble_detail;
    }

    public String getProvince() {
        return province;
    }

    public void setProvince(String province) {
        this.province = province;
    }

    public String getCity() {
        return city;
    }

    public void setCity(String city) {
        this.city = city;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }

    public Byte getStatus() {
        return status;
    }

    public void setStatus(Byte status) {
        this.status = status;
    }

    public String getRemark() {
        return remark;
    }

    public void setRemark(String remark) {
        this.remark = remark;
    }

    public Date getCreate_time() {
        return create_time;
    }

    public void setCreate_time(Date create_time) {
        this.create_time = create_time;
    }

    public Integer getUpdate_user() {
        return update_user;
    }

    public void setUpdate_user(Integer update_user) {
        this.update_user = update_user;
    }

    public Date getUpdate_time() {
        return update_time;
    }

    public void setUpdate_time(Date update_time) {
        this.update_time = update_time;
    }
}
