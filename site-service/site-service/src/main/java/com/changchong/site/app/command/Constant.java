package com.changchong.site.app.command;

public class Constant {

	/**
	 * 验证码
	 */
	public static enum VerCodeType {
		USER_LOGIN("登录验证码"),
        USER_REGISTER("注册验证码"),
        USER_CHANGE_ACCOUNT("变更手机号验证码"),
        USER_RESET_PASS("密码修改验证码");
        
        String appVerCodeType;
        VerCodeType(String appVerCodeType){
			this.appVerCodeType = appVerCodeType;
		}
		public String getAppVerCodeType() {
            return appVerCodeType;
        }
    }
	
	/**
	 * 举报类型
	 */
	public static enum ReportType {
        USER_REPORT("用户举报"),
        CUSTOMER_CENTER_REPORT("客户中心举报");  
        
        String reportType;
        ReportType(String reportType){
			this.reportType = reportType;
		}
		public String getReportType() {
            return reportType;
        }
    }
	
	/**
	 * 举报类别
	 */
	public static enum ReportCategory {   
		CHARGELINE_PORT_BROKEN("充电线接口坏了"),  
		CHARGELINE_BROKENSKIN("充电线破皮"),   
		CHARGELINE_NOT_POWERED("充电线不供电"),   
		SCREEN_DISTORTION("屏幕失真"), 
		SCREEN_IS_SCRATCHED("屏幕被刮花"), 
		OTHERS("其它");
        
        String reportCategory;
        ReportCategory(String reportCategory){
			this.reportCategory = reportCategory;
		}
		public String getReportCategory() {
            return reportCategory;
        }
    }
	
	
	/**
	 * 优惠券类型
	 */
	public static enum CouponType {
		LESS_FREE_VOUCHER("减免劵"),   
		FULLDOWN_COUPON("满减"),  
		FULLGIFT_COUPON("满赠"),  
		DISCOUNT_COUPONS("折扣券");  
        
        String couponType;
        CouponType(String couponType){
			this.couponType = couponType;
		}
		public String getCouponType() {
            return couponType;
        }
    }
	
	
}
