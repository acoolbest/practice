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
#include "DepartModel.h"
#include "../DBPool.h"

CDepartModel* CDepartModel::m_pInstance = NULL;

CDepartModel* CDepartModel::getInstance()
{
    if(NULL == m_pInstance)
    {
        m_pInstance = new CDepartModel();
    }
    return m_pInstance;
}

void CDepartModel::getCompanyId(uint32_t nUserId,uint32_t& nCompanyId)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select companyid from IMUser  where id = " + int2string(nUserId);
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		log("SQL for getting companyid = %s",strSql.c_str());
		if(pResultSet)
		{
			while (pResultSet->Next()) 
			{
				nCompanyId = pResultSet->GetInt("companyid");
				log("GetCompany,CompanyID:%d", nCompanyId);
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


void CDepartModel::getChgedDeptId(uint32_t nId,uint32_t& nLastTime, list<uint32_t>& lsChangedIds)
{
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
    if (pDBConn)
    {
		uint32_t nCompanyId = 0;
		getCompanyId(nId,nCompanyId);
		string strSql;
		if (nLastTime == 0)
		{
			strSql = "select id, updated from IMDepart where companyid = " + int2string(nCompanyId) + " and status = 0";
		}
		else
		{
			strSql = "select id, updated from IMDepart where companyid = " + int2string(nCompanyId);
			strSql = strSql + " and updated > " + int2string(nLastTime);
		}

		log("getChgedDeptId,select sql = %s",strSql.c_str());
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            while (pResultSet->Next()) 
			{
                uint32_t id = pResultSet->GetInt("id");
                uint32_t nUpdated = pResultSet->GetInt("updated");
                if(nLastTime < nUpdated)
                {
                    nLastTime = nUpdated;
                }
                lsChangedIds.push_back(id);
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

void CDepartModel::getDepts(list<uint32_t>& lsDeptIds, list<IM::BaseDefine::DepartInfo>& lsDepts)
{
    if(lsDeptIds.empty())
    {
        log("list is empty");
        return;
    }
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
    if (pDBConn)
    {
        string strClause;
        bool bFirst = true;
        for (auto it=lsDeptIds.begin(); it!=lsDeptIds.end(); ++it) {
            if(bFirst)
            {
                bFirst = false;
                strClause += int2string(*it);
            }
            else
            {
                strClause += ("," + int2string(*it));
            }
        }
        string strSql = "select * from IMDepart where id in ( " + strClause + " )"; 
		log("SQL from getting departments: %s", strSql.c_str());
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            while (pResultSet->Next())
			{
                IM::BaseDefine::DepartInfo cDept;

                string strDepartID = pResultSet->GetString("uuid");
				uint32_t nCompanyId = pResultSet->GetInt("companyid");
                string strParentId = pResultSet->GetString("parentuuid");
                string strDeptName = pResultSet->GetString("departname");
                int32_t nStatus = pResultSet->GetInt("status");
				uint32_t unContactStatus = pResultSet->GetInt("contactstatus");
                uint32_t nPriority = pResultSet->GetInt("priority");
				uint32_t nLevel = pResultSet->GetInt("level");
                //if(IM::BaseDefine::DepartmentStatusType_IsValid(nStatus))
                {
                    cDept.set_dept_uuid(strDepartID);
					cDept.set_company_id(nCompanyId);
                    cDept.set_parent_dept_uuid(strParentId);
                    cDept.set_dept_name(strDeptName);
                    cDept.set_dept_status(nStatus);
                    cDept.set_priority(nPriority);
					cDept.set_dept_level(nLevel);
					cDept.set_contact_status(unContactStatus);
                    lsDepts.push_back(cDept);
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


void CDepartModel::getDept(uint32_t nDeptId, IM::BaseDefine::DepartInfo& cDept)
{
    CDBManager* pDBManager = CDBManager::getInstance();
    CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
    if (pDBConn)
    {
        string strSql = "select * from IMDepart where status = 0 and id = " + int2string(nDeptId);
        CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
        if(pResultSet)
        {
            while (pResultSet->Next()) {
                string strId = pResultSet->GetString("uuid");
				string strParentId = "";
				if (pResultSet->GetString("parentuuid") != NULL)
				{
					strParentId = pResultSet->GetString("parentuuid");
				}
                string strDeptName = pResultSet->GetString("departName");
                uint32_t nStatus = pResultSet->GetInt("status");
                uint32_t nPriority = pResultSet->GetInt("priority");
				uint32_t nLevel = pResultSet->GetInt("level");
                if(IM::BaseDefine::DepartmentStatusType_IsValid(nStatus))
                {
                    cDept.set_dept_uuid(strId);
                    cDept.set_parent_dept_uuid(strParentId);
                    cDept.set_dept_name(strDeptName);
                    cDept.set_dept_status(nStatus);
                    cDept.set_priority(nPriority);
					cDept.set_dept_level(nLevel);
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


void CDepartModel::getCompanyId(string& strUserID, uint32_t& nCompanyId)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select companyid from IMUserPosition where useruuid = '" + strUserID + "'";
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		log("CDepartModel::getCompanyId,select sql = %s", strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nCompanyId = pResultSet->GetInt("companyid");
				log("GetCompany,CompanyID:%d", nCompanyId);
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


void CDepartModel::getChgedDeptId(string& strUserID, uint32_t& nLastTime, list<string>& lsChangedIds)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "";

		if (nLastTime == 0)
		{
			strSql = "select a.uuid as duuid, a.updated as dupdated from IMDepart a, IMUser b where a.companyid = b.companyid and b.useruuid='" + strUserID + "' and a.status = 0 and b.status = 0";
		}
		else
		{
			strSql = "select a.uuid as duuid, a.updated as dupdated from IMDepart a, IMUser b where a.companyid = b.companyid and b.useruuid='" + strUserID + "' and updated > " + int2string(nLastTime);
			//strSql = "select uuid, updated from IMDepart where companyid = " + int2string(nCompanyId) + " and updated > " + int2string(nLastTime);
		}

		log("getChgedDeptId,select sql = %s", strSql.c_str());
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				string id = pResultSet->GetString("duuid");
				uint32_t nUpdated = pResultSet->GetInt("dupdated");
				if (nLastTime < nUpdated)
				{
					nLastTime = nUpdated;
				}
				lsChangedIds.push_back(id);
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


void CDepartModel::getDepts(list<string>& lsDeptIds, list<IM::BaseDefine::DepartInfo>& lsDepts)
{
	if (lsDeptIds.empty())
	{
		log("list is empty");
		return;
	}
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strClause;
		bool bFirst = true;
		for (auto it = lsDeptIds.begin(); it != lsDeptIds.end(); ++it) {
			if (bFirst)
			{
				bFirst = false;
				strClause += "'" + (*it) + "'";
			}
			else
			{
				strClause += (",'" + (*it) + "'");
			}
		}
		string strSql = "select * from IMDepart where uuid in ( " + strClause + " )";
		log("SQL from getting departments: %s", strSql.c_str());
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				IM::BaseDefine::DepartInfo cDept;

				string strDepartID = pResultSet->GetString("uuid");
				uint32_t nCompanyId = pResultSet->GetInt("companyid");
				string strParentId = "";
				if (pResultSet->GetString("parentuuid") != NULL)
				{
					strParentId = pResultSet->GetString("parentuuid");
				}
				string strDeptName = pResultSet->GetString("departname");
				int32_t nStatus = pResultSet->GetInt("status");
				uint32_t unContactStatus = pResultSet->GetInt("contactstatus");
				uint32_t nPriority = pResultSet->GetInt("priority");
				uint32_t nLevel = pResultSet->GetInt("level");
				//if(IM::BaseDefine::DepartmentStatusType_IsValid(nStatus))
				{
					cDept.set_dept_uuid(strDepartID);
					cDept.set_company_id(nCompanyId);
					cDept.set_parent_dept_uuid(strParentId);
					cDept.set_dept_name(strDeptName);
					cDept.set_dept_status(nStatus);
					cDept.set_priority(nPriority);
					cDept.set_dept_level(nLevel);
					cDept.set_contact_status(unContactStatus);
					lsDepts.push_back(cDept);
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

void CDepartModel::getDept(string& strDeptId, IM::BaseDefine::DepartInfo& cDept)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select * from IMDepart where status = 0 and uuid = '" + strDeptId + "'";
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				string strDepartID = pResultSet->GetString("uuid");
				uint32_t nCompanyId = pResultSet->GetInt("companyid");
				string strParentId = "";
				if (pResultSet->GetString("parentuuid") != NULL)
				{
					strParentId = pResultSet->GetString("parentuuid");
				}
				string strDeptName = pResultSet->GetString("departname");
				int32_t nStatus = pResultSet->GetInt("status");
				uint32_t unContactStatus = pResultSet->GetInt("contactstatus");
				uint32_t nPriority = pResultSet->GetInt("priority");
				uint32_t nLevel = pResultSet->GetInt("level");
				//if(IM::BaseDefine::DepartmentStatusType_IsValid(nStatus))
				{
					cDept.set_dept_uuid(strDepartID);
					cDept.set_company_id(nCompanyId);
					cDept.set_parent_dept_uuid(strParentId);
					cDept.set_dept_name(strDeptName);
					cDept.set_dept_status(nStatus);
					cDept.set_priority(nPriority);
					cDept.set_dept_level(nLevel);
					cDept.set_contact_status(unContactStatus);
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


void CDepartModel::getChgedDeptId(uint32_t nId, uint32_t& nLastTime, list<string>& lsChangedIds)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		uint32_t nCompanyId = 0;
		getCompanyId(nId, nCompanyId);
		string strSql;
		if (nLastTime == 0)
		{
			strSql = "select uuid, updated from IMDepart  where companyid = " + int2string(nCompanyId) + " and status = 0";
		}
		else
		{
			strSql = "select uuid, updated from IMDepart where companyid = " + int2string(nCompanyId) + " and updated > " + int2string(nLastTime);
		}

		log("getChgedDeptId,select sql = %s", strSql.c_str());
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				string id = pResultSet->GetString("uuid");
				uint32_t nUpdated = pResultSet->GetInt("updated");
				if (nLastTime < nUpdated)
				{
					nLastTime = nUpdated;
				}
				lsChangedIds.push_back(id);
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


void CDepartModel::getDepts(uint32_t unCompanyId, uint32_t& nLastTime, list<IM::BaseDefine::DepartInfo>& lsDepts)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql;
		if (nLastTime == 0)
		{
			strSql = "select * from IMDepart where companyid= " + int2string(unCompanyId) + " and status = 0";
		}
		else
		{
			strSql = "select * from IMDepart where companyid = " + int2string(unCompanyId) + " and updated > " + int2string(nLastTime);
		}

		log("getChgedDepts,select sql = %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				uint32_t nUpdated = pResultSet->GetInt("updated");
				if (nLastTime < nUpdated)
				{
					nLastTime = nUpdated;
				}
	
				IM::BaseDefine::DepartInfo cDept;

				string strDepartID = pResultSet->GetString("uuid");
				//uint32_t nCompanyId = pResultSet->GetInt("companyid");
				string strParentId = "";
				if (pResultSet->GetString("parentuuid") != NULL)
				{
					strParentId = pResultSet->GetString("parentuuid");
				}
				string strDeptName = pResultSet->GetString("departname");
				int32_t nStatus = pResultSet->GetInt("status");
				uint32_t unContactStatus = pResultSet->GetInt("contactstatus");
				uint32_t nPriority = pResultSet->GetInt("priority");
				uint32_t nLevel = pResultSet->GetInt("level");
			
				cDept.set_dept_uuid(strDepartID);
				cDept.set_company_id(unCompanyId);
				cDept.set_parent_dept_uuid(strParentId);
				cDept.set_dept_name(strDeptName);
				cDept.set_dept_status(nStatus);
				cDept.set_priority(nPriority);
				cDept.set_dept_level(nLevel);
				cDept.set_contact_status(unContactStatus);
				lsDepts.push_back(cDept);
			}
			delete  pResultSet;
			pResultSet = NULL;
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}


//void CDepartModel::GetDepartments(uint32_t unUserId, uint32_t& nLastTime, list<IM::BaseDefine::DepartInfo>& lsDepts)
//{
//	CDBManager* pDBManager = CDBManager::getInstance();
//	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
//	if (pDBConn)
//	{
//		string strTempTable = "(select companyid from IMStaff where userid=" + int2string(unUserId) + " and status=1) as ";
//		string strSql = "select a.*,c.companyuuid from IMDepart a, IMCompany c where a.companyid = c.id and exists(select 1 from " + strTempTable +" b where a.companyid=b.companyid) ";
//
//		if (nLastTime == 0)
//		{
//			strSql +=  " and a.status = 0";
//		}
//		else
//		{
//			strSql += " and a.updated > " + int2string(nLastTime);
//		}
//
//		log("SQL for getting changed departments: %s", strSql.c_str());
//
//		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
//		if (pResultSet)
//		{
//			while (pResultSet->Next())
//			{
//				uint32_t nUpdated = pResultSet->GetInt("updated");
//				if (nLastTime < nUpdated)
//				{
//					nLastTime = nUpdated;
//				}
//
//				IM::BaseDefine::DepartInfo cDept;
//
//				string strDepartId = pResultSet->GetString("uuid");
//				string strParentId = "";
//				if (pResultSet->GetString("parentuuid") != NULL)
//				{
//					strParentId = pResultSet->GetString("parentuuid");
//				}
//				string strDeptName = pResultSet->GetString("departname");
//				int32_t nStatus = pResultSet->GetInt("status");
//				uint32_t unContactStatus = pResultSet->GetInt("contactstatus");
//				uint32_t nPriority = pResultSet->GetInt("priority");
//				uint32_t nLevel = pResultSet->GetInt("level");
//				uint32_t unCompanyId = pResultSet->GetInt("companyid");
//				string strCompanyUuid = pResultSet->GetString("companyuuid");
//
//				cDept.set_dept_uuid(strDepartId);
//				cDept.set_company_id(unCompanyId);
//				cDept.set_parent_dept_uuid(strParentId);
//				cDept.set_dept_name(strDeptName);
//				cDept.set_dept_status(nStatus);
//				cDept.set_priority(nPriority);
//				cDept.set_dept_level(nLevel);
//				cDept.set_contact_status(unContactStatus);
//				cDept.set_company_uuid(strCompanyUuid);
//				lsDepts.push_back(cDept);
//			}
//			delete  pResultSet;
//			pResultSet = NULL;
//		}
//
//		pDBManager->RelDBConn(pDBConn);
//	}
//	else
//	{
//		log("no db connection for teamtalk_slave");
//	}
//}


void CDepartModel::GetDepartments(uint32_t unCompanyId, uint32_t& nLastTime, list<IM::BaseDefine::DepartInfo>& lsDepts)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select a.*, b.companyuuid from IMDepart a, IMCompany b where a.companyid=b.id and a.companyid=" + int2string(unCompanyId) + " and ";

		if (nLastTime == 0)
		{
			strSql += " a.status = 0";
		}
		else
		{
			strSql += " a.updated >= " + int2string(nLastTime);
		}

		log("SQL for getting changed departments: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				uint32_t nUpdated = pResultSet->GetInt("updated");
				if (nLastTime < nUpdated)
				{
					nLastTime = nUpdated;
				}

				IM::BaseDefine::DepartInfo cDept;

				string strDepartId = pResultSet->GetString("uuid");
				string strParentId = "";
				if (pResultSet->GetString("parentuuid") != NULL)
				{
					strParentId = pResultSet->GetString("parentuuid");
				}
				string strDeptName = pResultSet->GetString("departname");
				int32_t nStatus = pResultSet->GetInt("status");
				uint32_t unContactStatus = pResultSet->GetInt("contactstatus");
				uint32_t nPriority = pResultSet->GetInt("priority");
				uint32_t nLevel = pResultSet->GetInt("level");
				uint32_t unCompanyId = pResultSet->GetInt("companyid");
				string strCompanyUuid = "";
				if (pResultSet->GetString("companyuuid") != NULL)
				{
					strCompanyUuid = pResultSet->GetString("companyuuid");
				}

				cDept.set_dept_uuid(strDepartId);
				cDept.set_company_id(unCompanyId);
				cDept.set_parent_dept_uuid(strParentId);
				cDept.set_dept_name(strDeptName);
				cDept.set_dept_status(nStatus);
				cDept.set_priority(nPriority);
				cDept.set_dept_level(nLevel);
				cDept.set_contact_status(unContactStatus);
				cDept.set_company_uuid(strCompanyUuid);
				lsDepts.push_back(cDept);
			}
			delete  pResultSet;
			pResultSet = NULL;
		}

		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave");
	}
}
