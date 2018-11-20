/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：DepartModel.cpp
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月12日
*   描    述：
*
================================================================*/
#include "CompanyModel.h"
#include "../DBPool.h"

CCompanytModel* CCompanytModel::m_pInstance = NULL;

CCompanytModel* CCompanytModel::getInstance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CCompanytModel();
	}
	return m_pInstance;
}

void CCompanytModel::getChgedCompanyId(uint32_t	nUserId, uint32_t &nCompanyId)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select companyid from IMUser where id = " + int2string(nUserId);
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if(pResultSet)
		{
			while (pResultSet->Next()) 
			{
				nCompanyId = pResultSet->GetInt("companyid");
			}
			delete  pResultSet;
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave.");
	}
}

void CCompanytModel::getCompany(uint32_t nCompanyId, IM::BaseDefine::CompanyInfo& cCompany)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select * from IMCompany where id = " + int2string(nCompanyId);
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if(pResultSet)
		{
			while (pResultSet->Next())
			{
				if (pResultSet->GetInt("companyName") != NULL)
				{
					cCompany.set_company_name(pResultSet->GetString("companyName"));
				}
				else
				{
					cCompany.set_company_name("");
				}
			}
			delete  pResultSet;
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}
