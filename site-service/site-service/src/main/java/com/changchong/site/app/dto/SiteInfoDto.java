package com.changchong.site.app.dto;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.Date;

public class SiteInfoDto implements Serializable{

	private Integer id;
    private String name;
	private String developer;
	private String contract_begin;
	private String contract_end;
	private Byte open_type;
	private String open_start_time;
	private String open_end_time;
	private Integer province_id;
	private String province_name;
	private Integer district_id;
	private String district_name;
	private Integer company_id;
	private Integer site_type_id;
	private Byte features;
	private Integer surfaceman;
	private Byte surfaceman_send;
	private Integer director;
	private Byte director_send;
	private Integer inspection;
	private Byte inspection_send;
	private String location_describe;
	private String ad_version;
	private String payable_cfg_json;

	public Integer getId()
	{
		return id;
	}

	public void setId(Integer id)
	{
		this.id = id;
	}

	public String getName()
	{
		return name;
	}

	public void setName(String name)
	{
		this.name = name;
	}

	public String getDeveloper()
	{
		return developer;
	}

	public void setDeveloper(String developer)
	{
		this.developer = developer;
	}

	public String getContract_begin()
	{
		return contract_begin;
	}

	public void setContract_begin(String contract_begin)
	{
		this.contract_begin = contract_begin;
	}

	public String getContract_end()
	{
		return contract_end;
	}

	public void setContract_end(String contract_end)
	{
		this.contract_end = contract_end;
	}

	public Byte getOpen_type()
	{
		return open_type;
	}

	public void setOpen_type(Byte open_type)
	{
		this.open_type = open_type;
	}

	public String getOpen_start_time()
	{
		return open_start_time;
	}

	public void setOpen_start_time(String open_start_time)
	{
		this.open_start_time = open_start_time;
	}

	public String getOpen_end_time()
	{
		return open_end_time;
	}

	public void setOpen_end_time(String open_end_time)
	{
		this.open_end_time = open_end_time;
	}

	public Integer getProvince_id()
	{
		return province_id;
	}

	public void setProvince_id(Integer province_id)
	{
		this.province_id = province_id;
	}

	public String getProvince_name()
	{
		return province_name;
	}

	public void setProvince_name(String province_name)
	{
		this.province_name = province_name;
	}

	public Integer getDistrict_id()
	{
		return district_id;
	}

	public void setDistrict_id(Integer district_id)
	{
		this.district_id = district_id;
	}

	public String getDistrict_name()
	{
		return district_name;
	}

	public void setDistrict_name(String district_name)
	{
		this.district_name = district_name;
	}

	public Integer getCompany_id()
	{
		return company_id;
	}

	public void setCompany_id(Integer company_id)
	{
		this.company_id = company_id;
	}

	public Integer getSite_type_id()
	{
		return site_type_id;
	}

	public void setSite_type_id(Integer site_type_id)
	{
		this.site_type_id = site_type_id;
	}

	public Byte getFeatures()
	{
		return features;
	}

	public void setFeatures(Byte features)
	{
		this.features = features;
	}

	public Integer getSurfaceman()
	{
		return surfaceman;
	}

	public void setSurfaceman(Integer surfaceman)
	{
		this.surfaceman = surfaceman;
	}

	public Byte getSurfaceman_send()
	{
		return surfaceman_send;
	}

	public void setSurfaceman_send(Byte surfaceman_send)
	{
		this.surfaceman_send = surfaceman_send;
	}

	public Integer getDirector()
	{
		return director;
	}

	public void setDirector(Integer director)
	{
		this.director = director;
	}

	public Byte getDirector_send()
	{
		return director_send;
	}

	public void setDirector_send(Byte director_send)
	{
		this.director_send = director_send;
	}

	public Integer getInspection()
	{
		return inspection;
	}

	public void setInspection(Integer inspection)
	{
		this.inspection = inspection;
	}

	public Byte getInspection_send()
	{
		return inspection_send;
	}

	public void setInspection_send(Byte inspection_send)
	{
		this.inspection_send = inspection_send;
	}

	public String getLocation_describe()
	{
		return location_describe;
	}

	public void setLocation_describe(String location_describe)
	{
		this.location_describe = location_describe;
	}

	public String getAd_version()
	{
		return ad_version;
	}

	public void setAd_version(String ad_version)
	{
		this.ad_version = ad_version;
	}

	public String getPayable_cfg_json()
	{
		return payable_cfg_json;
	}

	public void setPayable_cfg_json(String payable_cfg_json)
	{
		this.payable_cfg_json = payable_cfg_json;
	}
}