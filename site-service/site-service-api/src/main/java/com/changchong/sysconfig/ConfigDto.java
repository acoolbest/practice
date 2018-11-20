package com.changchong.sysconfig;

import java.io.Serializable;


/**
 * Created by cm on 2017/8/8.
 */
public class ConfigDto implements Serializable
{
	private Integer id;
	private String project;
	private String config;
	private Byte env_type;

	public Integer getId()
	{
		return id;
	}

	public void setId(Integer id)
	{
		this.id = id;
	}

	public String getProject()
	{
		return project;
	}

	public void setProject(String project)
	{
		this.project = project;
	}


	public String getConfig()
	{
		return config;
	}

	public void setConfig(String config)
	{
		this.config = config;
	}

	public Byte getEnv_type()
	{
		return env_type;
	}

	public void setEnv_type(Byte env_type)
	{
		this.env_type = env_type;
	}
}
