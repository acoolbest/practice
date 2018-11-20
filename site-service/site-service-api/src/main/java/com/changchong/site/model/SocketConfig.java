package com.changchong.site.model;

import lombok.Data;

import java.io.Serializable;

@Data
public class SocketConfig implements Serializable {
    private Integer id;
    /**
     * SOCKET链接地址
     */
    private String microChargeUrl;
    /**
     * SOCKET链接端口
     */
    private String microChargePort;

}
