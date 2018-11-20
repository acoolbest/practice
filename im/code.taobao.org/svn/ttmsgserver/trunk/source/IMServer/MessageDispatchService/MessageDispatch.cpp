#include "RabbitMQ.h"
#include "DataSync.h"
#include "HttpClient.h"
#include "RouteServConn.h"
#include "IM.Buddy.pb.h"


typedef struct accessurl
{
	string m_strUrl;
	string m_strPostData;
	string m_strResponse;
	string m_strOperationType;
	string m_strUuid;
}ACCESS_URL;

int Processing(CDBConn *pCurrConn, string& strInData, string& strOutData, int  nBusinessType, string& strUrlPath, string& strMethod, string& strUuid);
void* AccessRemoteHttpServer(void* pvArgument);

static CacheConn* pCacheGroupMember = NULL;
static CacheConn* pCacheDomainUser = NULL;
static CacheConn* pCacheUnread = NULL;

using namespace IM::BaseDefine;
using namespace SYNC;

int main(int argc, const char** argv)
{
	if (argc != 2)
	{
		printf("usage:messagedispatch 1/2(1:product data;2:data sync;3:mail group)\n");
		return -1;
	}

	signal(SIGPIPE, SIG_IGN);

	//get configurations
	CConfigFileReader ConfigFile("messagedispatch.conf");

	////get redis for group member(group member----join time)
	//char *pszGroupMemberPool = ConfigFile.GetConfigName("group_member_pool");
	//char *pszGroupMemberHost = ConfigFile.GetConfigName("group_member_host");
	//char *pszGroupMemberPort = ConfigFile.GetConfigName("group_member_port");
	//char *pszGroupMemberDBNo = ConfigFile.GetConfigName("group_member_db");
	//char *pszGroupMemberMaxConn = ConfigFile.GetConfigName("group_member_maxconncnt");

	//if (!pszGroupMemberPool || !pszGroupMemberHost || !pszGroupMemberPort || !pszGroupMemberDBNo || !pszGroupMemberMaxConn)
	//{
	//	log("invalid group member configurations");
	//	return -1;
	//}

	////get redis for user(user---status)
	//char *pszUserPool = ConfigFile.GetConfigName("user_pool");
	//char *pszUserHost = ConfigFile.GetConfigName("user_host");
	//char *pszUserPort = ConfigFile.GetConfigName("user_port");
	//char *pszUserDBNo = ConfigFile.GetConfigName("user_db");
	//char *pszUserMaxConn = ConfigFile.GetConfigName("user_maxconncnt");
	//if (!pszUserPool || !pszUserHost || !pszUserPort || !pszUserDBNo || !pszUserMaxConn)
	//{
	//	log("invalid domain user configurations");
	//	return -1;
	//}

	//CachePool GroupMember(pszGroupMemberPool, pszGroupMemberHost, atoi(pszGroupMemberPort), atoi(pszGroupMemberDBNo), atoi(pszGroupMemberMaxConn));
	//CachePool DomainUser(pszUserPool, pszUserHost, atoi(pszUserPort), atoi(pszUserDBNo), atoi(pszUserMaxConn));

	//int nResult = GroupMember.Init();
	//if (nResult)
	//{
	//	log("failed to initialize group member.....");
	//	return -1;
	//}

	//pCacheGroupMember = GroupMember.GetCacheConn();
	//if (pCacheGroupMember == NULL)
	//{
	//	log("failed to get a connection for group member.........");
	//	return -1;
	//}

	//nResult = DomainUser.Init();
	//if (nResult)
	//{
	//	log("failed to initialize domain user.......");
	//	return -1;
	//}

	//pCacheDomainUser = DomainUser.GetCacheConn();
	//if (pCacheDomainUser == NULL)
	//{
	//	log("failed to get a connection for domain user......");
	//	return -1;
	//}

	int nBusinessID = atoi(argv[1]); //get business id

	char* pszMQServerIP = NULL;
	char* pszMQServerPort = NULL;
	int nServerPort = 0;
	char* pszURL = NULL;
	char *pszVHost = NULL;
	char *pszMaxFrameSize = NULL;
	char *pszUserName = NULL;
	char *pszPassword = NULL;
	char *pszChannel = NULL;
	int nMaxFrameSize = NULL;
	int nChannel =0;
	char *pszExchange = NULL;
	char *pszExchangeType = NULL;
	char *pszQueue = NULL;
	char *pszRoutingKey = NULL;
	
	char szKeyServerIP[32];
	char szKeyServerPort[32];
	char szKeyHttpMsgServerUrl[32];
	char szKeyVHost[32];
	char szKeyMaxFrameSize[32];
	char szKeyUserName[32];
	char szKeyPassword[32];
	char szKeyChannel[32];
	char szKeyExchange[32];
	char szKeyExchangeType[32];
	char szKeyQueue[32];
	char szKeyRoutingKey[32];

	memset(szKeyServerIP, 0, sizeof(szKeyServerIP));
	memset(szKeyServerPort, 0, sizeof(szKeyServerPort));
	memset(szKeyHttpMsgServerUrl, 0, sizeof(szKeyHttpMsgServerUrl));
	memset(szKeyVHost, 0, sizeof(szKeyVHost));
	memset(szKeyMaxFrameSize, 0, sizeof(szKeyMaxFrameSize));
	memset(szKeyUserName, 0, sizeof(szKeyUserName));
	memset(szKeyPassword, 0, sizeof(szKeyPassword));
	memset(szKeyChannel, 0, sizeof(szKeyChannel));
	memset(szKeyExchange, 0, sizeof(szKeyExchange));
	memset(szKeyExchangeType, 0, sizeof(szKeyExchangeType));
	memset(szKeyQueue, 0, sizeof(szKeyQueue));
	memset(szKeyRoutingKey, 0, sizeof(szKeyRoutingKey));

	snprintf(szKeyServerIP, sizeof(szKeyServerIP)-1, "ServerIP%d", nBusinessID);
	snprintf(szKeyServerPort, sizeof(szKeyServerPort)-1, "ServerPort%d", nBusinessID);
	snprintf(szKeyHttpMsgServerUrl, sizeof(szKeyHttpMsgServerUrl)-1, "HttpMsgServerUrl%d", nBusinessID);
	snprintf(szKeyVHost, sizeof(szKeyVHost)-1, "VHost%d", nBusinessID);
	snprintf(szKeyMaxFrameSize, sizeof(szKeyMaxFrameSize)-1, "MaxFrameSize%d", nBusinessID);
	snprintf(szKeyUserName, sizeof(szKeyUserName)-1, "UserName%d", nBusinessID);
	snprintf(szKeyPassword, sizeof(szKeyPassword)-1, "Password%d", nBusinessID);
	snprintf(szKeyChannel, sizeof(szKeyChannel)-1, "Channel%d", nBusinessID);
	snprintf(szKeyExchange, sizeof(szKeyExchange)-1, "Exchange%d", nBusinessID);
	snprintf(szKeyExchangeType, sizeof(szKeyExchangeType)-1, "ExchangeType%d", nBusinessID);
	snprintf(szKeyQueue, sizeof(szKeyQueue)-1, "Queue%d", nBusinessID);
	snprintf(szKeyRoutingKey, sizeof(szKeyRoutingKey)-1, "RoutingKey%d", nBusinessID);

	pszMQServerIP = ConfigFile.GetConfigName(szKeyServerIP);
	pszMQServerPort = ConfigFile.GetConfigName(szKeyServerPort);
	nServerPort = atoi(pszMQServerPort);
	pszURL = ConfigFile.GetConfigName(szKeyHttpMsgServerUrl);
	pszVHost = ConfigFile.GetConfigName(szKeyVHost);
	pszMaxFrameSize = ConfigFile.GetConfigName(szKeyMaxFrameSize);
	pszUserName = ConfigFile.GetConfigName(szKeyUserName);
	pszPassword = ConfigFile.GetConfigName(szKeyPassword);
	pszChannel = ConfigFile.GetConfigName(szKeyChannel);
	nMaxFrameSize = atoi(pszMaxFrameSize);
	nChannel = atoi(pszChannel);
	pszExchange = ConfigFile.GetConfigName(szKeyExchange);
	pszExchangeType = ConfigFile.GetConfigName(szKeyExchangeType);
	pszQueue = ConfigFile.GetConfigName(szKeyQueue);
	pszRoutingKey = ConfigFile.GetConfigName(szKeyRoutingKey);
	
	if (pszMQServerIP && pszMQServerPort && pszURL && pszURL && pszVHost && pszMaxFrameSize && pszUserName && pszPassword
		&& pszChannel && pszExchange && pszExchangeType && pszQueue && pszRoutingKey)
	{
		//get route servers
		uint32_t route_server_count = 0;
		serv_info_t* route_server_list = read_server_config(&ConfigFile, "RouteServerIP", "RouteServerPort", route_server_count);

		if (route_server_count > 0)
		{
			log("Initializing route server....");
			SYNC::init_route_serv_conn(route_server_list, route_server_count);
			SYNC::set_route_serv_conn();
		}

		while (1) //if error occurs when receiving message,make it reconnect automatically 
		{	
			CRabbitMQ  Consumer;
			Consumer.Init(pszMQServerIP, nServerPort);
			Consumer.Login(pszVHost, nMaxFrameSize, pszUserName, pszPassword);
			if (Consumer.Preparing(nChannel, pszExchange, pszExchangeType, pszQueue, pszRoutingKey))
			{
				CDBManager* pDBManager = CDBManager::getInstance();
				if (!pDBManager)
				{
					log("DBManager init failed");
					return -1;
				}

				CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
				if (pDBConn == NULL)
				{
					log("failed to get db connection");
					return -1;
				}

				CacheManager *pCacheManager = CacheManager::getInstance();
				if (!pCacheManager)
				{
					log("CacheManager init failed");
					return -1;
				}

				pCacheGroupMember = pCacheManager->GetCacheConn("group_member");
				if (!pCacheGroupMember)
				{
					log("failed to get cache connection for group member");
					return -1;
				}

				pCacheDomainUser = pCacheManager->GetCacheConn("user");
				if (!pCacheDomainUser)
				{
					log("failed to get cache connection for user");
					return -1;
				}

				pCacheUnread = pCacheManager->GetCacheConn("unread");
				if (!pCacheUnread)
				{
					log("failed to get cache connection for unread");
					return -1;
				}

				CDataSync Synchronization;

				Synchronization.SetConnection(pDBConn, pCacheGroupMember, pCacheDomainUser, pCacheUnread);
				Synchronization.SetAllUserStatus();
				Synchronization.SetAllGroupUserStatus();
				Synchronization.SetAllCompanyStaff();
				Synchronization.SetAllUsers();

				log("Ready to receving data...............\n");
				while (1)
				{
					string strMsgBody;
					string strPostData;
					string strResponse;
					string strPath = "";
					string strOperation = "";
					string strId = "";

					if (Consumer.RecvMessage(strMsgBody) > 0)
					{
						log("\n\nMESSAGE: %s", strMsgBody.c_str());
						if (Processing(pDBConn, strMsgBody, strPostData, nBusinessID, strPath, strOperation, strId) > 0)
						{
							log("POST: %s\n", strPostData.c_str());

							pthread_t ThreadID = 0;

							ACCESS_URL *pRequestMsg = new ACCESS_URL;
							pRequestMsg->m_strUrl = pszURL + strPath;
							pRequestMsg->m_strPostData = strPostData;
							pRequestMsg->m_strOperationType = strOperation;
							pRequestMsg->m_strUuid = strId;

							if (strOperation != "other")
							{
								if (strPath != "")
								{
									AccessRemoteHttpServer(pRequestMsg);
									if (strOperation == "create.domain")
									{
										pRequestMsg = new ACCESS_URL;
										pRequestMsg->m_strOperationType = "query.domain";
										pRequestMsg->m_strUrl = pszURL;
										pRequestMsg->m_strUrl += "/s/ec/domainHelper/getDomainInfo?";
										pRequestMsg->m_strPostData = strPostData;
										AccessRemoteHttpServer(pRequestMsg);
									}
								}
							}
							else if (strOperation != "push.new.count.relatedToMe")
							{
								int nValue = 0;
								nValue = pthread_create(&ThreadID, NULL, AccessRemoteHttpServer, pRequestMsg);
								if (nValue != 0)
								{
									log("failed to create a thread: %d", nValue);
								}
							}
						}
					}
					else
					{
						log("faied to recv message: %d......", nBusinessID);
						Consumer.Close();
						break;
					}
				}

				pDBManager->RelDBConn(pDBConn);
				pDBManager = NULL;
				pDBConn = NULL;
				pCacheManager->RelCacheConn(pCacheDomainUser);
				pCacheManager->RelCacheConn(pCacheGroupMember);
				pCacheManager->RelCacheConn(pCacheUnread);
				pCacheManager = NULL;
				pCacheDomainUser = NULL;
				pCacheGroupMember = NULL;
				pCacheUnread = NULL;
			}
		}
	}
	else
	{
		log("fewer configurations.............");
	}
}


int Processing(CDBConn *pCurrConn, string& strInData, string& strOutData, int nBusinessType, string& strUrlPath, string& strMethod, string& strUuid)
{
	Json::Reader Reader;
	Json::Value Value;
	Json::Value Root;

	if (nBusinessType == 1)
	{
		strMethod = "other";
		string strUserID = "";
		string strProductID = "";
		string strCode = "";
		string strStaffId = "";

		try
		{
			if (!Reader.parse(strInData, Value))
			{
				log("json parse failed, post_data=%s ", strInData.c_str());
				return -1;
			}

			if (!Value["userId"].isNull() && Value["userId"].isString())
			{
				strUserID = Value["userId"].asString();
			}

			if (!Value["productId"].isNull() && Value["productId"].isString())
			{
				strProductID = Value["productId"].asString();
			}
			
			if (!Value["code"].isNull() && Value["code"].isString())
			{
				strCode = Value["code"].asString();
			}

			if (!Value["attributes"].isNull())
			{
				Json::Value Attribute = Value["attributes"];
				if (!Attribute["staffId"].isNull() && Attribute["staffId"].isString())
				{
					strStaffId = Attribute["staffId"].asString();
				}
			}		
		}
		catch (std::runtime_error msg)
		{
			log("parse json data failed.");
			return -1;
		}

		//get user id
		string strSql = "";
		int nUserID = 0;
		if (strCode == "push.new.mail" || strCode == "push.new.hr")
		{
			strSql = "select userid from IMStaff where uuid='" + strStaffId + "' and status=1"; 
		}
		else if (strCode != "" )
		{
			strSql = "select id from IMUser where useruuid='" + strUserID + "' and status=1";
		}
		
	/*	if (strCode != "push.new.user" && strCode != "push.new.userVerify")
		{
			strSql += " and locate('" + strProductID + "', buyproduct) > 0";
		}*/

		CResultSet* pResultSet = pCurrConn->ExecuteQuery(strSql.c_str());		
		if(pResultSet)
		{
			while (pResultSet->Next())
			{
				if (strCode == "push.new.mail" || strCode == "push.new.hr")
				{
					nUserID = pResultSet->GetInt("userid");
				}
				else
				{
					nUserID = pResultSet->GetInt("id");
				}
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		int nProductID = 0;
		if (strCode == "push.new.workflow")
		{
			nProductID = HUAYUN_IM_PRODUCT_WORKFLOW;
		}
		else if (strCode == "push.new.mail")
		{
			nProductID = HUAYUN_IM_PRODUCT_EMAIL;
		}
		else if (strCode == "push.new.bulletin")
		{
			nProductID = HUAYUN_IM_PRODUCT_BULLETIN;
		}
		else if (strCode == "push.new.user")
		{
			nProductID = HUAYUN_IM_ASST_NEW_FRIEND;
		}
		else if (strCode == "push.new.userVerify")
		{
			nProductID = HUAYUN_IM_ASST_NEW_STAFF;
		}
		else if (strCode == "push.new.hr")
		{
			nProductID = HUAYUN_IM_ASST_ATTENDANCE;
		}

	/*	if (strProductID == "WORKFLOW")
		{
			nProductID = HUAYUN_IM_PRODUCT_WORKFLOW;
		}
		else if (strProductID == "MAIL")
		{
			nProductID = HUAYUN_IM_PRODUCT_EMAIL;
		}
		else if (strProductID == "BULLETIN")
		{
			nProductID = HUAYUN_IM_PRODUCT_BULLETIN;
		}
		else if (strCode == "push.new.user")
		{
			nProductID = HUAYUN_IM_ASST_NEW_FRIEND;
		}*/


		//log("PRODUCT ID: %d, SQL: %s\t UID: %d\n", nInProductID, strSql.c_str(), nUserID);
		log("PRODUCT: %s---->PRODUCT ID: %d, SQL: %s\t UID: %d\n", strProductID.c_str(), nProductID, strSql.c_str(), nUserID);

		if (nProductID && nUserID > 10000)
		{
			Root["to_user_id"] = nUserID;
			Root["detailed_msg"] = strInData;
			Root["product_id"] = nProductID;

			strOutData.append(Root.toStyledString());
			return 1;
		}
		else
		{
			log("%s is currently not supported or invalid user id: %s", strProductID.c_str(), strUserID.c_str());
			return 0;
		}
	}
	else if (nBusinessType == 3)
	{
		strMethod = "";
		string strUserID = "";
		unsigned int unUserId = 0;
		unsigned int unCount = 0;
		string strAvatar = "";

		try
		{
			if (!Reader.parse(strInData, Value))
			{
				log("json parse failed, post_data=%s ", strInData.c_str());
				return -1;
			}

			if (!Value["userId"].isNull() && Value["userId"].isString())
			{
				strUserID = Value["userId"].asString();
			}

			if (!Value["code"].isNull() && Value["code"].isString())
			{
				strMethod = Value["code"].asString();
			}

			if (!Value["count"].isNull() && Value["count"].isUInt())
			{
				unCount = Value["count"].asUInt();
			}

			if (!Value["headPortrait"].isNull() && Value["headPortrait"].isString())
			{
				strAvatar = Value["headPortrait"].asString();
			}	
		}
		catch (std::runtime_error msg)
		{
			log("parse json data failed.");
			return -1;
		}

		//get user id
		string strSql = "select id from IMUser where useruuid='" + strUserID + "' and status=1";

		CResultSet* pResultSet = pCurrConn->ExecuteQuery(strSql.c_str());
		if (pResultSet)
		{
			while (pResultSet->Next())
			{
				unUserId = pResultSet->GetInt("id");
			}
			delete pResultSet;
			pResultSet = NULL;
		}

		if (unUserId != 0)
		{
			IM::Buddy::IMUserMomentNotify msgNotify;
			msgNotify.set_user_id(unUserId);
			msgNotify.set_like_count(unCount);
			msgNotify.set_avatar(strAvatar);

			CRouteServConn* pRouteConn = SYNC::get_route_serv_conn();
			if (pRouteConn)
			{
				pRouteConn->Lock();

				CImPdu pdu;
				pdu.SetPBMsg(&msgNotify);
				pdu.SetServiceId(SID_BUDDY_LIST);
				pdu.SetCommandId(CID_BUDDY_LIST_USER_MOMENT_NOTIFY);

				int nBytes = pRouteConn->SendPdu(&pdu);
				log("Sent %d bytes to route server", nBytes);
				
				pRouteConn->Unlock();
			}
			else
			{
				log("route server is unavailable!........");
			}
		}
	}
	//else if (nBusinessType == 2 || nBusinessType == 3)
	else if (nBusinessType == 2)
	{
		try
		{
			if (!Reader.parse(strInData, Value))
			{
				log("json parse failed, post_data=%s ", strInData.c_str());
				return -1;
			}

			if (Value["code"].isNull() || !Value["code"].isString())
			{
				log("valid json data type for code");
				return -1;
			}
		}
		catch (std::runtime_error msg)
		{
			log("parse json data failed.");
			return -1;
		}

		string strCodeType = Value["code"].asString();
		strMethod = strCodeType;

		string strDataID = "", strOrgID = "", strDomainID = "", strUserID = "", strPositionID = "", strStaffId = "";
		string strJobGrade = "", strGroupId = "";

		if (!Value["dataId"].isNull() && Value["dataId"].isString())
		{
			strDataID = Value["dataId"].asString();
			strStaffId = Value["dataId"].asString();
		}

		if (!Value["primaryOrgId"].isNull() && Value["primaryOrgId"].isString())
		{
			strDomainID = Value["primaryOrgId"].asString();
		}

		if (!Value["groupId"].isNull() && Value["groupId"].isString())
		{
			strGroupId = Value["groupId"].asString();
		}

		if (!Value["orgId"].isNull() && Value["orgId"].isString())
		{
			strOrgID = Value["orgId"].asString();
		}

		if (!Value["userId"].isNull() && Value["userId"].isString())
		{
			strUserID = Value["userId"].asString();
		}

		if (!Value["positionId"].isNull() && Value["positionId"].isString())
		{
			strPositionID = Value["positionId"].asString();
		}

		if (!Value["staffId"].isNull() && Value["staffId"].isString())
		{
			strStaffId = Value["staffId"].asString();
		}

		if (!Value["relation"].isNull() && Value["relation"].isString())
		{
			strJobGrade = Value["relation"].asString();
		}

		bool bContinue = true;

		CDataSync Synchronization;
		Synchronization.SetConnection(pCurrConn, pCacheGroupMember, pCacheDomainUser, pCacheUnread);
		//Synchronization.SetAllCompanyStaff();

		if (strCodeType == "create.user" || strCodeType == "update.user")
		{
			strUrlPath = "rs/s/ec/userHelper/getUser?";
			strOutData = "userId=" + strDataID;
		}
		else if (strCodeType == "delete.user")
		{
			Synchronization.DeleteUser(strDataID);
		}
		else if (strCodeType == "create.organization" || strCodeType == "update.organization")
		{
			strUrlPath = "rs/s/ec/organizationHelper/getOrgByOrgId?";
			strOutData = "orgId=" + strDataID;
		}
		else if (strCodeType == "delete.organization")
		{
			Synchronization.DeleteDepartmentByDepartmentID(strDataID);
		}
		else if (strCodeType == "create.position" || strCodeType == "update.position")
		{
			strUrlPath = "rs/s/ec/positionHelper/getPositionByPositionId?";
			strOutData = "positionId=" + strDataID;
		}
		else if (strCodeType == "create.domain" || strCodeType == "update.domain") //create a domain
		{
			strUrlPath = "rs/s/ec/domainHelper/getDomainByDomainId?";
			strOutData = "domainId=" + strDataID;
		}
		else if (strCodeType == "create.userDetails" || strCodeType == "update.userDetails" || strCodeType == "delete.userDetails") //additional info
		{
			strUrlPath = "rs/s/ec/userDetailsHelper/queryUserDetailsListByUserId?";
			strOutData = "userId=" + strDataID;
			strUuid = strDataID;
		}
		else if (strCodeType == "create.userProduct" || strCodeType == "update.userProduct" || strCodeType == "delete.userProduct") //buy products
		{
			strUrlPath = "rs/s/ec/userProductHelper/queryUserProductsByUserId?";
			strOutData = "userId=" + strUserID;
		}
		else if (strCodeType == "update.DomainPassword")
		{
			strUrlPath = "rs/s/ec/domainHelper/getDomainPasswordByDomainId?";
			strOutData = "domainId=" + strDataID;
		}
		else if (strCodeType == "delete.position")
		{
			Synchronization.DeletePositionByPositionID(strDataID);
		}
		else if (strCodeType == "delete.position.domainId")
		{
			//Synchronization.DeletePositionByDomainID(string& strDomainID);
		}
		else if (strCodeType == "delete.orgPosition.orgId")
		{
			//delete a department
			Synchronization.DeleteDepartmentByDepartmentID(strDataID);
		}
		else if (strCodeType == "delete.orgPosition.positionId")
		{
			Synchronization.DeletePositionByPositionID(strDataID);
			//delete position id
		}
		else if (strCodeType == "delete.orgPosition")
		{
			//delete department and position
			Synchronization.DeleteDepartmentAndPosition(strOrgID, strPositionID);
		}
		else if (strCodeType == "create.orgPosition")
		{
			//ignore
		}
		else if (strCodeType == "delete.orgStaff.orgId")
		{
			//delete a department
			Synchronization.DeleteDepartmentByDepartmentID(strDataID);
		}
		else if (strCodeType == "delete.orgUser.userId")
		{
			//ignore
		}
		else if (strCodeType == "delete.orgStaff")
		{
			//delete from userpositin(usrid,departid)
			Synchronization.DeleteDepartIDAndUserID(strOrgID, strStaffId);
		}
		else if (strCodeType == "create.orgStaff")
		{
			//insert into usrposition(userid,departid)
			Synchronization.AddStaffPosition(strOrgID, strPositionID, strStaffId, strJobGrade);
		}
		else if (strCodeType == "update.orgStaff")
		{
			Synchronization.UpdateStaffPosition(strStaffId, strOrgID, strJobGrade);
		}
		else if (strCodeType == "update.orgUser")
		{
			//update from userposition(userid,departid)
		}
		else if (strCodeType == "delete.staffPosition.orgIdAndStaffId")
		{
			Synchronization.DeleteStaffPosition(strOrgID, strStaffId);
			//Synchronization.DeleteDepartIDAndUserID(strOrgID, strUserID);
			//delete from userposition ,departid, userid
		}
		else if (strCodeType == "delete.staffPosition.staffId")
		{
			//ignore
		}
		else if (strCodeType == "delete.staffPosition")
		{
			//delete userposition
			Synchronization.DeleteStaffPosition(strOrgID, strPositionID, strStaffId);
		}
		else if (strCodeType == "create.staffPosition")
		{
			Synchronization.AddStaffPosition(strOrgID, strPositionID, strStaffId, strJobGrade);
			//insert userposition from db(departid,posiitonid,userid)
		}
		else if (strCodeType == "update.staffPosition")
		{

		}
		else if (strCodeType == "delete.staffPosition.orgIdAndPositionId")
		{
			//ignore 
		}
		else if (strCodeType == "delete.staffPosition.positionId")
		{
			//delete position from db(key: positionid)
		}
		else if (strCodeType == "create.userAlias" || strCodeType == "update.userAlias" || strCodeType == "delete.userAlias")
		{
			strUrlPath = "rs/s/ec/userAliasHelper/queryUserAliasListByUserId?";
			strOutData = "userId=" + strUserID;
			strUuid = strUserID;
		}
	/*	else if (strCodeType == "delete.UserAlias")
		{
			Synchronization.DeleteUserAliasByAliasId(strDataID);
		}*/
		else if (strCodeType == "deleteByUserId.UserAlias")
		{
			Synchronization.DeleteUserAliasByUserId(strDataID);
		}
		else if (strCodeType == "deleteByAliasName.UserAlias")
		{
			Synchronization.DeleteUserAliasByAliasName(strDataID);
		}
		else if (strCodeType == "create.MailGroup" || strCodeType == "update.MailGroup") 
		{
			strUrlPath = "rs/s/ec/mailHelperProxy/queryGroupByGroupIdAndPriOrgId?";
			strOutData = "groupId=" + strGroupId;
			strOutData += "&primaryOrgId=" + strDomainID;

			//log("PATH: %s", strUrlPath.c_str());
		}
		else if (strCodeType == "deleteByUserId.UserAlias")
		{
			Synchronization.DeleteMailGroup(strDataID);
		}
		else if (strCodeType == "delete.MailGroup")
		{
			Synchronization.DeleteMailGroup(strDataID);
		}
		else if (strCodeType == "create.MailGroupUser" || strCodeType == "update.MailGroupUser" || strCodeType == "delete.MailGroupUser")
		{
			strUrlPath = "rs/s/ec/mailHelperProxy/queryUserIdsByGroupIdAndPriOrgId?";
			//log("PATH: %s", strUrlPath.c_str());
			strOutData = "groupId=" + strGroupId;
			strOutData += "&primaryOrgId=" + strDomainID; 
		}
		else if (strCodeType == "create.orgInfo" || strCodeType == "update.orgInfo")
		{
			strUrlPath = "rs/s/ec/orgInfoHelper/getOrgInfo?";
			strOutData = "primaryOrgId=" + strDataID;
		}
		else if (strCodeType == "delete.orgInfo")
		{
			Synchronization.DeleteCompany(strDataID);
		}
		else if (strCodeType == "create.orgPassword" || strCodeType == "update.orgPassword")
		{
			strUrlPath = "rs/s/ec/orgInfoHelper/getOrgPassword?";
			strOutData = "primaryOrgId=" + strDataID;
		}
		else if (strCodeType == "delete.orgPassword")
		{
			Synchronization.DeleteCompanyPasswordRule(strDataID);
		}
		else if (strCodeType == "create.orgSet" || strCodeType == "update.orgSet")
		{
			strUrlPath = "rs/s/ec/orgSetHelper/getOrgSet?";
			strOutData = "primaryOrgId=" + strDataID;
		}
		else if (strCodeType == "delete.orgSet")
		{
			Synchronization.DeleteCompanyInfoSet(strDataID);
		}
		else if (strCodeType == "delete.position.primaryOrgId")
		{
			Synchronization.DeletePositionByCompanyUuid(strDataID);
		}
		else if (strCodeType == "create.staff" || strCodeType == "update.staff")
		{
			strUrlPath = "rs/s/ec/staffHelper/getStaff?";
			strOutData = "staffId=" + strDataID;
		}
		else if (strCodeType == "delete.staff")
		{
			Synchronization.DeleteCompanyStaff(strStaffId);
		}
		else if (strCodeType == "create.staffDetails" || strCodeType == "delete.staffDetails")
		{
			strUrlPath = "rs/s/ec/staffDetailsHelper/queryStaffDetailsByStaffId?";
			strOutData = "staffId=" + strDataID;
			strUuid = strDataID;
		}
		else if (strCodeType == "delete.orgStaff.staffId")
		{
			Synchronization.DeleteStaffPositionByStaffId(strDataID);
		}
		else if (strCodeType == "delete.orgStaff.orgId")
		{
			Synchronization.DeleteStaffPositionByDepartmentId(strDataID);
		}
		else if (strCodeType == "create.staffProduct" || strCodeType == "update.staffProduct" || strCodeType == "delete.staffProduct") //buy products
		{
			strUrlPath = "rs/s/ec/staffProductHelper/queryStaffProductsByStaffId?";
			strOutData = "staffId=" + strStaffId;
		}
		else if (strCodeType == "delete.staffProduct.orgProductId")
		{

		}
		else if (strCodeType == "delete.position.primaryOrgId")
		{

		}
		else if (strCodeType == "create.userLimitIp" || strCodeType == "update.userLimitIp" || strCodeType == "delete.userLimitIp")
		{
			strUrlPath = "rs/s/ec/userLimitIpHelper/queryUserLimitIps?";
			strOutData = "userId=" + strDataID;
			strUuid = strDataID;
		}	
		else
		{
			log("Operation Type: %s is currently not supported", strCodeType.c_str());
		}
		
		//DbPool.RelDBConn(pConn);
		return 1;
	}
}


void* AccessRemoteHttpServer(void* pvArgument)
{
	pthread_detach(pthread_self());

	if (pvArgument)
	{
		CHttpClient HttpClient;
		ACCESS_URL *pAccessUrl = (ACCESS_URL*)pvArgument;
		if (pAccessUrl->m_strOperationType == "other")  //directely access the url
		{
			HttpClient.Post(pAccessUrl->m_strUrl, pAccessUrl->m_strPostData, pAccessUrl->m_strResponse);
			log("URL: %s, PostData: %s, Response: %s", pAccessUrl->m_strUrl.c_str(), pAccessUrl->m_strPostData.c_str(), pAccessUrl->m_strResponse.c_str());
		}
		else
		{

			//log("Ready.............");
			//usleep(250000);//george added on June 3rd,2016.sleep 100 milliseconds
			//log("1 second expired.....Beginning.................");
		/*	struct timespec TimeOut;	
			memset(&TimeOut, 0, sizeof(TimeOut));
			TimeOut.tv_nsec = 250000000;
			nanosleep(&TimeOut, NULL);*/

			string strResponse;		
			string strAddiHeader = "";
			string strToken = "";

			HttpClient.GetAdditionalInfo(strToken, strAddiHeader);

			string strActualPath = pAccessUrl->m_strUrl + pAccessUrl->m_strPostData + "&rs_token=" + strToken;

			HttpClient.Post(strActualPath, "", strResponse, strAddiHeader);
			log("URL: %s, Header: %s, Response: %s", strActualPath.c_str(), strAddiHeader.c_str(), strResponse.c_str());

			if (strResponse == "")
			{
				return NULL;
			}

			CDBManager* pDBManager = CDBManager::getInstance();
			if (!pDBManager)
			{
				log("DBManager init failed");
				return NULL;
			}

			CDBConn* pDBConn = pDBManager->GetDBConn("teamtalk_master");
			if (pDBConn == NULL)
			{
				log("failed to get db connection");
				return NULL;
			}


			CDataSync Synchronization;
			Synchronization.SetConnection(pDBConn, pCacheGroupMember, pCacheDomainUser, pCacheUnread);
			//Synchronization.SetAllCompanyStaff();

			//handle each kind of operations
			if (pAccessUrl->m_strOperationType == "create.user")
			{
				USER_INFO UserInformation;
				Synchronization.GetUserInfo(strResponse, UserInformation);
				Synchronization.AddUser(UserInformation);
			}
			else if (pAccessUrl->m_strOperationType == "update.user")
			{
				USER_INFO UserInformation;
				Synchronization.GetUserInfo(strResponse, UserInformation);
				Synchronization.UpdateUser(UserInformation);
			}
			else if (pAccessUrl->m_strOperationType == "create.organization")
			{
				DEPARTMENT_INFO DepartmentInformation;
				Synchronization.GetDepartInfo(strResponse, DepartmentInformation);
				Synchronization.AddCompanyDepart(DepartmentInformation);
			}
			else if (pAccessUrl->m_strOperationType == "update.organization")
			{
				DEPARTMENT_INFO DepartmentInformation;
				Synchronization.GetDepartInfo(strResponse, DepartmentInformation);
				Synchronization.UpdateCompanyDepart(DepartmentInformation);
			}
			else if (pAccessUrl->m_strOperationType == "create.position")
			{
				POSITION_INFO PositionInformation;
				Synchronization.GetPositionInfo(strResponse, PositionInformation);
				Synchronization.AddCompanyPosition(PositionInformation);
			}
			else if (pAccessUrl->m_strOperationType == "update.position")
			{
				POSITION_INFO PositionInformation;
				Synchronization.GetPositionInfo(strResponse, PositionInformation);
				Synchronization.UpdateCompanyPosition(PositionInformation);
			}
			else if (pAccessUrl->m_strOperationType == "create.userProduct" || pAccessUrl->m_strOperationType == "update.userProduct" || pAccessUrl->m_strOperationType == "delete.userProduct")
			{
				Synchronization.GetUserProducts(strResponse);
			}
			else if (pAccessUrl->m_strOperationType == "create.userDetails" || pAccessUrl->m_strOperationType == "update.userDetails" || pAccessUrl->m_strOperationType == "delete.userDetails")
			{
				LIST_USER_DETAILS listUserDetails;
				Synchronization.GetUserDetails(strResponse, listUserDetails);
				Synchronization.SetUserDetails(pAccessUrl->m_strUuid, listUserDetails);
				//Synchronization.AddUserDetails(listUserDetails);
			}
			else if (pAccessUrl->m_strOperationType == "create.orgInfo")
			{
				DOMAIN_INFO DomainInfo;
				Synchronization.GetCompanyInfo(strResponse, DomainInfo);
				Synchronization.AddNewCompany(DomainInfo);
			}
			else if (pAccessUrl->m_strOperationType == "update.orgInfo")
			{
				DOMAIN_INFO DomainInfo;
				Synchronization.GetCompanyInfo(strResponse, DomainInfo);
				Synchronization.UpdateCompany(DomainInfo);
			}
			else if (pAccessUrl->m_strOperationType == "create.orgPassword" || pAccessUrl->m_strOperationType == "update.orgPassword")
			{
				DOMAIN_PASSWORD DomainPassword;
				Synchronization.GetCompanyPasswordRule(strResponse, DomainPassword);
				Synchronization.UpdateCompanyPassword(DomainPassword);
			}
			else if (pAccessUrl->m_strOperationType == "create.orgSet" || pAccessUrl->m_strOperationType == "update.orgSet")
			{
				DOMAIN_INFO DomainInfo;
				Synchronization.GetCompanyInfoSet(strResponse, DomainInfo);
				Synchronization.UpdateCompanyInfoSet(DomainInfo);
			}
		/*	else if (pAccessUrl->m_strOperationType == "query.domain")
			{
				DOMAIN_INFO DomainInfo;
				Synchronization.GetCompanyByCompanyId(strResponse, DomainInfo);
				Synchronization.UpdateCompanyByDomainId(DomainInfo);
			}*/
			else if (pAccessUrl->m_strOperationType == "create.userAlias" || pAccessUrl->m_strOperationType == "update.userAlias" || pAccessUrl->m_strOperationType == "delete.userAlias")
			{
				LIST_USER_ALIAS lsUserAlias;
				Synchronization.GetUserAllAlias(strResponse, lsUserAlias);
				Synchronization.SetUserAllAlias(pAccessUrl->m_strUuid, lsUserAlias);
			}
			else if (pAccessUrl->m_strOperationType == "create.MailGroup")
			{
				DOMAIN_GROUP DomainGroup;
				list<string> listGroupUsers;
				if (Synchronization.GetGroupInfo(strResponse, DomainGroup, listGroupUsers) > 0)
				{
					Synchronization.AddNewMailGroupAndMembers(DomainGroup, listGroupUsers);
				}
			}
			else if (pAccessUrl->m_strOperationType == "update.MailGroup")
			{
				DOMAIN_GROUP DomainGroup;
				list<string> listGroupUsers;
				if (Synchronization.GetGroupInfo(strResponse, DomainGroup, listGroupUsers) > 0)
				{
					Synchronization.UpdateMailGroup(DomainGroup);
					if (DomainGroup.m_nStatus == 0)
					{
						Synchronization.SetMailGroupUser(DomainGroup.m_strGroupID, listGroupUsers);
					}
				}
			}
			else if (pAccessUrl->m_strOperationType == "create.MailGroupUser" || pAccessUrl->m_strOperationType == "update.MailGroupUser" || pAccessUrl->m_strOperationType == "delete.MailGroupUser")
			{
				list<string> listGroupUsers;
				string strGroupID;
				if (Synchronization.GetMailGroupUser(strResponse, listGroupUsers,strGroupID) > 0)
				{
					Synchronization.SetMailGroupUser(strGroupID,listGroupUsers);
				}
			}
		/*	else if (pAccessUrl->m_strOperationType == "delete.UserDetails")
			{
				;
			}*/
			else if (pAccessUrl->m_strOperationType == "delete.position")
			{

				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.position.domainId")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.orgPosition.orgId")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.orgPosition.positionId")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.orgPosition")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "create.orgPosition")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.orgUser.orgId")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.orgUser.userId")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.orgUser")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "create.orgUser")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "update.orgUser")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.userPosition.userIdAndOrgId")
			{
				;
			}	
			else if (pAccessUrl->m_strOperationType == "delete.userPosition.userId")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.userPosition")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "create.userPosition")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.userPosition.orgIdAndPositionId")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "delete.userPosition.positionId")
			{
				;
			}
			else if (pAccessUrl->m_strOperationType == "create.orgInfo" || pAccessUrl->m_strOperationType == "update.orgInfo")
			{
				
			}
		/*	else if (pAccessUrl->m_strOperationType == "delete.orgInfo")
			{
				Synchronization.DeleteCompany(strDataID);
			}*/
		/*	else if (strCodeType == "create.orgPassword" || strCodeType == "update.orgPassword")
			{
				strUrlPath = "rs/s/ec/orgInfoHelper/getOrgPassword?";
				strOutData = "primaryOrgId=" + strDataID;
			}
			else if (strCodeTyp == "delete.orgPassword")
			{
				Synchronization.DeleteCompanyPasswordRule(strDataID);
			}
			else if (strCodeType == "create.orgSet" || strCodeType == "update.orgSet")
			{
				strUrlPath = "rs/s/ec/orgSetHelper/getOrgSet?";
				strOutData = "primaryOrgId=" + strDataID;
			}
			else if (strCodeType == "delete.orgSet")
			{
				Synchronization.DeleteCompanyInfoSet(strDataID);
			}
			else if (strCodeType == "delete.position.primaryOrgId")
			{

			}*/
			else if (pAccessUrl->m_strOperationType == "create.staff")
			{
				STAFF_INFO StaffInformation;
				Synchronization.GetStaffInfo(strResponse, StaffInformation);
				Synchronization.AddCompanyStaff(StaffInformation);
			}
			else if (pAccessUrl->m_strOperationType == "update.staff")
			{
				STAFF_INFO StaffInformation;
				Synchronization.GetStaffInfo(strResponse, StaffInformation);
				Synchronization.UpdateCompanyStaff(StaffInformation);
			}
			/*else if (pAccessUrl->m_strOperationType == "delete.staff")
			{
				Synchronization.DeleteCompanyStaff(strStaffId);
			}*/
			else if (pAccessUrl->m_strOperationType == "create.staffDetails" || pAccessUrl->m_strOperationType == "delete.staffDetails")
			{
				LIST_USER_DETAILS listUserDetails;
				Synchronization.GetStaffDetails(strResponse, listUserDetails);
				Synchronization.SetStaffDetails(pAccessUrl->m_strUuid, listUserDetails);
			}
			else if (pAccessUrl->m_strOperationType == "create.staffProduct" || pAccessUrl->m_strOperationType == "update.staffProduct" || pAccessUrl->m_strOperationType == "delete.staffProduct") //buy products
			{
				 Synchronization.GetStaffProducts(strResponse);
			}
			else if (pAccessUrl->m_strOperationType == "delete.staffProduct.orgProductId")
			{

			}
			else if (pAccessUrl->m_strOperationType == "create.userLimitIp" || pAccessUrl->m_strOperationType == "update.userLimitIp" || pAccessUrl->m_strOperationType == "delete.userLimitIp")
			{
				LIST_LIMIT_IP listLimitedIp;
				Synchronization.GetLimitedIpInfo(strResponse, listLimitedIp);
				Synchronization.SetUserLimitedIp(pAccessUrl->m_strUuid,listLimitedIp);
			}
			else
			{
				log("Operation Type: %s is currently not supported: %s", pAccessUrl->m_strOperationType.c_str());
			}

			pDBManager->RelDBConn(pDBConn);
		}

		delete pAccessUrl;	
		pAccessUrl = NULL;
	}
	return NULL;
}