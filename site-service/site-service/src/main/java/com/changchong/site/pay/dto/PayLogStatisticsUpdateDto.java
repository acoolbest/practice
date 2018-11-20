package com.changchong.site.pay.dto;

import java.io.Serializable;

public class PayLogStatisticsUpdateDto implements Serializable {

	private static final long serialVersionUID = 1L;
	private String sn;//支付成功订单编号
	private Byte status;//订单状态 1-支付成功 0-等待 2-申请退款 3-已退款 4-未知 9-取消  -1 支付失败'
	public String getSn() {
		return sn;
	}
	public void setSn(String sn) {
		this.sn = sn;
	}
	public Byte getStatus() {
		return status;
	}
	public void setStatus(Byte status) {
		this.status = status;
	}
}
