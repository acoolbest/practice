package com.changchong.site.app.util;

public class Constants {

	public static final String REDIS_NULL_VALUE = "1";

	public static final Long REDIS_TIME_OUT = 60l;

	public static final String EMPTY = "empty:";
	
	public static final String FORCE_LOGIN_OUT_KEY = "force_loginout:";

	/**
	 * 存储用户Id
	 */
	public static final String USER_ID = "userId:detail:";
	
	public static final String DEVICE_ID = "deviceId:detail:";
	
	public static final String USER_DEVICE_ID = "userId:devices:";
	
	public static final String USER_ACCESS_ID = "userAccessId:detail:";
	
	public static final String USER_DEFAULT_ID = "userDefaultId:detail:";
	
	public static final String USER_TIMEKEEPER_ID = "userTimeKeeperId:detail:";
	
	public static final String USER_CERT_ID = "userCertId:detail:";
	
	public static final String COMPANY_ALIPAY_ACCOUNT_ID = "companyAlipayAccountId:detail:";
	
	public static final String COMPANY_ALIPAY_SELLER = "companyAlipaySeller:detail:";
	
	public static final String COMPANY_WECHAT_ACCOUNT_ID = "companyWechatAccountId:detail:";
	
	public static final String COMPANY_WECHAT_PARTERID = "companyParterId:detail:";
	
	public static final String QRCODE_ID = "qrcode:detail:";

	public static final String USER_DYNAMIC_ID = "userDynamicId:detail:";
	
	public static final String NEW_TOPIC_MESSAGE = "newTopicMessage:detail:";
	
	/**
	 * 存放用户登录token key
	 */
	public static final String CONSUMER_LOGIN_TOKEN = "consumerLoginToken:";

    /**
     * 存放验证码
     */
    public static final String VER_CODE = "verCode:";
    
    public static final String LUA_INSERT_C_DEVICE = "lua:c:device";
    
}
