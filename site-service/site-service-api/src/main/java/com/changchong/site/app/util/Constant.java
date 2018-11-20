package com.changchong.site.app.util;

public class Constant {

	/**
	 * 验证码
	 */
	public static enum VerCodeType {
        USER_REGISTER("注册验证码"),
        USER_RESET_PASS("密码修改验证码");
        
        String appVerCodeType;
        VerCodeType(String appVerCodeType){
			this.appVerCodeType = appVerCodeType;
		}
		public String getAppVerCodeType() {
            return appVerCodeType;
        }
    }
	
	public static enum SysPropertyType {
    	VER_CODE_TIME, //验证码失效时间
    	CALL_CENTER_MOBILE, //客服电话
    	ORDER_TIMEOUT, //系统订单超时时间
    	ORDER_PAYPARAMS_TIMEOUT; //第三方支付时间过期
    }
	
	
}
