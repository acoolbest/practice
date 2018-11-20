package com.changchong.sdk.constants;

/**
 * Created by cm on 2017/8/7.
 */
public class MarketEnum
{
	public enum MARKET{
		LIST("COM.CHANGCHONG.SITE.SERVICEIMPL.MARKETCONFIGLIST"),
		MULL("MULL");//数据库中为空
		private final String value;
		MARKET(String flag) {
			this.value =flag;
		}
		public final String getValue() {
			return value;
		}
	}

	public enum STATUS{
		DEL(new Byte("0")),//删除审核
		SAVE(new Byte("1")),//保存审核
		ONE_ADD(new Byte("2")), //提交审核
		ONE_NO_ACCEPT(new Byte("3")),//一审未通过
		ONE_ACCEPT(new Byte("4")),//一审通过
		TWO_NO_ACCEPT(new Byte("5")),//二审未通过
		TWO_ACCEPT(new Byte("6"));//二审通过
		private final byte value;
		STATUS(byte flag) {
			this.value = new Byte(flag);
		}
		public final byte getValue() {
			return value;
		}
	}
}
