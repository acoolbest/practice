/*================================================================
*     Copyright (c) 2015�� lanhu. All rights reserved.
*   
*   �ļ����ƣ�CompanyModel.h
*   �� �� �ߣ�jiangzhicheng
*   ��    �䣺jiangzhicheng@chinac.com
*   �������ڣ�2015��04��21��
*   ��    ����
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
