package com.changchong.site.app.util;

public class ErrorCode {

    public static final String SYSTEM_ERROR = "999999";
    
    public static final String SECURITY_ERROR = "999998";
    
    public static final String FILE_UPLOAD_ERROR = "999997";

    public static final String USER_NOT_FOUND = "000400";

    public static final String INVALID_APPTYPE = "000402";

    public static final String UNAVAILABLE_USER = "000501";
    
    public static final String SIGN_ERROR = "000502";

    public static final String INVALID_LOGIN_TOKEN = "000504";

    public static final String USER_NOT_LOGIN = "000505";
    
    public static final String USER_LEVEL_NOT_RIGHT = "000506";

    public static final String OPTIMISTIC_LOCK_FAILED = "900000";

    public static final String EMPTY_VER_CODE = "000600";

    public static final String INCORRECT_VER_CODE = "000601";
    
    public static final String UPDATE_APP = "000602";

    public static final String INVALID_PARAM = "000700";

    public static final String NO_SYS_PROPERTY = "000701";
    
    public static final String HAS_NOT_SUPPORT_WORD = "000702";
    
    public static final String HAS_NOT_SUPPORT_SENSITIVE_WORD = "000704";
    
    public static final String USER_NOT_BIND_RENTROOM = "000800";
    public static final String MOBILE_IS_REGISTER = "000801";

    public static final String EMPLOYEE_NOT_BIND_SPACE = "000900";
    public static final String EMPLOYEE_NOT_BIND_SVC = "000901";


    public static final String VAL_ERROR = "010000"; //{0}格式错误
    public static final String MIN_LENGTH = "010001"; //{0}非法,最少为{1}位
    public static final String MAX_LENGTH = "010002"; //{0}非法,最多为{1}位
}
