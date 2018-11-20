#include "PositionModel.h"
#include "../DBPool.h"


CPositionModel* CPositionModel::m_pInstance = NULL;

CPositionModel* CPositionModel::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CPositionModel();
	}
	return m_pInstance;
}

void CPositionModel::getCompanyId(uint32_t	nUserId, uint32_t &nCompanyId)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select companyid from IMUser  where id = " + int2string(nUserId);
		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		log("SQL for getting companyid = %s", strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nCompanyId = pResultSet->GetInt("companyid");
				//log("GetCompany,CompanyID:%d", nCompanyId);
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


void CPositionModel::getPositions(uint32_t unCompanyId, uint32_t& nLastTime, list<IM::BaseDefine::PositionInfo>& lsPositions)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql;
		if (nLastTime == 0)
		{
			strSql = "select uuid,posname,type,status,sort,updated from Position where companyid= 0 or (companyid= " + int2string(unCompanyId) + " and status = 0)";
		}
		else
		{
			strSql = "select uuid,posname,type,status,sort,updated from Position where companyid=" + int2string(unCompanyId) + " and updated > " + int2string(nLastTime);
		}

		log("SQL for getting changed position = %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			IM::BaseDefine::PositionInfo PositionInfo;
			while (pResultSet->Next())
			{
				if (pResultSet->GetString("uuid") == NULL)
				{
					continue;
				}
				
				string strPositionId = pResultSet->GetString("uuid");
				string strPositionName = "";
				if (pResultSet->GetString("posname") != NULL)
				{
					strPositionName = pResultSet->GetString("posname");
				}

				string strPositionType = "";
				uint32_t unPositionType = IM::BaseDefine::OTHER;
				if (pResultSet->GetString("type") != NULL)
				{
					strPositionType = pResultSet->GetString("type");
					if (strPositionType == "chiefleader")
					{
						unPositionType = IM::BaseDefine::CHIEF_LEADER;
					}
					else if (strPositionType == "leader")
					{
						unPositionType = IM::BaseDefine::LEADER;
					}
					else if (strPositionType == "staff")
					{
						unPositionType = IM::BaseDefine::STAFF;
					}
					//else if (strPositionType == "other")
					else
					{
						unPositionType = IM::BaseDefine::OTHER;
					}
				}

				int nStatus = pResultSet->GetInt("status");
				uint32_t unSort = pResultSet->GetInt("sort");
				uint32_t nUpdated = pResultSet->GetInt("updated");

				if (nLastTime < nUpdated)
				{
					nLastTime = nUpdated;
				}

				PositionInfo.set_position_uuid(strPositionId); 
				PositionInfo.set_position_type((IM::BaseDefine::JobGradeType)unPositionType);
				PositionInfo.set_position_name(strPositionName);
				PositionInfo.set_status(nStatus);
				PositionInfo.set_sort(unSort);

				lsPositions.push_back(PositionInfo);
			}

			delete  pResultSet;
			pResultSet = NULL;
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave.");
	}
}


//void CPositionModel::GetPositions(uint32_t unUserId, uint32_t& nLastTime, list<IM::BaseDefine::PositionInfo>& lsPositions)
//{
//	CDBManager* pDBManager = CDBManager::getInstance();
//	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
//	if (pDBConn)
//	{
//		string strTempTable = "(select companyid from IMStaff where userid=" + int2string(unUserId) + " and status=1) as ";
//		string strSql = "select uuid,posname,type,status,sort,updated from IMPosition a where a.companyid=0 or (exists(select 1 from " + strTempTable + " b where a.companyid=b.companyid) ";
//
//		if (nLastTime == 0)
//		{
//			strSql += " and a.status = 0)";
//		}
//		else
//		{
//			strSql += " and a.updated > " + int2string(nLastTime) + ")";
//		}
//
//		log("SQL for getting changed position: %s", strSql.c_str());
//
//		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
//		if (pResultSet)
//		{
//			IM::BaseDefine::PositionInfo PositionInfo;
//			while (pResultSet->Next())
//			{
//				if (pResultSet->GetString("uuid") == NULL)
//				{
//					continue;
//				}
//
//				string strPositionId = pResultSet->GetString("uuid");
//				string strPositionName = "";
//				if (pResultSet->GetString("posname") != NULL)
//				{
//					strPositionName = pResultSet->GetString("posname");
//				}
//
//				string strPositionType = "";
//				uint32_t unPositionType = IM::BaseDefine::NONE;
//				if (pResultSet->GetString("type") != NULL)
//				{
//					strPositionType = pResultSet->GetString("type");
//					if (strPositionType == "chiefleader")
//					{
//						unPositionType = IM::BaseDefine::CHIEF_LEADER;
//					}
//					else if (strPositionType == "leader")
//					{
//						unPositionType = IM::BaseDefine::LEADER;
//					}
//					else if (strPositionType == "staff")
//					{
//						unPositionType = IM::BaseDefine::STAFF;
//					}
//					else if (strPositionType == "other")
//					{
//						unPositionType = IM::BaseDefine::OTHER;
//					}
//				}
//
//				int nStatus = pResultSet->GetInt("status");
//				uint32_t unSort = pResultSet->GetInt("sort");
//				uint32_t nUpdated = pResultSet->GetInt("updated");
//
//				if (nLastTime < nUpdated)
//				{
//					nLastTime = nUpdated;
//				}
//
//				PositionInfo.set_position_uuid(strPositionId);
//				PositionInfo.set_position_type((IM::BaseDefine::JobGradeType)unPositionType);
//				PositionInfo.set_position_name(strPositionName);
//				PositionInfo.set_status(nStatus);
//				PositionInfo.set_sort(unSort);
//				PositionInfo.set_company_id(pResultSet->GetInt("companyid"));
//
//				lsPositions.push_back(PositionInfo);
//			}
//
//			delete  pResultSet;
//			pResultSet = NULL;
//		}
//		pDBManager->RelDBConn(pDBConn);
//	}
//	else
//	{
//		log("no db connection for teamtalk_slave.");
//	}
//}


void CPositionModel::GetPositions(uint32_t unCompanyId, uint32_t& nLastTime, list<IM::BaseDefine::PositionInfo>& lsPositions)
{
	CDBManager* pDBManager = CDBManager::getInstance();
	CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_slave");
	if (pDBConn)
	{
		string strSql = "select * from IMPosition  where  companyid=" + int2string(unCompanyId) + " and ";

		if (nLastTime == 0)
		{
			strSql += " status = 0";
		}
		else
		{
			strSql += " updated >= " + int2string(nLastTime);
		}

		log("SQL for getting changed position: %s", strSql.c_str());

		CResultSet* pResultSet = pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			IM::BaseDefine::PositionInfo PositionInfo;
			while (pResultSet->Next())
			{
				if (pResultSet->GetString("uuid") == NULL)
				{
					continue;
				}

				string strPositionId = pResultSet->GetString("uuid");
				string strPositionName = "";
				if (pResultSet->GetString("posname") != NULL)
				{
					strPositionName = pResultSet->GetString("posname");
				}

				string strPositionType = "";
				uint32_t unPositionType = IM::BaseDefine::NONE;
				if (pResultSet->GetString("type") != NULL)
				{
					strPositionType = pResultSet->GetString("type");
					if (strPositionType == "chiefleader")
					{
						unPositionType = IM::BaseDefine::CHIEF_LEADER;
					}
					else if (strPositionType == "leader")
					{
						unPositionType = IM::BaseDefine::LEADER;
					}
					else if (strPositionType == "staff")
					{
						unPositionType = IM::BaseDefine::STAFF;
					}
					else if (strPositionType == "other")
					{
						unPositionType = IM::BaseDefine::OTHER;
					}
				}

				int nStatus = pResultSet->GetInt("status");
				uint32_t unSort = pResultSet->GetInt("sort");
				uint32_t nUpdated = pResultSet->GetInt("updated");

				if (nLastTime < nUpdated)
				{
					nLastTime = nUpdated;
				}

				PositionInfo.set_position_uuid(strPositionId);
				PositionInfo.set_position_type((IM::BaseDefine::JobGradeType)unPositionType);
				PositionInfo.set_position_name(strPositionName);
				PositionInfo.set_status(nStatus);
				PositionInfo.set_sort(unSort);
				PositionInfo.set_company_id(pResultSet->GetInt("companyid"));

				lsPositions.push_back(PositionInfo);
			}

			delete  pResultSet;
			pResultSet = NULL;
		}
		pDBManager->RelDBConn(pDBConn);
	}
	else
	{
		log("no db connection for teamtalk_slave.");
	}
}
