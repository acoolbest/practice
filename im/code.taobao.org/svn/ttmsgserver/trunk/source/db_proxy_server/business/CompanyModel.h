/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：CompanyModel.h
*   创 建 者：jiangzhicheng
*   邮    箱：jiangzhicheng@chinac.com
*   创建日期：2015年04月21日
*   描    述：
*
#pragma once
================================================================*/
#ifndef __COMPANYMODEL_H__
#define __COMPANYMODEL_H__

#include "ImPduBase.h"
#include "IM.BaseDefine.pb.h"

class CCompanytModel {
public:
	static CCompanytModel* getInstance();
	~CCompanytModel() {}
	void getChgedCompanyId(uint32_t	nUserId, uint32_t &nCompanyId);
	void getCompany(uint32_t nCompanyId, IM::BaseDefine::CompanyInfo& cCompany);
private:
	CCompanytModel(){};
private:
	static CCompanytModel* m_pInstance;
};

#endif /*defined(__DEPARTMODEL_H__) */
