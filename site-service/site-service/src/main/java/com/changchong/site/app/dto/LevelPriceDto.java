package com.changchong.site.app.dto;

import java.io.Serializable;


public class LevelPriceDto implements Serializable
{
	
	private static final long serialVersionUID = 1L;
	private String original_price;//定价
	private String present_price;//实际价格
	
	public String getOriginal_price()
	{
		return original_price;
	}
	
	public void setOriginal_price(String original_price)
	{
		this.original_price = original_price;
	}
	
	public String getPresent_price()
	{
		return present_price;
	}
	
	public void setPresent_price(String present_price)
	{
		this.present_price = present_price;
	}
}
