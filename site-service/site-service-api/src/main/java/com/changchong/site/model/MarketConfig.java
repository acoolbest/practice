package com.changchong.site.model;

import java.util.Date;

/**
 * Created by cm on 2017/8/7.
 */
public class MarketConfig
{
	private Integer id;
	private String market_price;
	private Byte status;
	private String market_code;
	private Date effect_time;
	private Byte sys_type;
	private Integer user_id;
	private Date update_time;
	private Date create_time;
	private String future;

	public Integer getId()
	{
		return id;
	}

	public void setId(Integer id)
	{
		this.id = id;
	}

	public String getMarket_price()
	{
		return market_price;
	}

	public void setMarket_price(String market_price)
	{
		this.market_price = market_price;
	}

	public Byte getStatus()
	{
		return status;
	}

	public void setStatus(Byte status)
	{
		this.status = status;
	}

	public String getMarket_code()
	{
		return market_code;
	}

	public void setMarket_code(String market_code)
	{
		this.market_code = market_code;
	}

	public Date getEffect_time()
	{
		return effect_time;
	}

	public void setEffect_time(Date effect_time)
	{
		this.effect_time = effect_time;
	}

	public Byte getSys_type()
	{
		return sys_type;
	}

	public void setSys_type(Byte sys_type)
	{
		this.sys_type = sys_type;
	}

	public Integer getUser_id()
	{
		return user_id;
	}

	public void setUser_id(Integer user_id)
	{
		this.user_id = user_id;
	}

	public Date getUpdate_time()
	{
		return update_time;
	}

	public void setUpdate_time(Date update_time)
	{
		this.update_time = update_time;
	}

	public Date getCreate_time()
	{
		return create_time;
	}

	public void setCreate_time(Date create_time)
	{
		this.create_time = create_time;
	}

	public String getFuture()
	{
		return future;
	}

	public void setFuture(String future)
	{
		this.future = future;
	}
}
