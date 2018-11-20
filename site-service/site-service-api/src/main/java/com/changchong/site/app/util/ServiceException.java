package com.changchong.site.app.util;

import org.apache.commons.lang.builder.EqualsBuilder;

public class ServiceException extends RuntimeException {

    private String code;

    private Object value;

    public ServiceException(String code) {
        super(code);
        this.code = code;
    }

    public ServiceException(String code, String message) {
        super(message);
        this.code = code;
    }

    public ServiceException(String code, Object value) {
        this.code = code;
        this.value = value;
    }

    public ServiceException(String code, String message, Exception e) {
        super(message, e);
        this.code = code;
    }

    public Object getValue() {
        return value;
    }

    public String getCode() {
        return code;
    }

    @Override
    public boolean equals(final Object obj) {
        if (obj instanceof ServiceException) {
            final ServiceException other = (ServiceException) obj;
            return new EqualsBuilder()
                    .append(code, other.getCode())
                    .append(getMessage(), other.getMessage())
                    .isEquals();
        } else {
            return false;
        }
    }
}
