package com.changchong.site.model;

import java.util.Date;

/**
 * Created by cm on 2017/5/19.
 */
public class AppReport {
    private Integer id;
    private Integer user_id;
    private String report_detail;
    private String report_picture;
    private String province;
    private String city;
    private String address;
    private Byte status;
    private String  remark;
    private Date create_time;
    private Integer update_user;
    private Date update_time;
    private String phone;

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

    public String getReport_detail() {
        return report_detail;
    }

    public void setReport_detail(String report_detail) {
        this.report_detail = report_detail;
    }


    public String getReport_picture() {
        return report_picture;
    }

    public void setReport_picture(String report_picture) {
        this.report_picture = report_picture;
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

    public String getPhone() {
        return phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }
}