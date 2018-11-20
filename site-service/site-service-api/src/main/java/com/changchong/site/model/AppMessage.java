package com.changchong.site.model;

import java.util.Date;

public class AppMessage {
    private Integer id;

    private String logo;

    private String title;

    private String body;

    private Boolean msgType;

    private Boolean pushType;

    private String destination;

    private Date createTime;

    private String createUser;

    private Date updateTime;

    private String updateUser;

    private Boolean isSend;

    private Date liveTime;

    private String delayTime;

    private String sendTerm;

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getLogo() {
        return logo;
    }

    public void setLogo(String logo) {
        this.logo = logo == null ? null : logo.trim();
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title == null ? null : title.trim();
    }

    public String getBody() {
        return body;
    }

    public void setBody(String body) {
        this.body = body == null ? null : body.trim();
    }

    public Boolean getMsgType() {
        return msgType;
    }

    public void setMsgType(Boolean msgType) {
        this.msgType = msgType;
    }

    public Boolean getPushType() {
        return pushType;
    }

    public void setPushType(Boolean pushType) {
        this.pushType = pushType;
    }

    public String getDestination() {
        return destination;
    }

    public void setDestination(String destination) {
        this.destination = destination == null ? null : destination.trim();
    }

    public Date getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Date createTime) {
        this.createTime = createTime;
    }

    public String getCreateUser() {
        return createUser;
    }

    public void setCreateUser(String createUser) {
        this.createUser = createUser == null ? null : createUser.trim();
    }

    public Date getUpdateTime() {
        return updateTime;
    }

    public void setUpdateTime(Date updateTime) {
        this.updateTime = updateTime;
    }

    public String getUpdateUser() {
        return updateUser;
    }

    public void setUpdateUser(String updateUser) {
        this.updateUser = updateUser == null ? null : updateUser.trim();
    }

    public Boolean getIsSend() {
        return isSend;
    }

    public void setIsSend(Boolean isSend) {
        this.isSend = isSend;
    }

    public Date getLiveTime() {
        return liveTime;
    }

    public void setLiveTime(Date liveTime) {
        this.liveTime = liveTime;
    }

    public String getDelayTime() {
        return delayTime;
    }

    public void setDelayTime(String delayTime) {
        this.delayTime = delayTime == null ? null : delayTime.trim();
    }

    public String getSendTerm() {
        return sendTerm;
    }

    public void setSendTerm(String sendTerm) {
        this.sendTerm = sendTerm == null ? null : sendTerm.trim();
    }
}