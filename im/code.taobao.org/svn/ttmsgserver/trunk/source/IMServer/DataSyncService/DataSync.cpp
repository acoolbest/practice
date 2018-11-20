#include "DataSync.h"

CDataSync::CDataSync()
{
	m_pDBConn = NULL;
	m_pCacheGroupMember = NULL;
	m_pCacheUser = NULL;
}


CDataSync::~CDataSync()
{
	if (m_pDBConn)
	{
		m_pDBConn = NULL;
	}

	if (m_pCacheGroupMember)
	{
		m_pCacheGroupMember = NULL;
	}

	if (m_pCacheUser)
	{
		m_pCacheUser = NULL;
	}
}


int CDataSync::ParseJsonFromFile(const char *pszFileName, LIST_DOMAIN_INFO& refDomainInfo, LIST_COMPANY_INFO& refCompanyInfo, LIST_DEPARTMENT_INFO& refDepartmentInfo, LIST_USER_INFO& refUserInfo,
	LIST_DOMAIN_GROUP &refDomainGroup, LIST_GROUP_USER &refGroupUser)
{
	Json::Reader reader;
	Json::Value root;

	//printf("FILE NAME: %s\n", pszFileName);

	std::ifstream is;
	is.open(pszFileName, std::ios::binary);
	if (reader.parse(is, root))
	{
		const char* pszCurrPos = NULL;

		//if ((pszCurrPos = strstr(pszFileName, FILE_NAME_DOMAIN_PREFIX)))
		if ((pszCurrPos = strstr(pszFileName, FILE_NAME_DOMAIN_PREFIX)))
		{
			DOMAIN_INFO DomainInfo;
			pszCurrPos += strlen(FILE_NAME_DOMAIN_PREFIX) + 1;
			const char *pszSyncServer = strchr(pszCurrPos, '_');
			if (pszSyncServer)
			{
				char szSyncServer[256];
				memset(szSyncServer, 0, sizeof(szSyncServer));
				strncpy(szSyncServer, pszCurrPos, pszSyncServer - pszCurrPos);
				DomainInfo.m_strSyncServer = szSyncServer;
			}

			int nElements = root.size();
			for (int i = 0; i < nElements; ++i)
			{
				string strProductId;
				if (!root[i]["productId"].isNull() && root[i]["productId"].isString())
				{
					strProductId = root[i]["productId"].asString();
			/*		if (strProductId.compare("IM") != 0)
					{
						continue;
					}*/
				}

				string strDomainID;
				string strDomainName;
				string strCompany;
				int nStatus = 0;
				if (!root[i]["domainId"].isNull() && root[i]["domainId"].isString())
				{
					strDomainID = root[i]["domainId"].asString();
				}

				if (!root[i]["domainName"].isNull() && root[i]["domainName"].isString())
				{
					strDomainName = root[i]["domainName"].asString();
				}

				if (!root[i]["company"].isNull() && root[i]["company"].isString())
				{
					strCompany = root[i]["company"].asString();
				}

				if (!root[i]["domainStatus"].isNull() && root[i]["domainStatus"].isInt())
				{
					nStatus = root[i]["domainStatus"].asInt();
				}

				if (nStatus == 1)  //enabled
				{
					nStatus = 0;
				}
				else if (nStatus == -1) //deleted
				{
					nStatus = 1;
				}
				else if (nStatus == 0) //disabled
				{
					nStatus = 2;
				}

				log("Index: %d\tDomainID: %s\tDomain Name: %s\tStatus:%d\tCompany:%s\tSyncServer: %s\n", i, strDomainID.c_str(), strDomainName.c_str(), nStatus, strCompany.c_str(), DomainInfo.m_strSyncServer.c_str());

				DomainInfo.m_nStatus = nStatus;
				DomainInfo.m_strCompanyName = strCompany;
				DomainInfo.m_strDomainID = strDomainID;
				DomainInfo.m_strDomainName = strDomainName;
				refDomainInfo.push_back(DomainInfo);
			}
		}
		else if (strstr(pszFileName, FILE_NAME_USER_PREFIX))
		{
			int nCount = root.size();
			string strCompany = "chinac";
			//GetDomainUser(root, NULL, 0, strCompany, refDepartmentInfo, refUserInfo);
		}
		else if (strstr(pszFileName, FILE_NAME_GROUP_PREFIX))
		{
			int nElements = root.size();
			for (int i = 0; i < nElements; ++i)
			{
				DOMAIN_GROUP DomainGroup;
				if (!root[i]["groupId"].isNull() && root[i]["groupId"].isString())
				{
					DomainGroup.m_strGroupID = root[i]["groupId"].asString();
				}

				if (!root[i]["domainId"].isNull() && root[i]["domainId"].isString())
				{
					DomainGroup.m_strDomainID = root[i]["domainId"].asString();
				}

				if (!root[i]["groupName"].isNull() && root[i]["groupName"].isString())
				{
					DomainGroup.m_strGroupName = root[i]["groupName"].asString();
				}

				if (!root[i]["email"].isNull() && root[i]["email"].isString())
				{
					DomainGroup.m_strEmail = root[i]["email"].asString();
				}

				if (!root[i]["userCount"].isNull() && root[i]["userCount"].isInt())
				{
					DomainGroup.m_nUserCount = root[i]["userCount"].asInt();
				}

				if (!root[i]["status"].isNull() && root[i]["status"].isInt())
				{
					DomainGroup.m_nStatus = root[i]["status"].asInt();
					if (DomainGroup.m_nStatus == 1)
					{
						DomainGroup.m_nStatus = 0;
					}
					else if (DomainGroup.m_nStatus == 0)
					{
						DomainGroup.m_nStatus = 1;
					}
				}

				log("INDEX:%d\tGROUP ID: %s\tDOMAIN ID: %s\tGROUP NAME: %s\tEMAIL; %s\tUSER COUNT: %d\tSTATUS: %d\n", i + 1, DomainGroup.m_strGroupID.c_str(), DomainGroup.m_strDomainID.c_str(),
					DomainGroup.m_strGroupName.c_str(), DomainGroup.m_strEmail.c_str(), DomainGroup.m_nUserCount, DomainGroup.m_nStatus);
				if (!root[i]["groupUsers"].isNull() && root[i]["groupUsers"].isArray())
				{
					Json::Value GroupUsers = root[i]["groupUsers"];
					int nUsers = GroupUsers.size();
					GROUP_USER Users;
					for (int i = 0; i < nUsers; i++)
					{

						if (!GroupUsers[i]["userId"].isNull() && GroupUsers[i]["userId"].isString())
						{
							Users.m_strUserID = GroupUsers[i]["userId"].asString();
						}

						if (!GroupUsers[i]["email"].isNull() && GroupUsers[i]["email"].isString())
						{
							Users.m_strEmail = GroupUsers[i]["email"].asString();
						}

						if (!GroupUsers[i]["userType"].isNull() && GroupUsers[i]["userType"].isInt())
						{
							Users.m_nUserType = GroupUsers[i]["userType"].asInt();
						}

						if (!GroupUsers[i]["groupId"].isNull() && GroupUsers[i]["groupId"].isString())
						{
							Users.m_strGroupID = GroupUsers[i]["groupId"].asString();
						}

						if (Users.m_nUserType == 0)
						{
							refGroupUser.push_back(Users);
						}
						log("INDEX: %d\tUSER ID: %s\tEMAIL: %s\tUSER TYPE: %d\tGROUP ID: %s\n", i + 1, Users.m_strUserID.c_str(), Users.m_strEmail.c_str(), Users.m_nUserType, Users.m_strGroupID.c_str());
					}
				}

				refDomainGroup.push_back(DomainGroup);
			}
		}
		is.close();
		return 0;
	}
}


//int CDataSync::GetDomainUser(Json::Value &root, const char* pszParent, int nLevel, string& strCompanyID, LIST_DEPARTMENT_INFO& DepartmentInfo, LIST_USER_INFO& UserInfo)
//{
//	try
//	{
//
//		int nCount = root.size();
//		if (!nCount)
//		{
//			return -1;
//		}
//
//		//get default avatar urls from a specified config file
//		char *pszAvatarUrl = NULL;
//		int nTotalAvatarUrls = 0;
//		nTotalAvatarUrls = GetDefaultAvatarUrl(&pszAvatarUrl);
//		srand(time(NULL));
//
//		string strTopDepartment = "";
//
//		log("COUNT: %d\n", nCount);
//		for (int i = 0; i < nCount; ++i)
//		{
//			Json::Value Department = root[i];
//			DEPARTMENT_INFO DepartInfo;
//			if (!Department["orgId"].isNull() && Department["orgId"].isString())
//			{
//				DepartInfo.m_strDepartmentID = Department["orgId"].asString();
//			}
//
//			if (!Department["domainId"].isNull() && Department["domainId"].isString())
//			{
//				DepartInfo.m_strDomainID = Department["domainId"].asString();
//			}
//
//			if (!Department["orgName"].isNull() && Department["orgName"].isString())
//			{
//				DepartInfo.m_strDepartmentName = Department["orgName"].asString();
//			}
//
//			if (!Department["parentId"].isNull() && Department["parentId"].isString())
//			{
//				DepartInfo.m_strParentDepartmentID = Department["parentId"].asString();
//			}
//
//			if (!Department["level"].isNull() && Department["level"].isInt())
//			{
//				DepartInfo.m_nLevel = Department["level"].asInt();
//				if (DepartInfo.m_nLevel == 0)
//				{
//					strTopDepartment = DepartInfo.m_strDepartmentID;
//				}
//			}
//
//			if (!Department["sort"].isNull())
//			{
//				if (Department["sort"].isInt())
//				{
//					DepartInfo.m_nPriority = Department["sort"].asInt();
//				}
//			}
//			else
//			{
//				DepartInfo.m_nPriority = 65535;
//			}
//
//			if (DepartInfo.m_strParentDepartmentID.compare("-2") == 0)
//			{
//				DepartInfo.m_strDepartmentID = DepartInfo.m_strDomainID + DepartInfo.m_strParentDepartmentID;
//				DepartInfo.m_strParentDepartmentID = strTopDepartment;
//			}
//
//			DepartmentInfo.push_back(DepartInfo);
//
//			/*	if (DepartInfo.m_nPriority == 65535)
//			{
//			printf("DOMAINID: %s\tPARENTID: %s\tLEVEL:%d\tNAME:%s\n", DepartInfo.m_strDomainID.c_str(), DepartInfo.m_strParentDepartmentID.c_str(), DepartInfo.m_nLevel, DepartInfo.m_strDepartmentName.c_str());
//			}*/
//
//			log("INDEX: %d\tDEPART ID:%s\tDOMAIN ID:%s\tDEPART NAME:%s\tPARENT DEPART ID: %s\t LEVEL:%d PRIORITY: %d\n", i, DepartInfo.m_strDepartmentID.c_str(), DepartInfo.m_strDomainID.c_str(), DepartInfo.m_strDepartmentName.c_str(),
//				DepartInfo.m_strParentDepartmentID.c_str(), DepartInfo.m_nLevel, DepartInfo.m_nPriority);
//
//			if (!Department["users"].isArray())
//			{
//				continue;
//			}
//
//			int nTotals = 0;
//			if (!Department["users"].isNull() && Department["users"].isArray())
//			{
//				nTotals = Department["users"].size();
//			}
//			log("TOTAL USERS: %d\n", nTotals);
//
//			if (nTotals)
//			{
//				for (int j = 0; j < nTotals; j++)
//				{
//					Json::Value CurrentUser = Department["users"][j];
//					int nProducts = CurrentUser["productIds"].size();
//					if (nProducts)
//					{
//						Json::Value Products = CurrentUser["productIds"];
//						int k = 0;
//
//						//get products a user buys
//						string strBuyProducts = "";
//						for (; k < nProducts - 1; k++)
//						{
//							strBuyProducts += Products[k].asString() + ',';
//						}
//						strBuyProducts += Products[k].asString();
//
//						//if (strBuyProducts.find("IM") == string::npos) //IM no found
//						//{
//						//	continue;
//						//}
//
//						USER_INFO User;
//						//memset(&User, 0, sizeof(User));
//						User.m_strDepartmentID = DepartInfo.m_strDepartmentID;
//						User.m_strDomain = CurrentUser["domainId"].asString();
//						User.m_strUserID = CurrentUser["userId"].asString();
//						User.m_strMail = CurrentUser["username"].asString();
//						User.m_strFullName = CurrentUser["fullName"].asString();
//						User.m_strBuyProducts = strBuyProducts;
//						if (CurrentUser["sex"].isInt())
//						{
//							User.m_nSex = CurrentUser["sex"].asInt();
//							if (User.m_nSex == 0)
//							{
//								//User.m_nSex = 2; //female
//								User.m_nSex = 0; //unknown
//							}
//							else if (User.m_nSex == 2)
//							{
//								User.m_nSex = 0; //unknown
//							}
//						}
//						else
//						{
//							User.m_nSex = 0; //unknown
//						}
//
//						User.m_strPhone = CurrentUser["phone"].asString();
//						User.m_strKeyword = CurrentUser["keyword"].asString();
//						User.m_strPassword = CurrentUser["password"].asString();
//						User.m_strUserCode = CurrentUser["userCode"].asString();
//						User.m_unContactStatus = CurrentUser["contactsStatus"].asInt();
//						long long llBirthday = 0ll;
//						if (!CurrentUser["birthday"].isNull())
//						{
//							if (CurrentUser["birthday"].isUInt64())
//							{
//								llBirthday = CurrentUser["birthday"].asLargestUInt();
//							}
//							else
//							{
//								llBirthday = 0;
//							}
//						}
//
//						time_t unBirthday = (time_t)(llBirthday / 1000);
//						char szBirthday[16];
//						memset(szBirthday, 0, sizeof(szBirthday));
//						strftime(szBirthday, sizeof(szBirthday)-1, "%Y/%m/%d", localtime(&unBirthday));
//						User.m_strBirthday = szBirthday;
//
//						if (!CurrentUser["updateTime"].isNull() && CurrentUser["updateTime"].isUInt64())
//						{
//							llBirthday = CurrentUser["updateTime"].asLargestUInt();
//						}
//
//						User.m_unUpdatedTime = (unsigned int)(llBirthday / 1000);
//
//						int nPositions = CurrentUser["positionNames"].size();
//						if (nPositions)
//						{
//							User.m_strPosition = "";
//							Json::Value Positions = CurrentUser["positionNames"];
//							//int nPositions = CurrentUser["positionNames"].asInt();
//							for (int l = 0; l < nPositions - 1; l++)
//							{
//								User.m_strPosition += Positions[l].asString() + ',';
//							}
//							User.m_strPosition += Positions[nPositions - 1].asString();
//
//						}
//
//						/*	if (CurrentUser["isLock"].asInt() == 1)
//						{
//						User.m_nStatus = 5;
//						}*/
//						//get alias.inserted by george on Nov 5th,2015 
//						int nAliasNames = 0;
//						if (!CurrentUser["aliasNames"].isNull() && CurrentUser["aliasNames"].isArray())
//						{
//							nAliasNames = CurrentUser["aliasNames"].size();
//							if (nAliasNames)
//							{
//								User.m_strAliasName = "";
//								Json::Value  jsonAliasName = CurrentUser["aliasNames"];
//								for (int i = 0; i < nAliasNames - 1; i++)
//								{
//									User.m_strAliasName += jsonAliasName[i].asString() + ',';
//								}
//								User.m_strAliasName += jsonAliasName[nAliasNames - 1].asString();
//							}
//						}
//
//						int nStatus = CurrentUser["status"].asInt();
//						if (nStatus == -1)
//						{
//							User.m_nStatus = 1;
//						}
//						else if (nStatus == 0)
//						{
//							User.m_nStatus = 2;
//						}
//						else
//						{
//							User.m_nStatus = 0;
//						}
//
//						//get avatar url
//						User.m_strAvatarUrl = "";
//						if (nTotalAvatarUrls > 0)
//						{
//							int nIndex = rand() % nTotalAvatarUrls;
//							User.m_strAvatarUrl = pszAvatarUrl + 512 * nIndex;
//							log("INDEX: %d\tAVATAR: %s\n", nIndex, User.m_strAvatarUrl.c_str());
//							//User.m_strAvatarUrl = ""; //for test
//						}
//		
//						log("UserID: %s\tPassword: %s\tUserName:%s\tDomaindID; %s\tAvatarURL: %s=\tFullName: %s\tAlias: %s\n", User.m_strUserID.c_str(), User.m_strPassword.c_str(), User.m_strMail.c_str(), User.m_strDomain.c_str(), "", User.m_strFullName.c_str(), User.m_strAliasName.c_str());
//						log("Products: %s\tSex: %d\tPhone: %s\tStatus: %d\tKeyword: %s\tPosition: %s\tBirthday:%s ContactStatus: %d, UpdatedTime: %d\n\n", User.m_strBuyProducts.c_str(), User.m_nSex, User.m_strPhone.c_str(), User.m_nStatus, User.m_strKeyword.c_str(), User.m_strPosition.c_str(), User.m_strBirthday.c_str(), User.m_unContactStatus, User.m_unUpdatedTime);
//
//						//if (strBuyProducts.find("IM") == string::npos) //IM no found
//						//{
//						//	log("%s:%s dose not order IM product,discarding..........", User.m_strUserID.c_str(), User.m_strMail.c_str());
//						//	continue;
//						//}
//						//else
//						//support all products
//						{
//							UserInfo.push_back(User);
//						}
//					}
//
//				}
//
//			}
//		}
//
//		//don't forget to free memory
//		if (pszAvatarUrl)
//		{
//			delete[]pszAvatarUrl;
//			pszAvatarUrl = NULL;
//		}
//	}
//	catch (std::runtime_error msg)
//	{
//		log("parse json data failed...........\n");
//	}
//
//	return 0;
//}


//all companies
void CDataSync::ImportCompanyInfo(LIST_COMPANY_INFO& refCompanyInfo, LIST_DOMAIN_INFO& refDomainInfo)
{
	if (refDomainInfo.size() > 0)
	{
		string strSql = "truncate table IMCompanyTemp";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		for (IT_LIST_DOMAIN_INFO it = refDomainInfo.begin(); it != refDomainInfo.end(); it++)
		{
			strSql = "insert into IMCompanyTemp(domainid,domainname,companyname,status,created,updated,syncserver ) values(?,?,?,?,?,?,?)";
			// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
			{
				int nIndex = 0;
				unsigned int nCurrenTime = time(NULL);

				pStmt->SetParam(nIndex++, it->m_strDomainID);
				pStmt->SetParam(nIndex++, it->m_strDomainName);
				pStmt->SetParam(nIndex++, it->m_strCompanyName);
				pStmt->SetParam(nIndex++, it->m_nStatus);
				pStmt->SetParam(nIndex++, nCurrenTime);
				pStmt->SetParam(nIndex++, nCurrenTime);
				pStmt->SetParam(nIndex++, it->m_strSyncServer);
				pStmt->ExecuteUpdate();
			}
			delete pStmt;
		}

		//delete duplicate data
		strSql = "delete from IMCompanyTemp  using IMCompanyTemp, (select max(id) as maxid,domainid from IMCompanyTemp group by domainid having count(*) > 1) as B where (IMCompanyTemp.domainid = B.domainid) and (IMCompanyTemp.id != B.maxid)";
		bool bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//delete removed data 
		strSql = "update IMCompany set status = 1,updated=unix_timestamp() where exists(select 1 from IMCompanyTemp where IMCompany.syncserver =IMCompanyTemp.syncserver) and status <> 1 and not exists(select 1 from IMCompanyTemp  where IMCompany.domainid=IMCompanyTemp.domainid)";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//update data for existing company 
		strSql = "update IMCompany a inner join IMCompanyTemp b on a.domainid=b.domainid set a.companyname=b.companyname,a.domainname=b.domainname,a.status=b.status,a.syncserver=b.syncserver,a.updated=b.updated where a.domainid=b.domainid and (a.syncserver <> b.syncserver or a.companyname<>b.companyname or a.status <> b.status or a.domainname <> b.domainname)";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//update sync server
		strSql = "update IMCompany a, IMCompanyTemp b set a.syncserver = b.syncserver where a.domainid=b.domainid and a.syncserver=''";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());


		//insert new data
		strSql = "insert into IMCompany(domainid,domainname,companyname,status,created,updated,syncserver) select domainid,domainname,companyname,status,created,updated,syncserver from IMCompanyTemp where not exists(select 1 from IMCompany where IMCompany.domainid =IMCompanyTemp.domainid)";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
	}

}


//all departments in a company
void CDataSync::ImportDepartmentInfo(LIST_DEPARTMENT_INFO& refDepartmentInfo)
{

	int nStatus = 0;

	if (refDepartmentInfo.size() > 0)
	{
		string strSql = "truncate table IMDepartTemp";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		for (IT_LIST_DEPARTMENT_INFO it = refDepartmentInfo.begin(); it != refDepartmentInfo.end(); ++it) //data write company info to temp table
		{
			strSql = "insert into IMDepartTemp(departid,departname,priority,parentid,domainid,level,status,created,updated) values(?,?,?,?,?,?,?,?,?)";
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
			{
				int nIndex = 0;
				unsigned int  nCurrenTime = time(NULL);
				pStmt->SetParam(nIndex++, it->m_strDepartmentID);
				pStmt->SetParam(nIndex++, it->m_strDepartmentName);
				pStmt->SetParam(nIndex++, it->m_nPriority);
				pStmt->SetParam(nIndex++, it->m_strParentDepartmentID);
				pStmt->SetParam(nIndex++, it->m_strDomainID);
				pStmt->SetParam(nIndex++, it->m_nLevel);
				pStmt->SetParam(nIndex++, nStatus);
				pStmt->SetParam(nIndex++, nCurrenTime);
				pStmt->SetParam(nIndex++, nCurrenTime);
				pStmt->ExecuteUpdate();
			}
			delete pStmt;
		}


		//delete duplicate data
		strSql = "delete from IMDepartTemp  using IMDepartTemp, (select max(id) as maxid,departid from IMDepartTemp group by departid having count(*) > 1) as B where (IMDepartTemp.departid = B.departid) and (IMDepartTemp.id != B.maxid)";
		//printf("SQL: %s\n", strSql.c_str());
		bool bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//update companyid from temp department table
		strSql = "update IMDepartTemp a inner join IMCompany b on a.domainid=b.domainid set a.companyid=b.id where a.domainid=b.domainid";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		typedef list<int> LIST_TMPCOMPANY;
		typedef LIST_TMPCOMPANY::iterator IT_LIST_TMPCOMPANY;
		LIST_TMPCOMPANY listCompany;
		//get company id from temp table
		strSql = "select distinct companyid from IMDepartTemp";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		int nCompanyID = 0;
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nCompanyID = pResultSet->GetInt("companyid");
				listCompany.push_back(nCompanyID);
				//delete removed data
				char szSql[256];
				memset(szSql, 0, sizeof(szSql));
				snprintf(szSql, sizeof(szSql)-1, "update IMDepart set status = 1,updated=unix_timestamp() where companyid= %d and status <> 1 and not exists(select 1 from IMDepartTemp where IMDepartTemp.departid = IMDepart.departid)", nCompanyID);
				//printf("SQL for updating department: %s\n", szSql);
				bRet = m_pDBConn->ExecuteUpdate(szSql);
			}

			delete pResultSet;
			pResultSet = NULL;
		}

		//update existing data
		strSql = "update IMDepart a inner join IMDepartTemp b on a.departid=b.departid set a.companyid=b.companyid,a.departname=b.departname,a.priority=b.priority,a.status=b.status,a.level=b.level,a.updated=b.updated where a.departid=b.departid and (a.departname <> b.departname or a.level <> b.level or a.priority <> b.priority or a.status <> b.status)";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());


		//insert new data
		strSql = "insert into IMDepart(companyid,departid,departname,priority,level,status,created,updated) select companyid,departid,departname,priority,level,status,created,updated from IMDepartTemp where not exists(select 1 from IMDepart where IMDepart.departid =IMDepartTemp.departid)";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//update parentid
		strSql = "update IMDepart c inner join (select a.departid,b.id from IMDepartTemp a, (select id,departid from IMDepart where exists(select 1 from (select distinct parentid as dp from IMDepartTemp where parentid<>'-1')temp1 where IMDepart.departid = temp1.dp)) b where a.parentid=b.departid)d on c.departid=d.departid set c.parentid=d.id where c.departid=d.departid";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
}


//all users in a company
//void CDataSync::ImportUserInfo(LIST_USER_INFO& refUserInfo)
//{
//	if (refUserInfo.size() > 0)
//	{
//		string strSql = "truncate table IMUserTemp";
//		m_pDBConn->ExecuteUpdate(strSql.c_str());
//		string strDomainInEmail = "";
//
//		for (IT_LIST_USER_INFO it = refUserInfo.begin(); it != refUserInfo.end(); ++it) //data write user info to temp table
//		{
//			strSql = "insert into IMUserTemp(userid,name,sex,keyword,domain,jobnumber,password,phone,birthday,email,avatar,status,created,updated,alias,buyproduct) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
//			// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
//
//			CPrepareStatement* pStmt = new CPrepareStatement();
//			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
//			{
//				int nIndex = 0;
//
//				unsigned int nCurrenTime = time(NULL);
//				pStmt->SetParam(nIndex++, it->m_strUserID);
//				pStmt->SetParam(nIndex++, it->m_strFullName);
//				pStmt->SetParam(nIndex++, it->m_nSex);
//				pStmt->SetParam(nIndex++, it->m_strKeyword);
//				pStmt->SetParam(nIndex++, it->m_strDomain);
//				pStmt->SetParam(nIndex++, it->m_strUserCode);
//				pStmt->SetParam(nIndex++, it->m_strPassword);
//				pStmt->SetParam(nIndex++, it->m_strPhone);
//				pStmt->SetParam(nIndex++, it->m_strBirthday);
//				pStmt->SetParam(nIndex++, it->m_strMail);
//				pStmt->SetParam(nIndex++, it->m_strAvatarUrl);
//				pStmt->SetParam(nIndex++, it->m_nStatus);
//				pStmt->SetParam(nIndex++, nCurrenTime);
//				pStmt->SetParam(nIndex++, it->m_unUpdatedTime);
//				pStmt->SetParam(nIndex++, it->m_strAliasName);
//				pStmt->SetParam(nIndex++, it->m_strBuyProducts);
//				pStmt->ExecuteUpdate();
//				/*int nPosition  = it->m_strMail.find('@');
//				if(nPosition != string::npos)
//				{
//				strDomainInEmail = it->m_strMail.substr(nPosition+1);
//				}*/
//			}
//			delete pStmt;
//		}
//
//		//delete duplicate data
//		strSql = "delete from IMUserTemp  using IMUserTemp, (select max(id) as maxid,userid from IMUserTemp group by userid having count(*) > 1) as B where (IMUserTemp.userid = B.userid) and (IMUserTemp.id != B.maxid)";
//		bool bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//		//delete removed data 
//		strSql = "select distinct substring_index(email,'@',-1) as mailsuffix from IMUserTemp";
//		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
//		if (pResultSet)
//		{
//			while (pResultSet->Next())
//			{
//				strDomainInEmail = pResultSet->GetString("mailsuffix");
//				strSql = "update IMUser set status=1,updated=unix_timestamp() where substring_index(email,'@',-1) = '" + strDomainInEmail + "' and status <> 1 and  not exists(select 1 from IMUserTemp where IMUserTemp.userid = IMUser.userid)";
//				//printf("SQL for updating user info: %s\n", strSql.c_str());
//				bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//			}
//			delete pResultSet;
//			pResultSet = NULL;
//		}
//
//		//update existing data except field 'avatar'
//		//george added a status condition judgement on Dec 4th,2015
//		strSql = "update IMUser a inner join IMUserTemp b on a.userid=b.userid set a.name=b.name,a.domain=b.domain,a.email=b.email,a.keyword=b.keyword,a.password=b.password,a.status=b.status,a.updated=unix_timestamp(),a.orgupdated=b.updated, a.jobnumber=b.jobnumber,a.alias=b.alias,a.buyproduct=b.buyproduct where a.userid=b.userid and a.orgupdated < b.updated ";
//		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//		//update existing data only for field 'avatar'
//		//strSql = "update IMUser a inner join IMUserTemp b on a.userid=b.userid set a.avatar=b.avatar where a.userid=b.userid and a.avatar='' and b.avatar <> ''";
//		//bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//		////update existing data only for field 'avatar'
//		//strSql = "update IMUser set nick=name where nick=''";
//		//bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//		////update existing data only for field 'alias'. inserted by george on Nov 6th, 2015
//		//strSql = "update IMUser a inner join IMUserTemp b on a.userid=b.userid set a.alias =b.alias where a.userid=b.userid and a.alias ='' and b.alias <> ''";
//
//		//insert new data
//		strSql = "insert into IMUser(userid,name,nick,sex,keyword,domain,password,phone,jobnumber,birthday,email,avatar,status,created,updated,orgupdated,buyproduct,alias) select userid,name,name,sex,keyword,domain,password,phone,jobnumber,birthday,email,avatar,status,created,created,updated,buyproduct,alias from IMUserTemp where not exists(select 1 from IMUser where IMUser.userid =IMUserTemp.userid)";
//		//printf("SQL: %s\n", strSql.c_str());
//		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//	}
//
//}


//void CDataSync::ImportUserPosition(LIST_USER_INFO& refUserInfo)
//{
//	if (refUserInfo.size() > 0)
//	{
//		string strSql = "truncate table IMUserPositionTemp";
//		m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//		for (IT_LIST_USER_INFO it = refUserInfo.begin(); it != refUserInfo.end(); ++it) //data write user info to temp table
//		{
//			strSql = "insert into IMUserPositionTemp(userid,departid,position,created,updated) values(?,?,?,?,?)";
//			// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
//
//			CPrepareStatement* pStmt = new CPrepareStatement();
//			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
//			{
//				int nIndex = 0;
//				unsigned int nCurrenTime = time(NULL);
//				pStmt->SetParam(nIndex++, it->m_strUserID);
//				pStmt->SetParam(nIndex++, it->m_strDepartmentID);
//				pStmt->SetParam(nIndex++, it->m_strPosition);
//				pStmt->SetParam(nIndex++, nCurrenTime);
//				pStmt->SetParam(nIndex++, nCurrenTime);
//				pStmt->ExecuteUpdate();
//			}
//			delete pStmt;
//		}
//
//
//		//delete duplicate data
//		strSql = "delete from IMUserPositionTemp  using IMUserPositionTemp, (select max(id) as maxid,userid,departid from IMUserPositionTemp group by userid,departid having count(*) > 1) as B where (IMUserPositionTemp.userid = B.userid) and (IMUserPositionTemp.departid = B.departid)  and (IMUserPositionTemp.id != B.maxid)";
//		//printf("SQL: %s\n", strSql.c_str());
//		bool bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//		//delete removed data 
//		char szSql[512];
//		memset(szSql, 0, sizeof(szSql));
//		strcpy(szSql, "select distinct a.companyid from IMDepart a, IMUserPositionTemp b where a.departid=b.departid");
//		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(szSql);
//		int nCompanyID = 0;
//		if (pResultSet)
//		{
//			while (pResultSet->Next())
//			{
//				nCompanyID = pResultSet->GetInt("companyid");
//				memset(szSql, 0, sizeof(szSql));
//				//delete removed data 
//				sprintf(szSql, "update IMUserPosition set status=1,updated=unix_timestamp() where companyid=%d and status <> 1 and not exists(select 1 from (select a.id as userid,b.id  as departid from IMUser a, IMDepart b, IMUserPositionTemp c where a.userid=c.userid and b.departid=c.departid) as c  where IMUserPosition.userid = c.userid and IMUserPosition.departid = c.departid)", nCompanyID);
//				bRet = m_pDBConn->ExecuteUpdate(szSql);
//			}
//			delete pResultSet;
//			pResultSet = NULL;
//		}
//
//		//update existing data
//		strSql = "update IMUserPosition a inner join (select b.id as uid,c.id as did,position,d.status,d.updated from IMUser b,IMDepart c,IMUserPositionTemp d where b.userid=d.userid and c.departid=d.departid) as e on a.userid=e.uid and a.departid=e.did set a.position=e.position,a.status=e.status,a.updated=e.updated where a.userid=e.uid and a.departid=e.did and (a.position <> e.position or a.status <> e.status)";
//		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//		//insert new data
//		strSql = "insert into IMUserPosition(userid,departid,position,created,updated) select userid,departid,position,created,updated from (select a.id as userid,b.id  as departid,position,c.created,c.updated from IMUser a, IMDepart b, IMUserPositionTemp c where a.userid=c.userid and b.departid=c.departid) as d where not exists(select 1 from IMUserPosition  where d.userid = IMUserPosition.userid and d.departid = IMUserPosition.departid)";
//		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//		strSql = "update IMUserPosition a inner join IMDepart b on a.departid=b.id set a.companyid=b.companyid where a.departid=b.id and a.companyid=0";
//		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
//	}
//}
//
//
//int CDataSync::GetDefaultAvatarUrl(char** ppAvatarUrl)
//{
//	FILE *phFile = fopen(AVATAR_CONFIG_FILE_NAME, "r");
//	int nCount = 0, nTotalCount = 0;
//	if (phFile)
//	{
//		char szLine[1024];
//		memset(szLine, 0, sizeof(szLine));
//		while (!feof(phFile))
//		{
//			if (fgets(szLine, sizeof(szLine)-1, phFile))
//			{
//				szLine[strlen(szLine) - 1] = 0;
//				if (strstr(szLine, KEY_TOTAL_URLS))
//				{
//					if (sscanf(szLine, FORMAT_KEY_TOTAL_URLS, &nCount) == 1)
//					{
//						//printf("COUNT: %d\n", nCount);
//						*ppAvatarUrl = new char[512 * nCount];
//						memset(*ppAvatarUrl, 0, 512 * nCount);
//					}
//				}
//				else if (strstr(szLine, KEY_AVATAR_URL))
//				{
//					int nIndex = 0;
//					if (sscanf(szLine, FORMAT_KEY_AVATAR_URL, &nIndex, *ppAvatarUrl + 512 * nTotalCount) == 2)
//					{
//						//printf("INDEX: %d\tURL: %s\n", nIndex, *ppAvatarUrl + 512 * nTotalCount);
//						++nTotalCount;
//					}
//				}
//			}
//		}
//	}
//	fclose(phFile);
//	return nTotalCount;
//}
//
//
////void CDataSync::CheckSyncData(CDBConn* m_pDBConn, CacheConn* pCacheConn)
//void CDataSync::CheckSyncData(void)
//
//{
//	string strSql = "update IMDepart a,IMCompany b  set a.status=1,a.updated=unix_timestamp() where a.companyid=b.id and b.status<>0";
//	m_pDBConn->ExecuteUpdate(strSql.c_str());
//	strSql = "update IMUser a,IMUserPosition b ,IMCompany c set a.status=1,a.updated=unix_timestamp(),b.status=1,b.updated=unix_timestamp() where a.id=b.userid and c.id=b.companyid and c.status <> 0";
//	m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//	strSql = "update IMGroupMember a, IMGroup b set a.status=1,a.updated=unix_timestamp() where a.groupid=b.id and b.status <> 0 and a.status = 0";
//	m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//	//get group which has no creator
//	strSql = "select id from IMGroup a where exists(select 1 from IMUser b where a.creator=b.id and b.status=1)";
//	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
//	if (pResultSet)
//	{
//		uint32_t unGroupID = 0;
//		while (pResultSet->Next())
//		{
//			unGroupID = pResultSet->GetInt("id");
//			char szSql[512];
//			memset(szSql, 0, sizeof(szSql));
//			snprintf(szSql, sizeof(szSql)-1, "update IMGroup a, (select min(userid) as minuserid from IMGroupMember where status = 0 and groupid = %d) as b set a.creator=b.minuserid,a.updated=unix_timestamp(), a.version=a.version+1 where a.id=%d", unGroupID, unGroupID);
//			m_pDBConn->ExecuteUpdate(szSql); //update creator 
//		}
//		delete pResultSet;
//		pResultSet = NULL;
//	}
//
//	//the following 2 steps should be in a transaction,correct lately
//	//update group member counts
//	strSql = "update IMGroup a, (select groupid,count(*) as deletedusers from IMGroupMember b  where exists (select 1 from IMUser c where b.userid=c.id and c.status=1 and b.status <> 1) group by groupid having count(*) > 0) as d set a.usercnt=a.usercnt-d.deletedusers,a.updated=unix_timestamp(), a.version=a.version+1  where a.id=d.groupid and a.status = 0  and d.deletedusers <= a.usercnt";
//	m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//	//update group member 
//	strSql = "update IMGroupMember a set a.status=1, a.updated=unix_timestamp() where exists(select 1 from IMUser b where a.userid=b.id and a.status <> 1 and b.status = 1)";
//	m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//	//更新一份到redis中 added by george on Nov 5th,2015
//	if (m_pCacheGroupMember)
//	{
//		map<string, string> mapCachedGroupMember;
//		MAP_MEMBER_INFO mapDBGroupMember; //key:userid,value:status
//		list<uint32_t> listGroup;
//
//		//get groups
//		strSql = "select id from IMGroup where status = 0";
//		pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
//		if (pResultSet)
//		{
//			while (pResultSet->Next())
//			{
//				uint32_t unGroupID = pResultSet->GetInt("id");
//				listGroup.push_back(unGroupID);
//			}
//			delete pResultSet;
//			pResultSet = NULL;
//		}
//
//		for (list<uint32_t>::iterator it = listGroup.begin(); it != listGroup.end(); it++) //check members for each group
//		{
//			char szSql[512];
//			memset(szSql, 0, sizeof(szSql));
//			snprintf(szSql, sizeof(szSql)-1, "select userId,status,created from IMGroupMember where groupid=%d", *it);  //get members for a group from db
//			pResultSet = m_pDBConn->ExecuteQuery(szSql);
//			if (pResultSet)
//			{
//				while (pResultSet->Next())
//				{
//					MEMBER_INFO CurrentUser;
//					memset(&CurrentUser, 0, sizeof(CurrentUser));
//					uint32_t unUserID = pResultSet->GetInt("userId");
//					CurrentUser.m_unStatus = pResultSet->GetInt("status");
//					CurrentUser.m_unCreateTime = pResultSet->GetInt("created");
//					mapDBGroupMember[unUserID] = CurrentUser;
//				}
//				delete pResultSet;
//				pResultSet = NULL;
//			}
//
//			//get members for a group from cache
//			char szKey[64];
//			memset(szKey, 0, sizeof(szKey));
//			snprintf(szKey, sizeof(szKey)-1, "group_member_%d", *it);
//			m_pCacheGroupMember->hgetAll(szKey, mapCachedGroupMember);
//
//			//both cache and db data are consistent
//			for (IT_MAP_MEMBER_INFO it1 = mapDBGroupMember.begin(); it1 != mapDBGroupMember.end(); it1++)
//			{
//				char szUserID[32], szCreateTime[32];
//				memset(szUserID, 0, sizeof(szUserID));
//				memset(szCreateTime, 0, sizeof(szCreateTime));
//
//				snprintf(szUserID, sizeof(szUserID)-1, "%d", it1->first);
//				snprintf(szCreateTime, sizeof(szCreateTime)-1, "%d", it1->second.m_unCreateTime);
//
//				if (mapCachedGroupMember.find(szUserID) == mapCachedGroupMember.end())
//				{
//
//					if (it1->second.m_unStatus == 0)  //a new member
//					{
//						m_pCacheGroupMember->hset(szKey, szUserID, szCreateTime);
//					}
//				}
//				else
//				{
//					if (it1->second.m_unStatus == 1) //deleted member
//					{
//						m_pCacheGroupMember->hdel(szKey, szUserID);
//					}
//				}
//			}
//
//			//clear members 
//			mapDBGroupMember.clear();
//			mapCachedGroupMember.clear();
//		}
//	}
//
//	//added by george on Nov 23rd,2015
//	if (m_pCacheUser)
//	{
//		strSql = "select id,status from IMUser";
//		pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
//		if (pResultSet)
//		{
//			while (pResultSet->Next())
//			{
//				uint32_t unUserID = pResultSet->GetInt("id");
//				uint32_t unStatus = pResultSet->GetInt("status");
//				SetUserStatus(unUserID, unStatus);
//			}
//			delete pResultSet;
//			pResultSet = NULL;
//		}
//	}
//}


void CDataSync::ImportDomainGroup(LIST_DOMAIN_GROUP& refDomainGroup)
{
	if (refDomainGroup.size() > 0)
	{
		string strSql = "truncate table IMGroupTemp";
		m_pDBConn->ExecuteUpdate(strSql.c_str());
		for (IT_LIST_DOMAIN_GROUP it = refDomainGroup.begin(); it != refDomainGroup.end(); ++it) //write data to temp table
		{
			strSql = "insert into IMGroupTemp(groupid,domainid,name,type,usercnt,status,created,updated,email) values(?,?,?,?,?,?,?,?,?)";
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
			{
				int nIndex = 0;

				unsigned int nCurrentTime = time(NULL);
				int nType = 3;
				pStmt->SetParam(nIndex++, it->m_strGroupID);
				pStmt->SetParam(nIndex++, it->m_strDomainID);
				pStmt->SetParam(nIndex++, it->m_strGroupName);
				pStmt->SetParam(nIndex++, nType);
				pStmt->SetParam(nIndex++, it->m_nUserCount);
				pStmt->SetParam(nIndex++, it->m_nStatus);
				pStmt->SetParam(nIndex++, nCurrentTime);
				pStmt->SetParam(nIndex++, nCurrentTime);
				pStmt->SetParam(nIndex++, it->m_strEmail);
				pStmt->ExecuteUpdate();
			}
			delete pStmt;
		}

		//delete duplicate data
		strSql = "delete from IMGroupTemp  using IMGroupTemp, (select max(id) as maxid,groupid from IMGroupTemp group by groupid having count(*) > 1) as B where (IMGroupTemp.groupid = B.groupid and IMGroupTemp.id != B.maxid)";
		//printf("SQL: %s\n", strSql.c_str());
		bool bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//delete removed data 
		strSql = "select a.id from IMCompany a, IMGroupTemp b where a.domainid=b.domainid";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		int nCompanyID = 0;
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nCompanyID = pResultSet->GetInt("id");
				char szSql[512];
				memset(szSql, 0, sizeof(szSql));
				//in order to query group members ,do not reset usercnt here
				snprintf(szSql, sizeof(szSql)-1, "update IMGroup set status=1,updated=unix_timestamp(),version=version+1 where companyid=%d and status <> 1 and not exists(select 1 from IMGroupTemp where IMGroupTemp.groupid = IMGroup.groupid)", nCompanyID);
				bRet = m_pDBConn->ExecuteUpdate(szSql);
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		//update existing data
		strSql = "update IMGroup a inner join IMGroupTemp b on a.groupid=b.groupid set a.name=b.name,a.status=b.status,a.version=a.version+1,a.usercnt=b.usercnt,a.updated=b.updated,a.email=b.email where a.groupid=b.groupid and (a.name <> b.name or a.status <> b.status or a.usercnt <> b.usercnt or a.email is NULL or a.email <> b.email)";
		//printf("SQL: %s\n", strSql.c_str());
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//get company id for temp table
		strSql = "update IMGroupTemp a, IMCompany b set a.companyid=b.id where a.domainid=b.domainid";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		//insert new data
		strSql = "insert into IMGroup(groupid,companyid,name,type,usercnt,status,created,updated,email) select groupid,companyid,name,type,usercnt,status,created,updated,email from IMGroupTemp where not exists(select 1 from IMGroup where IMGroup.groupid =IMGroupTemp.groupid)";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
}


void  CDataSync::ImportGroupUser(LIST_GROUP_USER& refGroupUser)
{
	if (refGroupUser.size() > 0)
	{
		string strSql = "truncate table IMGroupMemberTemp";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		for (IT_LIST_GROUP_USER it = refGroupUser.begin(); it != refGroupUser.end(); ++it) //write data to temp table
		{
			strSql = "insert into IMGroupMemberTemp(groupuuid,useruuid,created,updated) values(?,?,?,?)";
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
			{
				int nIndex = 0;

				unsigned int nCurrentTime = time(NULL);
				pStmt->SetParam(nIndex++, it->m_strGroupID);
				pStmt->SetParam(nIndex++, it->m_strUserID);
				pStmt->SetParam(nIndex++, nCurrentTime);
				pStmt->SetParam(nIndex++, nCurrentTime);
				pStmt->ExecuteUpdate();
			}
			delete pStmt;
		}

		//delete duplicate data
		strSql = "delete from IMGroupMemberTemp  using IMGroupMemberTemp, (select max(id) as maxid,groupuuid,useruuid from IMGroupMemberTemp group by groupuuid,useruuid having count(*) > 1) as B where (IMGroupMemberTemp.groupuuid = B.groupuuid and IMGroupMemberTemp.useruuid=B.useruuid and IMGroupMemberTemp.id != B.maxid)";
		//printf("SQL: %s\n", strSql.c_str());
		bool bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//set group id and user id
		strSql = "update IMGroupMemberTemp a, IMGroup b set a.groupid=b.id where a.groupuuid=b.groupid";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		strSql = "update IMGroupMemberTemp a, IMUser b set a.userid = b.id where a.useruuid=b.userid";
		m_pDBConn->ExecuteUpdate(strSql.c_str());


		//delete removed data 
		strSql = "update IMGroupMember set status = 1,updated=unix_timestamp() where status <> 1 and exists(select 1 from IMGroup where IMGroup.type=3 and IMGroupMember.groupid=IMGroup.id) and not exists(select 1 from IMGroupMemberTemp where IMGroupMember.groupid= IMGroupMemberTemp.groupid and IMGroupMember.userid=IMGroupMemberTemp.userid)";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		//update existing data except field 'avatar'
		strSql = "update IMGroupMember a, IMGroupMemberTemp b set a.status = 0,a.updated=unix_timestamp() where a.groupid= b.groupid and a.userid=b.userid and a.status = 1";
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//insert new data
		strSql = "insert into IMGroupMember(groupid,userid,created,updated) select groupid,userid,created,updated from IMGroupMemberTemp where exists(select 1 from IMUser where IMUser.id=IMGroupMemberTemp.userid) and exists(select 1 from IMGroup where IMGroup.id= IMGroupMemberTemp.groupid) and not exists(select 1 from IMGroupMember where IMGroupMember.groupid =IMGroupMemberTemp.groupid and IMGroupMember.userid=IMGroupMemberTemp.userid)";
		//printf("SQL: %s\n", strSql.c_str());
		bRet = m_pDBConn->ExecuteUpdate(strSql.c_str());

		//update creator
		strSql = "update IMGroup a ,(select userid,groupid from IMGroupMember group by groupid) as b set a.creator=b.userid where a.type=3 and a.creator = 0 and a.id=b.groupid";
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}

}


void CDataSync::SetConnection(CDBConn* pInDBConn, CacheConn* pInGroupMember, CacheConn* pInCacheUser)
{
	m_pDBConn = pInDBConn;
	m_pCacheGroupMember = pInGroupMember;
	m_pCacheUser = pInCacheUser;
}


void CDataSync::SetUserStatus(uint32_t unUserID, int32_t unStatus)
{
	if (m_pCacheUser)
	{
		string strKey = "user_" + int2string(unUserID);
		string strStatus = int2string(unStatus);
		//m_pCacheUser->set(strKey, int2string(unStatus));
		log("Set user %s 's status: %s", strKey.c_str(), strStatus.c_str());
		m_pCacheUser->set(strKey, strStatus);
	}
}


int  CDataSync::GetUserInfo(string& strInputString, USER_INFO& UserInfo)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(strInputString, root))
	{
		string strResult = "";
		if (!root["resultCode"].isNull() && root["resultCode"].isString())
		{
			strResult = root["resultCode"].asString();
		}

		if (strResult != "0000")
		{
			return -1;
		}

		//if (!root["data"].isNull() && root["data"].isArray())
		if (!root["data"].isNull())
		{
			Json::Value msgData = root["data"];
			if (!msgData["userId"].isNull() && msgData["userId"].isString())
			{
				UserInfo.m_strUserID = msgData["userId"].asString();
			}

			if (!msgData["domainId"].isNull() && msgData["domainId"].isString())
			{
				UserInfo.m_strDomainID = msgData["domainId"].asString();
			}

			if (!msgData["userCode"].isNull() && msgData["userCode"].isString())
			{
				UserInfo.m_strUserCode = msgData["userCode"].asString();
			}
			else
			{
				UserInfo.m_strUserCode = "";
			}

			if (!msgData["username"].isNull() && msgData["username"].isString())
			{
				UserInfo.m_strUserName = msgData["username"].asString();
			}

			if (!msgData["fullName"].isNull() && msgData["fullName"].isString())
			{
				UserInfo.m_strName = msgData["fullName"].asString();
			}

			if (!msgData["password"].isNull() && msgData["password"].isString())
			{
				UserInfo.m_strPassword = msgData["password"].asString();
			}

			if (!msgData["originalPassword"].isNull() && msgData["originalPassword"].isString())
			{
				UserInfo.m_strOriginalPassword = msgData["originalPassword"].asString();
			}
			else
			{
				UserInfo.m_strOriginalPassword = "";
			}

			if (!msgData["historyPassword"].isNull() && msgData["historyPassword"].isString())
			{
				UserInfo.m_strHistoryPassword = msgData["historyPassword"].asString();
			}
			else
			{
				UserInfo.m_strHistoryPassword = "";
			}


			if (!msgData["isAdmin"].isNull() && msgData["isAdmin"].isInt())
			{
				UserInfo.m_nIsAdmin = msgData["isAdmin"].asInt();
			}
			else
			{
				UserInfo.m_nIsAdmin = 0;
			}

			if (!msgData["headPortrait"].isNull() && msgData["headPortrait"].isString())
			{
				UserInfo.m_strAvatarUrl = msgData["headPortrait"].asString();
			}
			else
			{
				UserInfo.m_strAvatarUrl = "";
			}

			if (!msgData["sex"].isNull() && msgData["sex"].isInt())
			{
				UserInfo.m_nSex = msgData["sex"].asInt();
			}
			else
			{
				UserInfo.m_nSex = 2;
			}

			time_t nBirthday = 0;
			if (!msgData["birthday"].isNull() && msgData["birthday"].isInt64())
			{
				time_t  nBirthday = msgData["birthday"].asInt64() / 1000;
			}

			char szBirthday[32];
			memset(szBirthday, 0, sizeof(szBirthday));
			strftime(szBirthday, sizeof(szBirthday)-1, "%Y-%m-%d", localtime(&nBirthday));
			UserInfo.m_strBirthday = szBirthday;

			if (!msgData["phone"].isNull() && msgData["phone"].isString())
			{
				UserInfo.m_strPhone = msgData["phone"].asString();
			}
			else
			{
				UserInfo.m_strPhone = "";
			}

			if (!msgData["remark"].isNull() && msgData["remark"].isString())
			{
				UserInfo.m_strRemark = msgData["remark"].asString();
			}
			else
			{
				UserInfo.m_strRemark = "";
			}

			if (!msgData["adminStatus"].isNull() && msgData["adminStatus"].isInt())
			{
				UserInfo.m_nAdminStatus = msgData["adminStatus"].asInt();
			}
			else
			{
				UserInfo.m_nAdminStatus = 0;
			}

			if (!msgData["contactsStatus"].isNull() && msgData["contactsStatus"].isInt())
			{
				UserInfo.m_nContactStatus = msgData["contactsStatus"].asInt();
			}
			else
			{
				UserInfo.m_nContactStatus = 1;
			}

			if (!msgData["status"].isNull() && msgData["status"].isInt())
			{
				UserInfo.m_nStatus = msgData["status"].asInt();
			}
			else
			{
				UserInfo.m_nStatus = 1;
			}

			if (!msgData["beginTime"].isNull() && msgData["beginTime"].isInt64())
			{
				UserInfo.m_nBeginTime = msgData["beginTime"].asInt64() / 1000;
			}
			else
			{
				UserInfo.m_nBeginTime = 0;
			}

			if (!msgData["expiredTime"].isNull() && msgData["expiredTime"].isInt64())
			{
				UserInfo.m_nExpiredTime = msgData["expiredTime"].asInt64() / 1000;
			}
			else
			{
				UserInfo.m_nExpiredTime = 0;
			}

			if (!msgData["updateTime"].isNull() && msgData["updateTime"].isInt64())
			{
				UserInfo.m_nUpdateTime = msgData["updateTime"].asInt64() / 1000;
			}
			else
			{
				UserInfo.m_nUpdateTime = 0;
			}

			if (!msgData["isIpLimit"].isNull() && msgData["isIpLimit"].isInt())
			{
				UserInfo.m_nIsIPLimit = msgData["isIpLimit"].asInt();
			}
			else
			{
				UserInfo.m_nIsIPLimit = 0;
			}

			if (!msgData["lastModfyTime"].isNull() && msgData["lastModfyTime"].isInt64())
			{
				UserInfo.m_nLastModifyTime = msgData["lastModfyTime"].isInt64() / 1000;
			}
			else
			{
				UserInfo.m_nLastModifyTime = 0;
			}

			if (!msgData["isLock"].isNull() && msgData["isLock"].isInt())
			{
				UserInfo.m_nLock = msgData["isLock"].asInt();
			}
			else
			{
				UserInfo.m_nLock = 0;
			}

			if (!msgData["lockIp"].isNull() && msgData["lockIp"].isString())
			{
				UserInfo.m_strLockIP = msgData["lockIp"].asString();
			}
			else
			{
				UserInfo.m_strLockIP = "";
			}

			if (!msgData["lockStartTime"].isNull() && msgData["lockStartTime"].isInt64())
			{
				UserInfo.m_nLockStartTime = msgData["lockStartTime"].asInt64();
			}
			else
			{
				UserInfo.m_nLockStartTime = 0;
			}

			if (!msgData["lockEndTime"].isNull() && msgData["lockEndTime"].isInt64())
			{
				UserInfo.m_nLockEndTime = msgData["lockEndTime"].asInt64();
			}
			else
			{
				UserInfo.m_nLockEndTime = 0;
			}

			if (!msgData["keyword"].isNull() && msgData["keyword"].isString())
			{
				UserInfo.m_strKeyword = msgData["keyword"].asString();
			}
			else
			{
				UserInfo.m_strKeyword = "";
			}

			//just for temporarily test
			if (!msgData["lowerPassword"].isNull() && msgData["lowerPassword"].isString())
			{
				UserInfo.m_strTmpPassword = msgData["lowerPassword"].asString();
			}
			else
			{
				UserInfo.m_strTmpPassword = "";
			}

			if (!msgData["productIds"] && msgData["productIds"].isArray())
			{
				int nProducts = msgData["productIds"].size();
				if (nProducts)
				{
					Json::Value Products = msgData["productIds"];
					int i = 0;

					//get products a user buys
					string strBuyProducts = "";
					for (; i < nProducts - 1; i++)
					{
						strBuyProducts += Products[i].asString() + ',';
					}
					strBuyProducts += Products[i].asString();
					UserInfo.m_strBuyProducts = strBuyProducts;
				}
			}
			else
			{
				UserInfo.m_strBuyProducts = "";
			}

			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}


void CDataSync::AddDomainUser(USER_INFO& UserInfo)
{
	string strSql = "select id from IMCompany where domainid='" + UserInfo.m_strDomainID + "'";
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	int nCompanyID = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			nCompanyID = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if (nCompanyID == 0)
	{
		log("domain does not exists: %s", UserInfo.m_strDomainID.c_str());
		return;
	}

	strSql = "select id from IMUserA where useruuid='" + UserInfo.m_strUserID + "'";
	pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	int nUserID = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			nUserID = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if (nUserID == 0)
	{

		strSql = "insert into IMUserA (useruuid,jobnumber,username,name,password,origpassword,histpassword,avatar,sex,birthday,phone,isadmin,adminstatus,contactstatus,status,begintime,";
		strSql += "expiredtime,updatedtime,isiplimit,lastmodifytime,islock,lockip,lockstarttime,lockendtime,keyword,remark,buyproduct,created,updated,companyid,tmppassword) ";
		strSql += "values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
			// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash

		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			pStmt->SetParam(nIndex++, UserInfo.m_strUserID);
			pStmt->SetParam(nIndex++, UserInfo.m_strUserCode);
			pStmt->SetParam(nIndex++, UserInfo.m_strUserName);
			pStmt->SetParam(nIndex++, UserInfo.m_strName);
			pStmt->SetParam(nIndex++, UserInfo.m_strPassword);
			pStmt->SetParam(nIndex++, UserInfo.m_strOriginalPassword);
			pStmt->SetParam(nIndex++, UserInfo.m_strHistoryPassword);
			pStmt->SetParam(nIndex++, UserInfo.m_strAvatarUrl);
			pStmt->SetParam(nIndex++, UserInfo.m_nSex);
			pStmt->SetParam(nIndex++, UserInfo.m_strBirthday);
			pStmt->SetParam(nIndex++, UserInfo.m_strPhone);
			pStmt->SetParam(nIndex++, UserInfo.m_nIsAdmin);
			pStmt->SetParam(nIndex++, UserInfo.m_nAdminStatus);
			pStmt->SetParam(nIndex++, UserInfo.m_nContactStatus);
			pStmt->SetParam(nIndex++, UserInfo.m_nStatus);
			pStmt->SetParam(nIndex++, UserInfo.m_nBeginTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nExpiredTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nUpdateTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nIsIPLimit);
			pStmt->SetParam(nIndex++, UserInfo.m_nLastModifyTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nLock);
			pStmt->SetParam(nIndex++, UserInfo.m_strLockIP);
			pStmt->SetParam(nIndex++, UserInfo.m_nLockStartTime);
			pStmt->SetParam(nIndex++, UserInfo.m_nLockEndTime);
			pStmt->SetParam(nIndex++, UserInfo.m_strKeyword);
			pStmt->SetParam(nIndex++, UserInfo.m_strRemark);
			pStmt->SetParam(nIndex++, UserInfo.m_strBuyProducts);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCompanyID);
			pStmt->SetParam(nIndex++, UserInfo.m_strTmpPassword);
			pStmt->ExecuteUpdate();
			uint32_t unUserId = pStmt->GetInsertId();
			log("Add a new user; %d", unUserId);
			SetUserStatus(unUserId, UserInfo.m_nStatus);
		}
		delete pStmt;
	}
	else
	{
		log("User %s is already exists", UserInfo.m_strUserID.c_str());
	}
}


void CDataSync::UpdateDomainUser(USER_INFO& UserInfo)
{
	string strSql = "update IMUserA set jobnumber = '" + UserInfo.m_strUserCode + "',username = '" + UserInfo.m_strUserName + "',name = '" + UserInfo.m_strName;
	strSql += "',password = '" + UserInfo.m_strPassword + "',origpassword = '" + UserInfo.m_strOriginalPassword + "',histpassword = '" + UserInfo.m_strHistoryPassword;
	strSql += "',avatar = '" + UserInfo.m_strAvatarUrl + "',sex=" + int2string(UserInfo.m_nSex) + ",birthday ='" + UserInfo.m_strBirthday + "',phone = '" + UserInfo.m_strPhone;
	strSql += "',isadmin=" + int2string(UserInfo.m_nIsAdmin) + ",adminstatus = " + int2string(UserInfo.m_nAdminStatus) + ",contactstatus = " + int2string(UserInfo.m_nContactStatus);
	strSql += ",status= " + int2string(UserInfo.m_nStatus) + ",begintime=" + int2string(UserInfo.m_nBeginTime) + ",expiredtime=" + int2string(UserInfo.m_nExpiredTime);
	strSql += ",updatedtime=" + int2string(UserInfo.m_nUpdateTime) + ",isiplimit=" + int2string(UserInfo.m_nIsIPLimit) + ",lastmodifytime=" + int2string(UserInfo.m_nLastModifyTime);
	strSql += ",islock =" + int2string(UserInfo.m_nLock) + ",lockip='" + UserInfo.m_strLockIP + "',lockstarttime=" + int2string(UserInfo.m_nLockStartTime) + ",lockendtime=" + int2string(UserInfo.m_nLockEndTime);
	strSql += ",keyword='" + UserInfo.m_strKeyword + "',remark='" + UserInfo.m_strRemark + "',tmppassword='" + UserInfo.m_strTmpPassword + "',updated=unix_timestamp() where useruuid='" + UserInfo.m_strUserID + "'";
	log("SQL for updating a user info: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
	CheckUser(UserInfo.m_strUserID);
}


void CDataSync::DeleteDomainUser(string& strUserID)
{
	string strSql = "update IMUserA set status=-1,updated=unix_timestamp() where useruuid='" + strUserID + "'";
	log("SQL for deleting a user: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
	CheckUser(strUserID);

}


int CDataSync::GetUserDetails(string& strInputString, LIST_USER_DETAILS& listUserDetails)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(strInputString, root))
	{
		string strResult = "";
		if (!root["resultCode"].isNull() && root["resultCode"].isString())
		{
			strResult = root["resultCode"].asString();
		}

		if (strResult != "0000")
		{
			return -1;
		}

		if (!root["data"].isNull())
		{
			Json::Value msgData = root["data"];
			int nCount = msgData.size();
			for (int i = 0; i < nCount; i++)
			{
				USER_DETAILS UserDetails;
				if (!msgData[i]["userDetail"].isNull() && msgData[i]["userDetail"].isArray())
				{
					Json::Value Details = msgData[i]["userDetail"];
					if (!Details["userDetailsId"].isNull() && Details["userDetailsId"].isString())
					{
						UserDetails.m_strUUID = Details["userDetailsId"].asString();
					}

					if (!Details["userId"].isNull() && Details["userId"].isString())
					{
						UserDetails.m_strUserUUID = Details["userId"].asString();
					}

					if (!Details["type"].isNull() && Details["type"].isInt())
					{
						UserDetails.m_nType = Details["type"].asInt();
					}

					if (!Details["title"].isNull() && Details["title"].isString())
					{
						UserDetails.m_strTitle = Details["title"].asString();
					}

					if (!Details["content"].isNull() && Details["content"].isString())
					{
						UserDetails.m_strContent = Details["content"].asString();
					}

					listUserDetails.push_back(UserDetails);
				}
			}

		}
	}
	return 1;
}


void CDataSync::AddUserDetails(LIST_USER_DETAILS& listUserDetails)
{
	string strSql = "insert into UserDetails (uuid,useruuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
	// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash

	for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	{
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			USER_DETAILS UserDetails = *it;
			pStmt->SetParam(nIndex++, UserDetails.m_strUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_strUserUUID);
			pStmt->SetParam(nIndex++, UserDetails.m_nType);
			pStmt->SetParam(nIndex++, UserDetails.m_strTitle);
			pStmt->SetParam(nIndex++, UserDetails.m_strContent);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->ExecuteUpdate();
		}
		delete pStmt;
	}
}


void CDataSync::SetUserDetails(LIST_USER_DETAILS& listUserDetails)
{
	if (listUserDetails.size() > 0)
	{
		string strSql = "truncate table UserDetailsTemp";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		strSql = "insert into UserDetailsTemp (uuid,useruuid,type,title,content,created,updated) values(?,?,?,?,?,?,?)";
		// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
		string strUserId = "";
		for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
		{
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
			{
				int nIndex = 0;
				unsigned int nCurrenTime = time(NULL);
				USER_DETAILS UserDetails = *it;
				strUserId = UserDetails.m_strUserUUID;
				pStmt->SetParam(nIndex++, UserDetails.m_strUUID);
				pStmt->SetParam(nIndex++, UserDetails.m_strUserUUID);
				pStmt->SetParam(nIndex++, UserDetails.m_nType);
				pStmt->SetParam(nIndex++, UserDetails.m_strTitle);
				pStmt->SetParam(nIndex++, UserDetails.m_strContent);
				pStmt->SetParam(nIndex++, nCurrenTime);
				pStmt->SetParam(nIndex++, nCurrenTime);
				pStmt->ExecuteUpdate();
			}
			delete pStmt;
		}

		strSql = "delete from UserDetailsTemp using UserDetailsTemp,(select max(id) as maxid, uuid from UserDetailsTemp group by uuid having count(*) > 1) as b where UserDetailsTemp.uuid=b.uuid and UserDetailsTemp.id <> b.maxid";
		m_pDBConn->ExecuteQuery(strSql.c_str());

		strSql = "update UserDetails a, UserDetailsTemp b set a.type=b.type,a.title=b.title,a.content=b.content,a.updated=unix_timestamp() where (a.uuid=b.uuid) and (a.type <> b.type or a.title <> b.title or a.content <> b.content)";
		m_pDBConn->ExecuteQuery(strSql.c_str());

		strSql = "insert into UserDetails (uuid,useruuid,type,title,content,created,updated) select uuid,useruuid,type,title,content,created,updated from UserDetailsTemp a where not exists(select 1 from UserDetails b where a.uuid=b.uuid);";
		m_pDBConn->ExecuteQuery(strSql.c_str());

		strSql = "delete from UserDetails where useruuid='" + strUserId + "' and not exists(select 1 from UserDetailsTemp where UserDetailsTemp.uuid = UserDetails.uuid)";
		m_pDBConn->ExecuteQuery(strSql.c_str());

		strSql = "update IMUserA set updated=unix_timestamp() where useruuid='" + strUserId + "'";
		m_pDBConn->ExecuteQuery(strSql.c_str());
	}
}


void CDataSync::UpdateUserDetails(LIST_USER_DETAILS& listUserDetails)
{
	for (IT_LIST_USER_DETAILS it = listUserDetails.begin(); it != listUserDetails.end(); it++)
	{
		USER_DETAILS UserDetails = *it;
		string strSql = "update UserDetails set type = " + int2string(UserDetails.m_nType) + ",title=' " + UserDetails.m_strTitle + "',content='" + UserDetails.m_strContent + "'updated=unix_timestamp() where uuid='" + UserDetails.m_strUUID + "'";
		log("SQL for updating a user details: %s", strSql.c_str());
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
}


int CDataSync::GetDepartInfo(string& strInputString, DEPARTMENT_INFO& DepartInfo)
{
	
	{
		Json::Reader Reader;
		Json::Value Root;
		if (Reader.parse(strInputString, Root))
		{
			string strResultCode = "";
			if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
			{
				strResultCode = Root["resultCode"].asString();
			}

			if (strResultCode != "0000")
			{
				return -1;
			}

			if (Root["data"].isNull())
			{
				return -1;
			}

			Json::Value Department = Root["data"];
			if (!Department["orgId"].isNull() && Department["orgId"].isString())
			{
				DepartInfo.m_strDepartmentID = Department["orgId"].asString();
			}
			else
			{
				DepartInfo.m_strDepartmentID = "";
			}

			if (!Department["domainId"].isNull() && Department["domainId"].isString())
			{
				DepartInfo.m_strDomainID = Department["domainId"].asString();
			}
			else
			{
				DepartInfo.m_strDomainID = "";
			}

			if (!Department["orgName"].isNull() && Department["orgName"].isString())
			{
				DepartInfo.m_strDepartmentName = Department["orgName"].asString();
			}
			else
			{
				DepartInfo.m_strDepartmentName = "";
			}

			if (!Department["parentId"].isNull() && Department["parentId"].isString())
			{
				DepartInfo.m_strParentDepartmentID = Department["parentId"].asString();
				if (DepartInfo.m_strParentDepartmentID == "-1")
				{
					DepartInfo.m_strParentDepartmentID = "";
				}
			}
			else
			{
				DepartInfo.m_strParentDepartmentID = "";
			}

			if (!Department["level"].isNull() && Department["level"].isInt())
			{
				DepartInfo.m_nLevel = Department["level"].asInt();
			}
			else
			{
				DepartInfo.m_nLevel = 0;
			}

			if (!Department["sort"].isNull() && Department["sort"].isInt())
			{

				DepartInfo.m_nPriority = Department["sort"].asInt();
			}
			else
			{
				DepartInfo.m_nPriority = 65535;
			}

			if (!Department["path"].isNull() && Department["path"].isString())
			{
				DepartInfo.m_strDepartPath = Department["path"].asString();
			}
			else
			{
				DepartInfo.m_strDepartPath = "";
			}

			if (!Department["isOrg"].isNull() && Department["isOrg"].isInt())
			{
				DepartInfo.m_nIsDepart = Department["isOrg"].asInt();
			}
			else
			{
				DepartInfo.m_nIsDepart = 0;
			}

			if (!Department["status"].isNull() && Department["status"].isInt())
			{
				DepartInfo.m_nStatus = Department["status"].asInt();
			}
			else
			{
				DepartInfo.m_nStatus = 0;
			}

			if (!Department["contactsStatus"].isNull() && Department["contactsStatus"].isInt())
			{
				DepartInfo.m_nContactStatus = Department["contactsStatus"].asInt();
			}
			else
			{
				DepartInfo.m_nContactStatus = 1;
			}

			if (DepartInfo.m_strParentDepartmentID == "-2") //unallocated department
			{
				DepartInfo.m_strDepartmentID = DepartInfo.m_strDomainID + DepartInfo.m_strParentDepartmentID;
			}
		}
		return 1;
	}
	
	return 1;
}


void CDataSync::AddDomainDepart(DEPARTMENT_INFO& DepartInfo)
{
	string strSql = "select id from IMCompany where domainid='" + DepartInfo.m_strDomainID + "'";
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	int nCompanyID = 0;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			nCompanyID = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if (nCompanyID == 0)
	{
		log("domain does not exists: %s", DepartInfo.m_strDomainID.c_str());
		return;
	}

	strSql = "select uuid from IMDepartA where uuid='" + DepartInfo.m_strDepartmentID + "'";
	pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	bool bExists = false;
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			bExists = true;
		}
		delete pResultSet;
		pResultSet = NULL;
	}


	if (!bExists)
	{
		strSql = "insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,created,updated,level,isdepart,departpath,contactstatus)values(?,?,?,?,?,?,?,?,?,?,?,?)";
	// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			pStmt->SetParam(nIndex++, DepartInfo.m_strDepartmentID);
			pStmt->SetParam(nIndex++, nCompanyID);
			pStmt->SetParam(nIndex++, DepartInfo.m_strDepartmentName);
			pStmt->SetParam(nIndex++, DepartInfo.m_nPriority);
			pStmt->SetParam(nIndex++, DepartInfo.m_strParentDepartmentID);
			pStmt->SetParam(nIndex++, DepartInfo.m_nStatus);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, DepartInfo.m_nLevel);
			pStmt->SetParam(nIndex++, DepartInfo.m_nIsDepart);
			pStmt->SetParam(nIndex++, DepartInfo.m_strDepartPath);
			pStmt->SetParam(nIndex++, DepartInfo.m_nContactStatus);
			pStmt->ExecuteUpdate();
		}
		delete pStmt;
	}
	else
	{
		log("No company for domain: %s", DepartInfo.m_strDomainID.c_str());
	}
}


void CDataSync::UpdateDomainDepart(DEPARTMENT_INFO& DepartInfo)
{
	string strSql = "update IMDepartA set departname='" + DepartInfo.m_strDepartmentName + "',priority=" + int2string(DepartInfo.m_nPriority) + ",parentuuid='" + DepartInfo.m_strParentDepartmentID + "',";
	strSql += "status=" + int2string(DepartInfo.m_nStatus) + ",updated=unix_timestamp(),level=" + int2string(DepartInfo.m_nLevel) + ",isdepart=" + int2string(DepartInfo.m_nIsDepart);
	strSql += ",departpath='" + DepartInfo.m_strDepartPath + "',contactstatus=" + int2string(DepartInfo.m_nContactStatus) + " where uuid='" + DepartInfo.m_strDepartmentID + "'";
	log("SQL for updating department: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteDomainDepart(string& strDepartID)
{
	string strSql = "update IMDepartA set status=-1,updated=unix_timestamp() where uuid='" + strDepartID + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


int CDataSync::GetPositionInfo(string& strInputString, POSITION_INFO& PositionInfo)
{
	Json::Reader Reader;
	Json::Value Root;
	if (Reader.parse(strInputString, Root))
	{
		if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
		{
			string strResult = Root["resultCode"].asString();
			if (strResult != "0000")
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}

		if (!Root["data"].isNull())
		{
			Json::Value msgData = Root["data"];
			if (!msgData["positionId"].isNull() && msgData["positionId"].isString())
			{
				PositionInfo.m_strPositionID = msgData["positionId"].asString();
			}
			else
			{
				PositionInfo.m_strPositionID = "";
			}

			if (!msgData["positionName"].isNull() && msgData["positionName"].isString())
			{
				PositionInfo.m_strPositionName = msgData["positionName"].asString();
			}
			else
			{
				PositionInfo.m_strPositionName = "";
			}

			if (!msgData["domainId"].isNull() && msgData["domainId"].isString())
			{
				PositionInfo.m_strDomainID = msgData["domainId"].asString();
			}
			else
			{
				PositionInfo.m_strDomainID = "";
			}

			if (!msgData["type"].isNull() && msgData["type"].isString())
			{
				PositionInfo.m_strPositionType = msgData["type"].asString();
			}
			else
			{
				PositionInfo.m_strPositionType = "";
			}

			if (!msgData["remark"].isNull() && msgData["remark"].isString())
			{
				PositionInfo.m_strRemark = msgData["reamrk"].asString();
			}
			else
			{
				PositionInfo.m_strRemark = "";
			}

			if (!msgData["sort"].isNull() && msgData["sort"].isInt())
			{
				PositionInfo.m_nSort = msgData["sort"].asInt();
			}
			else
			{
				PositionInfo.m_nSort = 0;
			}

			if (!msgData["status"].isNull() && msgData["status"].isInt())
			{
				PositionInfo.m_nStatus = msgData["status"].asInt();
			}
			else
			{
				PositionInfo.m_nStatus = 0;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 1;
}

void CDataSync::AddDomainPosition(POSITION_INFO& PositionInfo)
{
	string strSql = "select id from IMCompany where domainid='" + PositionInfo.m_strDomainID + "'";
	int nCompanyID = 0;

	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			nCompanyID = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if (nCompanyID > 0)
	{
		string strSql = "insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated)values(?,?,?,?,?,?,?,?,?)";
		// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);

			pStmt->SetParam(nIndex++, PositionInfo.m_strPositionID);
			pStmt->SetParam(nIndex++, nCompanyID);
			pStmt->SetParam(nIndex++, PositionInfo.m_strPositionName);
			pStmt->SetParam(nIndex++, PositionInfo.m_strPositionType);
			pStmt->SetParam(nIndex++, PositionInfo.m_nStatus);
			pStmt->SetParam(nIndex++, PositionInfo.m_strRemark);
			pStmt->SetParam(nIndex++, PositionInfo.m_nSort);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->ExecuteUpdate();
		}
		delete pStmt;
	}
}


void CDataSync::UpdateDomainPosition(POSITION_INFO& PositionInfo)
{
	string strSql = "update Position set posname='" + PositionInfo.m_strPositionName + "',type='" + PositionInfo.m_strPositionType + "',status=" + int2string(PositionInfo.m_nStatus);
	strSql += ",remark='" + PositionInfo.m_strRemark + "',sort = " + int2string(PositionInfo.m_nSort) + ",updated=unix_timestamp() where uuid='" + PositionInfo.m_strPositionID + "'";
	log("SQL for updating position: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteDomainPosition(string& strPositionID)
{
	string strSql = "update Position set status=-1,updated=unix_timestamp() where uuid='" + strPositionID + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::AddUserPosition(string& strDepartID, string& strPositionID, string& strUserID)
{
	if (strPositionID == "")
	{
		strPositionID = "00000000000000000000000000000000";
	}

	string strSql = "select status from IMUserPositionA where useruuid='" + strUserID+ "' and departuuid='" + strDepartID + "' and posuuid='" + strPositionID + "'";
	int nStatus = 1;
	log("SQL for getting status: %s", strSql.c_str());
	CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if(pResultSet)
	{
		while(pResultSet->Next())
		{
			nStatus = pResultSet->GetInt("status");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if(nStatus == 0)
	{
		return;
	}
	else if(nStatus == -1) //update(set status 0)
	{
		strSql = "update IMUserPositionA set status = 0,updated=unix_timestamp() where  posuuid='" + strPositionID + "' and departuuid='" + strDepartID + "' and useruuid = '" + strUserID + "'";
		log("SQL for updating position status; %s", strSql.c_str());
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
	else if(nStatus == 1) //new position
	{
		//if the user no position previously?
		string strSql = "select count(*) as totalcount from IMUserPositionA where useruuid='" + strUserID+ "' and departuuid='" + strDepartID + "' and posuuid='00000000000000000000000000000000'";
		int nCount = 0;
		pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if(pResultSet)
		{
			while(pResultSet->Next())
			{
				nCount = pResultSet->GetInt("totalcount");
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		if(nCount > 0) //update
		{
			strSql = "update IMUserPositionA set status = 0,updated=unix_timestamp(), posuuid='" + strPositionID + "' where departuuid='" + strDepartID + "' and useruuid = '" + strUserID + "'";
			log("SQL for updating position; %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());
		}
		else
		{
			strSql = "insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select companyid,'" + strDepartID + "','" + strPositionID + "','" + strUserID + "',";
			strSql += "unix_timestamp(), unix_timestamp() from IMDepartA where uuid='" + strDepartID + "'";
			log("SQL for inserting a record for IMUserPositionA: %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());
		}
	}


	//strSql = "insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select companyid,'" + strDepartID + "','" + strPositionID + "','" + strUserID + "',";
	//strSql += "unix_timestamp(), unix_timestamp() from IMDepartA where uuid='" + strDepartID + "'";
	//log("SQL for inserting a record for IMUserPositionA: %s", strSql.c_str());

	//m_pDBConn->ExecuteUpdate(strSql.c_str());

	////if the user no position previously
	//if (strPositionID != "00000000000000000000000000000000")
	//{
	//	strSql = "update IMUserPositionA set status = -1,updated=unix_timestamp() where posuuid='00000000000000000000000000000000' and useruuid = '" + strUserID + "' and departuuid='" + strDepartID + "'";
	//	log("SQL for updating position status: %s", strSql.c_str());
	//	m_pDBConn->ExecuteUpdate(strSql.c_str());
	//}

}


void CDataSync::DeleteUserPosition(string& strDepartID, string& strPositionID, string& strUserID)
{
	//if no position after deleting ,we should set '' for the user
	string strSql = "update IMUserPositionA set status = -1, updated=unix_timestamp() where departuuid='" + strDepartID + "' and posuuid='" + strPositionID + "' and useruuid='" + strUserID + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
	log("SQL for deleting a record for IMUserPositionA: %s", strSql.c_str());
}


void CDataSync::GetUserProducts(string& strInputString)
{
	Json::Reader Reader;
	Json::Value Root;
	if (Reader.parse(strInputString, Root))
	{
		if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
		{
			string strResult = Root["resultCode"].asString();
			if (strResult != "0000")
			{
				return;
			}
		}
		else
		{
			return;
		}

		if (!Root["data"].isNull() && Root["data"].isArray())
		{
			Json::Value msgData = Root["data"];
			int nCount = msgData.size();
			string strUserID = "";
			string strProducts = "";
			if (nCount > 0)
			{
				int i = 0;
				for (; i < nCount - 1; i++)
				{
					int nStatus = 0;
					Json::Value Product = msgData[i];
					if (!Product["status"].isNull() && Product["status"].isInt())
					{
						nStatus = Product["status"].asInt();
					}

					if (nStatus)
					{
						if (!Product["productId"].isNull() && Product["productId"].isString())
						{
							if (strProducts == "")
								strProducts = Product["productId"].asString();
							else
								strProducts += "," + Product["productId"].asString();
						}
					}
				}


				Json::Value Product = msgData[i];
				if (!Product["userId"].isNull() && Product["userId"].isString())
				{
					strUserID = Product["userId"].asString();		
				}

				int nStatus = 0;
				//Json::Value Product = msgData[i];
				if (!Product["status"].isNull() && Product["status"].isInt())
				{
					nStatus = Product["status"].asInt();
				}

				if (nStatus)
				{
					if (!Product["productId"].isNull() && Product["productId"].isString())
					{
						if (strProducts == "")
							strProducts = Product["productId"].asString();
						else
							strProducts += "," + Product["productId"].asString();
					}
				}
				string strSql = "update IMUserA set buyproduct='" + strProducts + "',updated=unix_timestamp() where useruuid='" + strUserID +"'";
				log("SQL for updating IMUserA: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());
			}
		}
	}
}


void CDataSync::DeleteDpeartmentByDepartmentID(string& strDepartID)
{
	string strSql = "update IMDepartA a, IMUserPositionA b set a.status = -1, a.updated = unix_timestamp(), b.status = -1, b.updated = unix_timestamp() where a.uuid = b.departuuid and a.uuid = '" + strDepartID + "'";
	log("SQL for deleting department: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeletePositionByPositionID(string& strPositionID)
{
	string strSql = "update Position a, IMUserPositionA b set a.status = -1, a.updated = unix_timestamp(), b.status = -1, b.updated = unix_timestamp() where a.uuid = b.posuuid and a.uuid = '" + strPositionID + "'";
	log("SQL for deleting position: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeletePositionByDomainID(string& strDomainID)
{
	string strSql = "update Position a, IMUserPositionA b, IMCompany c set a.status = -1, a.updated=unix_timestamp(), b.status=-1,b.updated=unix_timestamp() where a.companyid=b.companyid and c.domainid ='" + strDomainID + "'" + " and (a.companyid=c.id)";
	log("SQL for  deleting position for a domain: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteDepartmentAndPosition(string& strDepartID, string& strPositionID)
{
	string strSql = "update IMUserPositionA  set status = -1, updated=unix_timestamp() where departuuid='" + strDepartID + "' and posuuid='" + strPositionID + "'";
	log("SQL for deleting position for a department and position: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteDepartIDAndUserID(string& strOrgID, string& strUserID)
{
	string strSql = "update IMUserPositionA  set status = -1, updated=unix_timestamp() where departuuid='" + strOrgID + "' and useruuid='" + strUserID + "'";
	log("SQL for deleting position for a department and user: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeletePositionIdAndUserId(string& strPositionId, string& strUserId)
{
	string strSql = "update IMUserPositionA  set status = -1, updated=unix_timestamp() where posuuid='" + strPositionId + "' and useruuid='" + strUserId + "'";
	log("SQL for deleting position for a position and user: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());

}


void CDataSync::GetDomainInfo(string& strInputString, DOMAIN_INFO& DomainInfo)
{
	Json::Reader Reader;
	Json::Value Root;
	
	if (Reader.parse(strInputString, Root))
	{
		string strResult = "";
		if (Root["resultCode"].isNull() && Root["resultCode"].isString())
		{
			strResult = Root["resultCode"].asString();
			if (strResult != "0000")
			{
				return;
			}
		}

		if (Root["data"].isNull())
		{
			return;
		}

		Json::Value msgData = Root["data"];

		if (!msgData["domainId"].isNull() && msgData["domainId"].isString())
		{
			DomainInfo.m_strDomainID = msgData["domainId"].asString();
		}
		else
		{
			DomainInfo.m_strDomainID = "";
		}

		if (!msgData["domainName"].isNull() && msgData["domainName"].isString())
		{
			DomainInfo.m_strDomainName = msgData["domainName"].asString();
		}
		else
		{
			DomainInfo.m_strDomainName = "";
		}

		if (!msgData["company"].isNull() && msgData["company"].isString())
		{
			DomainInfo.m_strCompanyName = msgData["company"].asString();
		}
		else
		{
			DomainInfo.m_strCompanyName = "";
		}

		if (!msgData["status"].isNull() && msgData["status"].isInt())
		{
			DomainInfo.m_nStatus = msgData["status"].asInt();
		}
		else
		{
			DomainInfo.m_nStatus = 0;
		}

		if (!msgData["serverName"].isNull() && msgData["serverName"].isString())
		{
			DomainInfo.m_strSyncServer = msgData["serverName"].asString();
		}
		else
		{
			DomainInfo.m_strSyncServer = "";
		}
	}
}


void CDataSync::AddNewDomain(DOMAIN_INFO& DomainInfo)
{
	int nCompanyID = 0;
	string strSql = "select id from IMCompany where domainid = '" + DomainInfo.m_strDomainID + "'";
	CResultSet  *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			nCompanyID = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	if (nCompanyID == 0)
	{
		strSql = "insert into IMCompany(domainid,domainname,companyname,status,created,updated,syncserver)values(?,?,?,?,?,?,?)";

		// 必须在释放连接前delete CPrepareStatement对象，否则有可能多个线程操作mysql对象，会crash
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;
			unsigned int nCurrenTime = time(NULL);
			int nStatus = DomainInfo.m_nStatus;
			pStmt->SetParam(nIndex++, DomainInfo.m_strDomainID);
			pStmt->SetParam(nIndex++, DomainInfo.m_strDomainName);
			pStmt->SetParam(nIndex++, DomainInfo.m_strCompanyName);
			pStmt->SetParam(nIndex++, nStatus);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, nCurrenTime);
			pStmt->SetParam(nIndex++, DomainInfo.m_strSyncServer);
			pStmt->ExecuteUpdate();
		}
		delete pStmt;
	}
	else
	{
		strSql = "update IMCompany set status = " + int2string(DomainInfo.m_nStatus);
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
}


void CDataSync::UpdateDomain(DOMAIN_INFO& DomainInfo)
{
	string strSql = "update IMCompany set domainname='" + DomainInfo.m_strDomainName + "',companyname='" + DomainInfo.m_strCompanyName + "',status=" + int2string(DomainInfo.m_nStatus);
	strSql += ",syncserver='" + DomainInfo.m_strSyncServer + "',updated=unix_timestamp() where domainid='" + DomainInfo.m_strDomainID + "'";
	log("SQL for updating domain: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


int  CDataSync::GetGroupInfo(string& strInputString, DOMAIN_GROUP& GroupInfo, list<string>& listUsers)
{
	Json::Reader Reader;
	Json::Value Root;
	int nResult = -1;
	try
	{
		if (Reader.parse(strInputString, Root))
		{
			if (!Root["groupId"].isNull() && Root["groupId"].isString())
			{
				GroupInfo.m_strGroupID = Root["groupId"].asString();
			}

			if (!Root["domainId"].isNull() && Root["domainId"].isString())
			{
				GroupInfo.m_strDomainID = Root["domainId"].asString();
			}

			if (!Root["groupName"].isNull() && Root["groupName"].isString())
			{
				GroupInfo.m_strGroupName = Root["groupName"].asString();
			}

			if (!Root["groupEmail"].isNull() && Root["groupEmail"].isString())
			{
				GroupInfo.m_strEmail = Root["groupEmail"].asString();
			}

			if (!Root["status"].isNull() && Root["status"].isInt())
			{
				GroupInfo.m_nStatus = Root["status"].asInt();
				if (GroupInfo.m_nStatus == 1)
				{
					GroupInfo.m_nStatus = 0;
				}
				else if (GroupInfo.m_nStatus == 0)
				{
					GroupInfo.m_nStatus = 1;
				}
			}

			nResult = 1;
			/*log("INDEX:%d\tGROUP ID: %s\tDOMAIN ID: %s\tGROUP NAME: %s\tEMAIL; %s\tUSER COUNT: %d\tSTATUS: %d\n", i + 1, DomainGroup.m_strGroupID.c_str(), DomainGroup.m_strDomainID.c_str(),
				DomainGroup.m_strGroupName.c_str(), DomainGroup.m_strEmail.c_str(), DomainGroup.m_nUserCount, DomainGroup.m_nStatus);*/
			if (!Root["userIds"].isNull() && Root["userIds"].isArray())
			{
				Json::Value GroupUsers = Root["userIds"];
				int nUsers = GroupUsers.size();;
				for (int i = 0; i < nUsers; i++)
				{
					string strUserID = GroupUsers[i].asString();
					log("USER ID: %s", strUserID.c_str());
					listUsers.push_back(strUserID);
					/*if (!GroupUsers[i]["userId"].isNull() && GroupUsers[i]["userId"].isString())
					{
						string strUserID = GroupUsers[i]["userId"].asString();
						listUsers.push_back(strUserID);
					}*/
				}
				GroupInfo.m_nUserCount = nUsers;
			}
			else
			{
				GroupInfo.m_nUserCount = 0;
			}
		}
		else
		{
			log("Invalid json data");
		}
	}
	catch (std::runtime_error msg)
	{
		log("error message");
	}
	catch (exception& Exception)
	{
		log("Exception occured: %s", Exception.what());
	}

	return nResult;
}


void CDataSync::AddNewMailGroupAndMembers(DOMAIN_GROUP& GroupInfo, list<string>& listUsers)
{
	int nCompanyID = GetCompanyByDomain(GroupInfo.m_strDomainID);

	if (nCompanyID != 0)
	{

		int nGroupID = 0;
		string strSql = "BEGIN";  //start the transaction
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		strSql = "select id from IMGroup where email = '" + GroupInfo.m_strEmail + "'";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nGroupID = pResultSet->GetInt("id");
			}
			delete pResultSet;
			pResultSet = NULL;
		}


		unsigned int nCurrentTime = time(NULL);

		if (nGroupID == 0)
		{
			strSql = "insert into IMGroup(groupid,companyid,name,type,usercnt,status,created,updated,email) values(?,?,?,?,?,?,?,?,?)";
			CPrepareStatement* pStmt = new CPrepareStatement();
			if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
			{
				int nIndex = 0;

				int nType = 3; //mail group
				int nUserCount = GroupInfo.m_nUserCount;
				int nStatus = GroupInfo.m_nStatus;
				pStmt->SetParam(nIndex++, GroupInfo.m_strGroupID);
				pStmt->SetParam(nIndex++, nCompanyID);
				pStmt->SetParam(nIndex++, GroupInfo.m_strGroupName);
				pStmt->SetParam(nIndex++, nType);
				pStmt->SetParam(nIndex++, nUserCount);
				pStmt->SetParam(nIndex++, nStatus);
				pStmt->SetParam(nIndex++, nCurrentTime);
				pStmt->SetParam(nIndex++, nCurrentTime);
				pStmt->SetParam(nIndex++, GroupInfo.m_strEmail);
				pStmt->ExecuteUpdate();
				nGroupID = pStmt->GetInsertId();
			}

			delete pStmt;
			pStmt = NULL;
		}

		//insert group members
		if (nGroupID > 0 && listUsers.size() > 0)
		{
			for (list<string>::iterator it = listUsers.begin(); it != listUsers.end(); it++)
			{
				strSql = "insert into IMGroupMember(groupid,userid,status,created,updated) select " + int2string(nGroupID) + ",id, 0, " + int2string(nCurrentTime) + ",";
				strSql += int2string(nCurrentTime) + " from IMUserA where useruuid='" + *it + "' and status = 1 and not exists(select 1 from IMGroupMember where groupid=" + int2string(nGroupID) + " and userid=IMUserA.id)";
				log("SQL for inserting users: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());
			}
			//update creator
			strSql = "update IMGroup a ,(select max(userid) as maxuid,groupid,count(*) as totalcount from IMGroupMember where status = 0 group by groupid) as b set a.creator=b.maxuid,a.usercnt=b.totalcount where a.type=3 and a.creator = 0 and a.id=b.groupid and a.id=" + int2string(nGroupID);
			m_pDBConn->ExecuteUpdate(strSql.c_str());
		}

		strSql = "COMMIT";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		SetGroupUserStatus(GroupInfo.m_strGroupID); //set cached group user
	}
	else
	{
		log("%s does not exists.............", GroupInfo.m_strDomainID.c_str());
	}
}


//void CDataSync::GetUserAlias(string& strInputString, LIST_USER_ALIAS& listAlias)
//{
//
//
//}
//
//
//void CDataSync::AddUserAlias(LIST_USER_ALIAS& listAlias)
//{
//
//}


void CDataSync::UpdateMailGroup(DOMAIN_GROUP& GroupInfo)
{
	int nCompanyID = GetCompanyByDomain(GroupInfo.m_strDomainID);
	if (nCompanyID != 0)
	{
		string strSql = "BEGIN";  //start the transaction
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		strSql = "update IMGroup set name='" + GroupInfo.m_strGroupName + "',status=" + int2string(GroupInfo.m_nStatus) + ",email='" + GroupInfo.m_strEmail + "',";
		strSql += "updated=unix_timestamp(), version=version+1 where groupid='" + GroupInfo.m_strGroupID + "'";
		log("SQL for updating mail group:%s", strSql.c_str());
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		if (GroupInfo.m_nStatus == 1)
		{
			strSql = "update IMGroupMember a, IMGroup b set a.status = 1, a.updated=unix_timestamp(),b.usercnt = 0 where a.groupid=b.id and b.groupid='" + GroupInfo.m_strGroupID + "'";
			log("SQL for deleting a group; %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());
			
			SetGroupUserStatus(GroupInfo.m_strGroupID); //set cached group user
		}

		strSql = "COMMIT";  //start the transaction
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
	else
	{
		log("domain %s does not exist", GroupInfo.m_strDomainID.c_str());
	}
}


void CDataSync::DeleteMailGroup(string& strGroupID)
{
	string strSql = "update IMGroupMember a, IMGroup b set a.status = 1, a.updated=unix_timestamp(),b.status=1,b.userCnt = 0,version=version+1, b.updated=unix_timestamp() where a.groupid=b.id and b.groupid='" + strGroupID + "'";
	log("SQL from deleting a group: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	SetGroupUserStatus(strGroupID); //set cached group user
}



int CDataSync::GetCompanyByDomain(string& strDomainID)
{
	int nCompanyID = 0;
	string strSql = "select id from IMCompany where domainid = '" + strDomainID + "'";
	log("SQL for getting company id: %s", strSql.c_str());
	CResultSet  *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			nCompanyID = pResultSet->GetInt("id");
		}
		delete pResultSet;
		pResultSet = NULL;
	}
	return nCompanyID;
}


int CDataSync::GetMailGroupUser(string& strInputString, list<string>& listUsers, string& strGroupID)
{
	Json::Reader Reader;
	Json::Value Root;
	int nResult = -1;
	try
	{
		if (Reader.parse(strInputString, Root))
		{
			if (!Root["groupId"].isNull() && Root["groupId"].isString())
			{
				strGroupID = Root["groupId"].asString();
			}
			else
			{	
				strGroupID = "";
			}

			if (!Root["userIds"].isNull() && Root["userIds"].isArray())
			{
				Json::Value GroupUsers = Root["userIds"];
				int nUsers = GroupUsers.size();;
				for (int i = 0; i < nUsers; i++)
				{
					if (!GroupUsers[i]["userId"].isNull() && GroupUsers[i]["userId"].isString())
					{
						string strUserID = GroupUsers[i]["userId"].asString();
						listUsers.push_back(strUserID);
					}
				}
				nResult = 1;
			}
		}
		else
		{
			log("Invalid json data");
		}
	}
	catch (std::runtime_error msg)
	{
		log("error message");
	}
	catch (exception& Exception)
	{
		log("Exception occured: %s", Exception.what());
	}

	return nResult;
}


void CDataSync::SetMailGroupUser(string& strGroupID, list<string>& listUsers)
{
	uint32_t nCurrentTime = time(NULL);
	int nGroupId = 0;
	int nUserCount = listUsers.size();
	if (nUserCount > 0)
	{
		string strSql = "truncate table IMGroupMemberTemp";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		strSql = "BEGIN";
		m_pDBConn->ExecuteUpdate(strSql.c_str());

		strSql = "select id from IMGroup where groupid='" + strGroupID + "' and status = 0";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				nGroupId = pResultSet->GetInt("id");
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		if (nGroupId > 0)
		{
			for (list<string>::iterator it = listUsers.begin(); it != listUsers.end(); ++it)
			{
				uint32_t unCurrentTime = time(NULL);
				strSql = "insert into IMGroupMemberTemp (groupid,userid,created,updated,groupuuid,useruuid) select a.id,b.id," + int2string(unCurrentTime) + "," + int2string(unCurrentTime);
				strSql += ",a.groupid,b.useruuid from IMGroup a, IMUserA b where a.groupid='" + strGroupID + "' and a.status=0 and b.useruuid='" + *it + "' and b.status=1";
				log("SQL for inserting data to temp table: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());
			}

			//add new group member
			strSql = "insert into IMGroupMember (groupid,userid,created,updated) select groupid,userid,created,updated from IMGroupMemberTemp where not exists(select 1 from IMGroupMember where IMGroupMember.groupid=IMGroupMemberTemp.groupid and IMGroupMember.userid=IMGroupMemberTemp.userid)";
			log("SQL for inserting new memers; %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			//delete group member
			strSql = "update IMGroupMember set status=1, updated=unix_timestamp() where groupid=" + int2string(nGroupId) + " and not exists(select 1 from IMGroupMemberTemp  where IMGroupMember.groupid=IMGroupMemberTemp.groupid and IMGroupMember.userid=IMGroupMemberTemp.userid)";
			log("SQL for deleting a group member: %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			//update group member(add  deleted users. change create time)
			strSql = "update IMGroupMember a ,IMGroupMemberTemp b  set a.status=0,a.created=unix_timestamp(),a.updated=unix_timestamp() where a.groupid=b.groupid and a.userid=b.userid and a.status = 1";
			log("SQL for update users for a group: %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			strSql = "update IMGroup, (select count(*) as totalcount from IMGroupMember where groupid=" + int2string(nGroupId) + " and status = 0) as b set usercnt=b.totalcount,version=version+1,updated=unix_timestamp() where groupid='" + strGroupID + "'";
			log("SQL for updating group; %s", strSql.c_str());
			m_pDBConn->ExecuteUpdate(strSql.c_str());

			//update creator
			strSql = "select a.status as status1 from IMGroupMember a, IMGroup b where a.groupid=b.id and a.userid= b.creator and b.id=" + int2string(nGroupId);
			log("SQL for getting creator status: %s", strSql.c_str());
			int nCreatorStatus = 0;
			pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
			if (pResultSet)
			{
				while (pResultSet->Next())
				{
					nCreatorStatus = pResultSet->GetInt("status1");
				}
			}
			
			if (nCreatorStatus == 1) //creator is deleted and should be replaced
			{
				strSql = "update IMGroup a ,(select max(userid) as maxuid from IMGroupMember where status = 0 and groupid=" + int2string(nGroupId) + ") as b set a.creator=b.maxuid,version=version+1, a.updated=unix_timestamp() where a.id=" + int2string(nGroupId);
				log("SQL for updating creator: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql.c_str());
			}

			SetGroupUserStatus(strGroupID);
		}

		strSql = "COMMIT";
		m_pDBConn->ExecuteUpdate(strSql.c_str());
	}
}

void CDataSync::CheckUser(string& strUserID)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "select id,status from IMUserA where useruuid='" + strUserID + "'";
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			uint32_t unUserId = pResultSet->GetInt("id");
			int nStatus = pResultSet->GetInt("status");
			SetUserStatus(unUserId, nStatus);
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	strSql = "select a.id as gid,b.id as uid ,b.status as bstatus, usercnt from IMGroup a, IMUserA b, IMGroupMember c where b.useruuid='" + strUserID + "' and a.id=c.groupid and b.id=c.userid";
	log("SQL for getting groupid and userid: %s", strSql.c_str());

	pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		uint32_t unUserID = 0, unGroupID = 0;
		int nStatus = 0;
		while (pResultSet->Next())
		{
			unUserID = pResultSet->GetInt("uid");
			nStatus = pResultSet->GetInt("bstatus");
			unGroupID = pResultSet->GetInt("gid");
			uint32_t unCurrentTime = time(NULL);
			int nUserCount = pResultSet->GetInt("usercnt");

			if (nStatus != 1) //user is deleted and disabled 
			{
				/*string strSql1 = "update IMGroup a, IMGroupMember b set userCnt = userCnt-1, a.updated=unix_timestamp(), version=version+1, b.status = 1,b.updated=a.updated where ";
				strSql1 += "b.userid=" + int2string(unUserID) + " and a.id=b.groupid and userCnt >= 1";
				log("SQL for updating group & group member: %s", strSql1.c_str());
				m_pDBConn->ExecuteUpdate(strSql1.c_str());*/

				string strSql1 = "update IMGroupMember set status = 1,updated=unix_timestamp() where groupid= " + int2string(unGroupID) + " and userid=" + int2string(unUserID);
				log("SQL for updating group member: %s", strSql1.c_str());
				m_pDBConn->ExecuteUpdate(strSql1.c_str());

				strSql1 = "update IMGroup a, (select max(userid) as maxuserid from IMGroupMember where status = 0 and groupid = " + int2string(unGroupID) + ") as b set a.creator=b.maxuserid,a.updated=unix_timestamp(), a.version=a.version+1 where a.id=" + int2string(unGroupID) + " and creator = " + int2string(unUserID);
				log("SQL for updating creator: %s", strSql.c_str());
				m_pDBConn->ExecuteUpdate(strSql1.c_str()); //update creator 

				//to decide if we should remove the group
				if (nUserCount > 1)
				{
					string strSql1 = "update IMGroup set userCnt = userCnt-1, updated=unix_timestamp(), version=version+1 where id=" + int2string(unGroupID);
					log("SQL for updating group: %s", strSql1.c_str());
					m_pDBConn->ExecuteUpdate(strSql1.c_str());
				}
				else
				{
					string strSql1 = "update IMGroup set userCnt = 0, status=1, updated=unix_timestamp(), version=version+1 where id=" + int2string(unGroupID);
					log("SQL for removing group: %s", strSql1.c_str());
					m_pDBConn->ExecuteUpdate(strSql1.c_str());
				}

				SetGroupUserStatus(unGroupID, unUserID, 1, unCurrentTime); //don't forget here
			}
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::SetGroupUserStatus(uint32_t unGroupID, uint32_t unUserID, uint32_t unStatus, uint32_t nCreateTime)
{
	if (m_pCacheGroupMember)
	{
		string strKey = "group_member_" + int2string(unGroupID);
		map<string, string> mapCachedGroupMember;

		m_pCacheGroupMember->hgetAll(strKey, mapCachedGroupMember);

		string strUserID = int2string(unUserID);
		string strCreateTime = int2string(nCreateTime);

		if (mapCachedGroupMember.find(strUserID) == mapCachedGroupMember.end())
		{
			if (unStatus == 0)  //a new member
			{
				m_pCacheGroupMember->hset(strKey, strUserID, strCreateTime);
			}
		}
		else
		{
			if (unStatus == 1) //deleted member
			{
				m_pCacheGroupMember->hdel(strKey, strUserID);
			}
		}
	}
}


void CDataSync::SetGroupUserStatus(string& strGroupID)
{
	if (m_pCacheGroupMember)
	{
		string strSql = "select a.groupid as gid,userid,a.created as acreated, a.status as status1,b.status as status2 from IMGroupMember a, IMGroup b where b.groupid= '" + strGroupID + "' and a.groupid=b.id";
		map<string, string> mapCachedGroupMember;


		MAP_MEMBER_INFO mapDBGroupMember; //key:userid,value:status
		uint32_t unGroupID = 0;
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());

		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				unGroupID = pResultSet->GetInt("gid");
				uint32_t unUserID = pResultSet->GetInt("userid");
				uint32_t unCreated = pResultSet->GetInt("acreated");
				uint32_t unStatus = pResultSet->GetInt("status1");
				MEMBER_INFO CurrentUser;
				memset(&CurrentUser, 0, sizeof(CurrentUser));
				CurrentUser.m_unStatus = unStatus;
				if (unStatus == 0)
				{
					CurrentUser.m_unCreateTime = unCreated;
					mapDBGroupMember[unUserID] = CurrentUser;
				}
			}
			delete pResultSet;
			pResultSet = NULL;
		}
			
		//get members for a group from cache
		string strKey =  "group_member_"  + int2string(unGroupID);
		m_pCacheGroupMember->hgetAll(strKey, mapCachedGroupMember);
		
		//both cache and db data are consistent
		//delete cache data first
		for (map<string, string>::iterator it = mapCachedGroupMember.begin(); it != mapCachedGroupMember.end(); it++)
		{
			if (mapDBGroupMember.find(string2int(it->first)) == mapDBGroupMember.end())
			{
				m_pCacheGroupMember->hdel(strKey, it->first);
			}
		}


		for (IT_MAP_MEMBER_INFO it1 = mapDBGroupMember.begin(); it1 != mapDBGroupMember.end(); it1++)
		{
			char szUserID[32], szCreateTime[32];
			memset(szUserID, 0, sizeof(szUserID));
			memset(szCreateTime, 0, sizeof(szCreateTime));

			snprintf(szUserID, sizeof(szUserID)-1, "%d", it1->first);
			snprintf(szCreateTime, sizeof(szCreateTime)-1, "%d", it1->second.m_unCreateTime);

			if (mapCachedGroupMember.find(szUserID) == mapCachedGroupMember.end())
			{

				if (it1->second.m_unStatus == 0)  //a new member
				{
					m_pCacheGroupMember->hset(strKey, szUserID, szCreateTime);
				}
			}
		}
		
		//clear members 
		mapDBGroupMember.clear();
		mapCachedGroupMember.clear();
	}
}


void CDataSync::SetAllUserStatus(void)
{
	if (m_pCacheUser)
	{
		string strSql = "select id,status from IMUserA";
		log("SQL for getting id: %s", strSql.c_str());
		CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				uint32_t unUserID = pResultSet->GetInt("id");
				int32_t nStatus = pResultSet->GetInt("status");
				string strKey = "user_" + int2string(unUserID);
				string strStatus = int2string(nStatus);
				//m_pCacheUser->set(strKey, int2string(unStatus));
				log("KEY: %s, VALUE: %s", strKey.c_str(), strStatus.c_str());
				m_pCacheUser->set(strKey, strStatus);
			}
			delete pResultSet;
			pResultSet = NULL;
		}
	}
}


int CDataSync::GetUserAllAlias(string& strInputString, LIST_USER_ALIAS& listAlias)
{
	Json::Reader Reader;
	Json::Value Root;

	int nResult = -1;
	try
	{
		if (Reader.parse(strInputString, Root))
		{
			if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
			{
				string strResult = Root["resultCode"].asString();
				if (strResult == "0000")
				{
					nResult = 1;
				}
				else
				{
					return nResult;
				}
			}
			else
			{
				return nResult;
			}

			if (!Root["data"].isNull() && Root["data"].isArray())
			{
				Json::Value Data = Root["data"];
				int nCount = Data.size();
				for (int i = 0; i < nCount; i++)
				{
					USER_ALIAS UserAlias;
					Json::Value Alias = Data[i];
					if (!Alias["aliasId"].isNull() && Alias["aliasId"].isString())
					{
						UserAlias.m_strUUID = Alias["aliasId"].asString();
					}
					else
					{
						return -1;
					}

					if (!Alias["userId"].isNull() && Alias["userId"].isString())
					{
						UserAlias.m_strUserID = Alias["userId"].asString();
					}
					else
					{
						return -1;
					}

					if (!Alias["username"].isNull() && Alias["username"].isString())
					{
						UserAlias.m_strUserName = Alias["username"].asString();
					}
					else
					{
						UserAlias.m_strUserName = "";
					}

					if (!Alias["aliasName"].isNull() && Alias["aliasName"].isString())
					{
						UserAlias.m_strAliasName = Alias["aliasName"].asString();
					}
					else
					{
						UserAlias.m_strAliasName = "";
					}
					listAlias.push_back(UserAlias);
				}
			}
		}
	}
	catch (exception& Exception)
	{
		log("an exception occurred; %s", Exception.what());
	}

	return nResult;
}


void CDataSync::SetUserAllAlias(LIST_USER_ALIAS& listAlias)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "truncate table UserAliasTemp";
	m_pDBConn->ExecuteQuery(strSql.c_str());

	uint32_t unCurrentTime = time(NULL);
	for (IT_LIST_USER_ALIAS it = listAlias.begin(); it != listAlias.end(); it++)
	{
		strSql = "insert into UserAliasTemp (uuid,useruuid,username,aliasname,created,updated) values(?,?,?,?,?,?)";
		CPrepareStatement* pStmt = new CPrepareStatement();
		if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
		{
			int nIndex = 0;

			pStmt->SetParam(nIndex++, it->m_strUUID);
			pStmt->SetParam(nIndex++, it->m_strUserID);
			pStmt->SetParam(nIndex++, it->m_strUserName);
			pStmt->SetParam(nIndex++, it->m_strAliasName);
			pStmt->SetParam(nIndex++, unCurrentTime);
			pStmt->SetParam(nIndex++, unCurrentTime);
			pStmt->ExecuteUpdate();
		}
		delete pStmt;
		pStmt = NULL;
	}

	//delete duplicated data
	strSql = "delete from UserAliasTemp a using (select uuid, max(id) as maxid from UserAliasTemp group by uuid having count(*) > 1) as b  where a.uuid=b.uuid and a.id <> b.id";
	m_pDBConn->ExecuteQuery(strSql.c_str());

	//insert new data
	strSql = "insert into UserAlias(uuid,useruuid,username,aliasname,created,updated) select uuid,useruuid,username,aliasname,created,updated from UserAliasTemp a where not exists(select 1 from UserAlias b where a.uuid=b.uuid)";
	m_pDBConn->ExecuteQuery(strSql.c_str());

	strSql = "update UserAlias a, UserAliasTemp b set a.useruuid=b.useruuid,a.username=b.username,a.aliasname=b.aliasname,a.updated=unix_timestamp() where a.uuid=b.uuid ";
	strSql += " and (a.useruuid <> b.useruuid or a.username <> b.username or a.aliasname <> b.aliasname)";
	m_pDBConn->ExecuteQuery(strSql.c_str());

	//delete alias
	strSql = "delete from UserAlias a using UserAliasTemp where not exists(select 1 from UserAliasTemp b where a.uuid=b.uuid)";
	m_pDBConn->ExecuteQuery(strSql.c_str());

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteUserAliasByAliasId(const string& strAliasId)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "delete from UserAlias where uuid='" + strAliasId + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "update IMUserA a, UserAlias b set a.updated=unix_timestamp() where a.useruuid=b.useruuid";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteUserAliasByUserId(const string& strUserId)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "delete from UserAlias where useruuid='" + strUserId + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "update IMUserA set updated=unix_timestamp() where useruuid= '" + strUserId + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::DeleteUserAliasByAliasName(const string& strAliasName)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "delete from UserAlias where aliasname='" + strAliasName + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "update IMUserA a, UserAlias b set a.updated=unix_timestamp() where a.useruuid=b.useruuid and b.aliasname='" + strAliasName + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::ChangeUserInfo(uint32_t unUserID, string& strUserID, string& strUserInfo)
{
	Json::Value Root;
	/*string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());*/

	string strSql = "select useruuid, avatar,unix_timestamp(birthday) as birth,sex,signature from IMUserA where id=" + int2string(unUserID);
	CResultSet *pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			if (pResultSet->GetString("useruuid") != NULL)
			{
				strUserID = pResultSet->GetString("useruuid");
			}
			else
			{
				strUserID = "";
			}

			Root["userId"] = strUserID;

			if (pResultSet->GetString("avatar") != NULL)
			{
				Root["headPortraitd"] = pResultSet->GetString("avatar");
			}
			else
			{
				Root["headPortraitd"] = "";
			}
			
			if (pResultSet->GetString("signature") != NULL)
			{
				Root["signature"] = pResultSet->GetString("signature");
			}
			else
			{
				Root["signature"] = "";
			}

			if (pResultSet->GetString("birth") != NULL)
			{
				Root["birthday"] = pResultSet->GetInt("birth") * 1000;
			}
			else
			{
				Root["birthday"] = 0;
			}
		}
		delete pResultSet;
		pResultSet = NULL;
	}

	//Root["userDetails"] = value;


	strSql = "select uuid,useruuid,type,title,content from IMUserA where useruuid='" + strUserID + "'";
	pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
	if (pResultSet)
	{
		while (pResultSet->Next())
		{
			
			;

		}
		delete pResultSet;
		pResultSet = NULL;
	}
	//strOutData.append(Root.toStyledString())
}


int CDataSync::GetUserAlias(const string& strInputString, USER_ALIAS& Alias)
{
	Json::Reader Reader;
	Json::Value Root;

	int nResult = -1;
	try
	{
		if (Reader.parse(strInputString, Root))
		{
			if (!Root["resultCode"].isNull() && Root["resultCode"].isString())
			{
				string strResult = Root["resultCode"].asString();
				if (strResult == "0000")
				{
					nResult = 1;
				}
				else
				{
					return nResult;
				}
			}
			else
			{
				return nResult;
			}

			if (!Root["data"].isNull())
			{
				Json::Value Data = Root["data"];
				if (!Data["aliasId"].isNull() && Data["aliasId"].isString())
				{
					Alias.m_strUUID = Data["aliasId"].asString();
				}
				else
				{
					return -1;
				}

				if (!Data["userId"].isNull() && Data["userId"].isString())
				{
					Alias.m_strUserID = Data["userId"].asString();
				}
				else
				{
					return -1;
				}

				if (!Data["username"].isNull() && Data["username"].isString())
				{
					Alias.m_strUserName = Data["username"].asString();
				}
				else
				{
					Alias.m_strUserName = "";
				}

				if (!Data["aliasName"].isNull() && Data["aliasName"].isString())
				{
					Alias.m_strAliasName = Data["aliasName"].asString();
				}
				else
				{
					Alias.m_strAliasName = "";
				}
			}
		}
	}
	catch (exception& Exception)
	{
		log("an exception occurred; %s", Exception.what());
	}

	return nResult;
}


void CDataSync::AddUserAlias(USER_ALIAS& Alias)
{
	string strSql = "BEGIN";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	//if the uuid exists? to be judge first(make up later)
	uint32_t unCurrTime = time(NULL);
	strSql = "insert into UserAlias (uuid,useruuid,username,aliasname,created,updated) values(?,?,?,?,?,?)";
	log("SQL for inserting useralias: %s", strSql.c_str());
	CPrepareStatement* pStmt = new CPrepareStatement();
	if (pStmt->Init(m_pDBConn->GetMysql(), strSql))
	{
		int nIndex = 0;

		pStmt->SetParam(nIndex++, Alias.m_strUUID);
		pStmt->SetParam(nIndex++, Alias.m_strUserID);
		pStmt->SetParam(nIndex++, Alias.m_strUserName);
		pStmt->SetParam(nIndex++, Alias.m_strAliasName);
		pStmt->SetParam(nIndex++, unCurrTime);
		pStmt->SetParam(nIndex++, unCurrTime);
		pStmt->ExecuteUpdate();
	}
	delete pStmt;
	pStmt = NULL;

	strSql = "update IMUserA set updated=" + int2string(unCurrTime) + "  where useruuid='" + Alias.m_strUserID + "'";
	m_pDBConn->ExecuteUpdate(strSql.c_str());

	strSql = "COMMIT";
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::UpdateUserAlias(USER_ALIAS& Alias)
{
	string strSql = "update IMUserA a, UserAlias b set a.updated=unix_timestamp(), b.updated=a.updated, b.aliasname='" + Alias.m_strAliasName + "' where a.useruuid= b.useruuid and b.uuid='" + Alias.m_strUUID + "'";
	log("SQL for updating useralias; %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


void CDataSync::SetAllGroupUserStatus(void)
{
	if (m_pCacheGroupMember)
	{
		//get group ids
		list<uint32_t> listGroup;
		string strSql = "select id from IMGroup where status = 0";
		CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				uint32_t unGroupId = pResultSet->GetInt("id");
				listGroup.push_back(unGroupId);
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		for (list<uint32_t>::iterator it = listGroup.begin(); it != listGroup.end(); it++)
		{
			map<string, string> mapCachedGroupMember;
			//get members for a group from cache
			string strKey = "group_member_" + int2string(*it);
			m_pCacheGroupMember->hgetAll(strKey, mapCachedGroupMember);
			if (mapCachedGroupMember.size() == 0)
			{
				strSql = "select userid,created from IMGroupMember where groupid=" + int2string(*it) + " and status = 0";
				CResultSet* pResultSet = m_pDBConn->ExecuteQuery(strSql.c_str());
				if (pResultSet)
				{
					while (pResultSet->Next())
					{
						uint32_t unUserID = pResultSet->GetInt("userid");
						uint32_t unCreated = pResultSet->GetInt("created");
						string strUserID = int2string(unUserID);
						string strCreated = int2string(unCreated);
						m_pCacheGroupMember->hset(strKey, strUserID, strCreated);
					}
					delete pResultSet;
					pResultSet = NULL;
				}
			}
			else
			{
				mapCachedGroupMember.clear();
			}
		}
	}
}


void CDataSync::GetDomainByDomainId(string& strInputString, DOMAIN_INFO& DomainInfo)
{
	try
	{
		Json::Reader Reader;
		Json::Value Root;

		if (Reader.parse(strInputString, Root))
		{
			string strResult = "";
			if (Root["resultCode"].isNull() && Root["resultCode"].isString())
			{
				strResult = Root["resultCode"].asString();
				if (strResult != "0000")
				{
					return;
				}
			}

			if (Root["data"].isNull())
			{
				return;
			}

			Json::Value msgData = Root["data"];

			if (!msgData["domainId"].isNull() && msgData["domainId"].isString())
			{
				DomainInfo.m_strDomainID = msgData["domainId"].asString();
			}
			else
			{
				DomainInfo.m_strDomainID = "";
			}

			if (!msgData["company"].isNull() && msgData["company"].isString())
			{
				DomainInfo.m_strCompanyName = msgData["company"].asString();
			}
			else
			{
				DomainInfo.m_strCompanyName = "";
			}
		}
	}
	catch (std::runtime_error msg)
	{
		log("error message");
	}
	catch (exception& Exception)
	{
		log("Exception occured: %s", Exception.what());
	}
}


void CDataSync::UpdateDomainByDomainId(DOMAIN_INFO& DomainInfo)
{
	string strSql = "update IMCompany set companyname='" + DomainInfo.m_strCompanyName + "',updated=unix_timestamp() where domainid='" + DomainInfo.m_strDomainID + "'";
	log("SQL for getting company name: %s", strSql.c_str());
	m_pDBConn->ExecuteUpdate(strSql.c_str());
}


//void CDataSync::DeleteDepartIDAndUserID(string& strOrgID, string& strUserID)
//{
//	string strSql = "update IMUserPositionA b set status = -1, updated = unix_timestamp() where uuid = b.posuuid and a.uuid = '" + strPositionID + "'";
//	log("SQL for deleting position: %s", strSql.c_str());
//	m_pDBConn->ExecuteUpdate(strSql.c_str());
//
//}
//
//
//void CDataSync::DeletePositionIdAndUserId(string& strPositionId, string& strUsrId)
//{
//	string strSql = "update Position a, IMUserPositionA b set a.status = -1, a.updated = unix_timestamp(), b.status = -1, b.updated = unix_timestamp() where a.uuid = b.posuuid and a.uuid = '" + strPositionID + "'";
//	log("SQL for deleting position: %s", strSql.c_str());
//	m_pDBConn->ExecuteUpdate(strSql.c_str());
//}
