package com.changchong.site.pay.dto;

import lombok.Data;

import java.io.Serializable;

@Data
public class SocketConfigDto implements Serializable {
    /**
     * SOCKET链接地址
     */
    private String microChargeUrl;
    /**
     * SOCKET链接端口
     */
    private String microChargePort;

    /**
     * 链接url和port
     */
    private String microUrlAndPort;


}
