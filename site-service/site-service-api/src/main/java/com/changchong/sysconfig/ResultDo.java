package com.changchong.sysconfig;



import java.io.Serializable;


public class ResultDo<T> implements Serializable
{

	private static final long serialVersionUID = 4455702538105064491L;

    private int ret =0;

    private String errcode;

    protected String errinfo;

	private T result;
	
	public ResultDo(){}

	public ResultDo(String errcode, String errinfo) {
	    this.errcode = errcode;
	    this.errinfo = errinfo;
	}

	public int getRet() {
		return ret;
	}

	public void setRet(int ret) {
		this.ret = ret;
	}

	public String getErrcode() {
		return errcode;
	}

	public void setErrcode(String errcode) {
		this.errcode = errcode;
	}

	public String getErrinfo() {
		return errinfo;
	}

	public void setErrinfo(String errinfo) {
		this.errinfo = errinfo;
	}

	public T getResult() {
		return result;
	}

	public void setResult(T result) {
		this.result = result;
	}
   
}
