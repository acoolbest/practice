package com.changchong.util;

import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;

import org.apache.log4j.Logger;
import org.bouncycastle.util.encoders.Base64;


/**
 * 加密，解密（3DES）
 * 主要用于登录密码的加解密
 * @author 
 * 
 */
public class EncryptUtils {

    static Logger logger = Logger.getLogger(EncryptUtils.class);
	/**
	 * 3des解码
	 * 
	 * @param value  待解密字符串
	 * @param key
	 * @return
	 * @throws Exception
	 */
    public static String Decrypt3DES(String value, String key) throws Exception {
        byte[] decValue = decryptMode(key.getBytes(), hexStr2ByteArr(value));
        return new String(decValue);
    }
    
    /**
     * 3des加密 
     * 
     * @param value 待加密字符串
     * @param key 原始密钥字符串
     * @return
     * @throws Exception
     */
    public static String Encrypt3DES(String value, String key) throws Exception {
    	byte[] encValue = byte2Base64(encryptMode(key.getBytes(), value.getBytes()));
        return new String(encValue);
    }
    
    private static final String Algorithm = "desede"; //定义 加密算法,可用 DES,DESede,Blowfish       

    /**
     * 加密
     * 
     * @param keybyte 加密密钥，长度为24字节
     * @param src 被加密的数据缓冲区（源）
     * @return
     * @throws Exception 
     */
    public static byte[] encryptMode(byte[] keybyte, byte[] src) throws Exception {
        //生成密钥
        SecretKey deskey = new SecretKeySpec(keybyte, Algorithm); //加密 
        Cipher c1 = Cipher.getInstance("desede" + "/ECB/PKCS5Padding");
        c1.init(Cipher.ENCRYPT_MODE, deskey);
        return c1.doFinal(src);
    }

    /**
     * 解码
     * 
     * @param keybyte 加密密钥，长度为24字节
     * @param src 加密后的缓冲区
     * @return
     */
    public static byte[] decryptMode(byte[] keybyte, byte[] src) throws Exception{
		//生成密钥   
        SecretKey deskey = new SecretKeySpec(keybyte, Algorithm);
        //解密     
        Cipher c1 = Cipher.getInstance("desede" + "/ECB/PKCS5Padding");
        c1.init(Cipher.DECRYPT_MODE, deskey);
        return c1.doFinal(src);
    }
    
   
    /**
    * 登陆密码3des加密
    * @param value
    * @return
    * @throws Exception
    */
   public static String PasswordEncrypt3DES(String value) throws Exception {
   	String key="8670a50254f431182a587cf3";
		byte[] encValue = byte2Base64(encryptMode(key.getBytes(), value.getBytes()));
		
       return new String(encValue);
   }
   /**
    * 登陆密码3des解密
    * @param value
    * @return
    * @throws Exception
    */
   public static String PasswordDecrypt3DES(String value) throws Exception {
   	String key="8670a50254f431182a587cf3";
       byte[] decValue = decryptMode(key.getBytes(), Base64.decode(value.replaceAll("\r|\n", "")));
       return new String(decValue);
   }
   public static void main(String[] args) {
   	try {
			String str = EncryptUtils.PasswordEncrypt3DES("123456");
			logger.info(str);
			String string = EncryptUtils.PasswordDecrypt3DES(str);
			logger.info(string);
		} catch (Exception e) {
			
			e.printStackTrace();
		}
  	}
    /**
     * 转换成base64编码
     * 
     * @param value
     * @return
     */
    public static byte[] byte2Base64(byte[] value) {
        return Base64.encode(value);
    }
    
    public  String byteArr2HexStr(byte[] arrB) throws Exception {
        int iLen = arrB.length;
        // 每个byte用两个字符才能表示，所以字符串的长度是数组长度的两倍..一个byte转成16进制最多不会超过两位。FF
        StringBuffer sb = new StringBuffer(iLen * 2);
        for (int i = 0; i < iLen; i++) {
            int intTmp = arrB[i];
            // 把负数转换为正数
            while (intTmp < 0) {
                intTmp = intTmp + 256;
            }
            // 小于0F的数需要在前面补0
            if (intTmp < 16) {
                sb.append("0");
            }
            sb.append(Integer.toString(intTmp, 16)); // 16代表进制
        }
        return sb.toString();
    }

    /**
     * 将表示16进制值的字符串转换为byte数组， 和public static String byteArr2HexStr(byte[] arrB)
     * 互为可逆的转换过程
     * 
     * @param strIn
     *            需要转换的字符串
     * @return 转换后的byte数组
     * @throws Exception
     *             本方法不处理任何异常，所有异常全部抛出
     * @author <a href="mailto:leo841001@163.com">LiGuoQing</a>
     */
    public static byte[] hexStr2ByteArr(String strIn) throws Exception {
        byte[] arrB = strIn.getBytes();
        int iLen = arrB.length;

        // 两个字符表示一个字节，所以字节数组长度是字符串长度除以2
        byte[] arrOut = new byte[iLen / 2];
        for (int i = 0; i < iLen; i = i + 2) {
            String strTmp = new String(arrB, i, 2);
            arrOut[i / 2] = (byte) Integer.parseInt(strTmp, 16);
        }
        return arrOut;
    }

}
