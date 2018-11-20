package com.changchong.util;

import org.apache.log4j.Logger;

import java.math.BigDecimal;

public class MathUtil {

    static Logger logger = Logger.getLogger(MathUtil.class);

    // 进行加法运算
    public static double add(double d1, double d2){        
        BigDecimal b1 = new BigDecimal(d1);
        BigDecimal b2 = new BigDecimal(d2);
        return b1.add(b2).doubleValue();
    }
    // 进行减法运算
    public static double sub(double d1, double d2){        
        BigDecimal b1 = new BigDecimal(d1);
        BigDecimal b2 = new BigDecimal(d2);
        return b1.subtract(b2).doubleValue();
    }
    // 进行乘法运算
    public static double mul(double d1, double d2){        
        BigDecimal b1 = new BigDecimal(d1);
        BigDecimal b2 = new BigDecimal(d2);
        return b1.multiply(b2).doubleValue();
    }
    // 进行除法运算
    public static double div(double d1,double d2,int len) {
        BigDecimal b1 = new BigDecimal(d1);
        BigDecimal b2 = new BigDecimal(d2);
        return b1.divide(b2,len,BigDecimal.ROUND_HALF_UP).doubleValue();
    }
    // 进行四舍五入操作
    public static double round(double d,int len) {     
        BigDecimal b1 = new BigDecimal(d);
        BigDecimal b2 = new BigDecimal(1);
        // 任何一个数字除以1都是原数字
        // ROUND_HALF_UP是BigDecimal的一个常量，表示进行四舍五入的操作
        return b1.divide(b2,len,BigDecimal.ROUND_HALF_UP).doubleValue();
    }
    // 进行四舍五入操作
    public static String roundString(double d,int len) {     
        BigDecimal b1 = new BigDecimal(d);
        BigDecimal b2 = new BigDecimal(1);
        // 任何一个数字除以1都是原数字
        // ROUND_HALF_UP是BigDecimal的一个常量，表示进行四舍五入的操作
        return b1.divide(b2,len,BigDecimal.ROUND_HALF_UP).toString();
    }
    //测试
    public static void main(String[] args) {
         logger.info("加法运算：" +
        		 MathUtil.roundString(MathUtil.add(10.345,3.333), 0));
         logger.info("乘法运算：" +
        		 MathUtil.round(MathUtil.mul(10.345,3.333), 3));
         logger.info("除法运算：" +
        		 MathUtil.div(10.345, 3.333, 3));
         logger.info("减法运算：" +
        		 MathUtil.round(MathUtil.sub(10.345,3.333), 3));
     }
}