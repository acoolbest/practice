#include "DataSync.h"

int main(int argc, const char* argv[])
{
	if (argc != 2)
	{
		printf("invalid parameters.......\n");
		printf("Usage: command interval(seconds)\n");
		printf("e.g: datasync 5\n");
		return -1;
	}

	signal(SIGPIPE, SIG_IGN);

	//get configurations
	CConfigFileReader ConfigFile("datasync.conf");
	//get db configurations
	char* pszDBPoolName = ConfigFile.GetConfigName("teamtalk_master_pool");
	char* pszDBHost = ConfigFile.GetConfigName("teamtalk_master_host");
	char* pszPort = ConfigFile.GetConfigName("teamtalk_master_port");
	char* pszDBName = ConfigFile.GetConfigName("teamtalk_master_dbname");
	char* pszUserName = ConfigFile.GetConfigName("teamtalk_master_username");
	char *pszPassword = ConfigFile.GetConfigName("teamtalk_master_password");
	char *pszDBMaxConn = ConfigFile.GetConfigName("teamtalk_master_maxconncnt");

	if (!pszDBPoolName || !pszDBHost || !pszPort || !pszDBName || !pszUserName || !pszPassword || !pszDBMaxConn)
	{
		log("invalid database configurations");
		return -1;
	}
	//get redis for data sync source
	char *pszSyncPool = ConfigFile.GetConfigName("im_sync_json_pool");
	char *pszSyncHost = ConfigFile.GetConfigName("im_sync_json_host");
	char *pszSyncPort = ConfigFile.GetConfigName("im_sync_json_port");
	char *pszSyncDBNo = ConfigFile.GetConfigName("im_sync_json_db");
	char *pszSyncMaxConn = ConfigFile.GetConfigName("im_sync_json_maxconncnt");

	if (!pszSyncPool || !pszSyncHost || !pszPort || !pszSyncDBNo || !pszSyncMaxConn)
	{
		log("invalid sync data source configurations");
		return -1;
	}

	//get redis for group member(group member----join time)
	char *pszGroupMemberPool = ConfigFile.GetConfigName("group_member_pool");
	char *pszGroupMemberHost = ConfigFile.GetConfigName("group_member_host");
	char *pszGroupMemberPort = ConfigFile.GetConfigName("group_member_port");
	char *pszGroupMemberDBNo = ConfigFile.GetConfigName("group_member_db");
	char *pszGroupMemberMaxConn = ConfigFile.GetConfigName("group_member_maxconncnt");

	if (!pszGroupMemberPool || !pszGroupMemberHost || !pszGroupMemberPort || !pszGroupMemberDBNo || !pszGroupMemberMaxConn)
	{
		log("invalid group member configurations");
		return -1;
	}

	//get redis for user(user---status)
	char *pszUserPool = ConfigFile.GetConfigName("user_pool");
	char *pszUserHost = ConfigFile.GetConfigName("user_host");
	char *pszUserPort = ConfigFile.GetConfigName("user_port");
	char *pszUserDBNo = ConfigFile.GetConfigName("user_db");
	char *pszUserMaxConn = ConfigFile.GetConfigName("user_maxconncnt");
	if (!pszUserPool || !pszUserHost || !pszUserPort || !pszUserDBNo || !pszUserMaxConn)
	{
		log("invalid domain user configurations");
		return -1;
	}

	CachePool DataSource(pszSyncPool, pszSyncHost, atoi(pszSyncPort), atoi(pszSyncDBNo), atoi(pszSyncMaxConn));
	CachePool GroupMember(pszGroupMemberPool, pszGroupMemberHost, atoi(pszGroupMemberPort), atoi(pszGroupMemberDBNo), atoi(pszGroupMemberMaxConn));
	CachePool DomainUser(pszUserPool, pszUserHost, atoi(pszUserPort), atoi(pszUserDBNo), atoi(pszUserMaxConn));

	int nResult = DataSource.Init();
	if (nResult)
	{
		log("failed to initialize data source..........\n");
		return -1;
	}

	CacheConn *pCacheConn = DataSource.GetCacheConn();
	if (pCacheConn == NULL)
	{
		log("failed to get a connection for data source.........\n");
		return -1;
	}

	nResult = GroupMember.Init();
	if (nResult)
	{
		log("failed to initialize group member.....");
		return -1;
	}

	CacheConn* pCacheGroupMember = GroupMember.GetCacheConn();
	if (pCacheGroupMember == NULL)
	{
		log("failed to get a connection for group member.........");
		return -1;
	}

	nResult = DomainUser.Init();
	if (nResult)
	{
		log("failed to initialize domain user.......");
		return -1;
	}

	CacheConn* pCacheDomainUser = DomainUser.GetCacheConn();
	if (pCacheDomainUser == NULL)
	{
		log("failed to get a connection for domain user......");
		return -1;
	}

	//CDBPool DbPool(DB_POOL_NAME, DB_SERVR_IP, DB_SERVER_PORT, USER_NAME, USER_PASSWORD, DB_NAME, MAX_CONN_COUNT);
	CDBPool DbPool(pszDBPoolName, pszDBHost, atoi(pszPort), pszUserName, pszPassword, pszDBName, atoi(pszDBMaxConn));
	if (DbPool.Init())
	{
		log("failed to initizlize a database pool...............");
		return -1;
	}

	CDBConn* pConn = DbPool.GetDBConn();
	if (!pConn)
	{
		return 0;
	}

	bool bContinue = true;
	CDataSync Synchronization;
	Synchronization.SetConnection(pConn, pCacheGroupMember, pCacheDomainUser);
	while (bContinue)
	{
		time_t unCurrentTime = time(NULL);
		char szTime[32];
		memset(szTime, 0, sizeof(szTime));
		strftime(szTime, sizeof(szTime)-1, "%Y/%m/%d %H:%M:%S", localtime(&unCurrentTime));
		log("====================================================CURRENT TIME: %s====================================================\n", szTime);

		LIST_STRING listFile;
		LIST_USER_INFO  listUserInfo;
		LIST_COMPANY_INFO listCompanyInfo;
		LIST_DEPARTMENT_INFO listDepartmentInfo;
		LIST_DOMAIN_INFO listDomainInfo;
		LIST_DOMAIN_GROUP listDomainGroup;
		LIST_GROUP_USER listGroupUser;

		log("GETTING AND PARSING DATA.......................\n\n");

		while (pCacheConn->llen(pszSyncPool) > 0)
		{
			string strFileName = pCacheConn->rpop(pszSyncPool);
			Synchronization.ParseJsonFromFile(strFileName.c_str(), listDomainInfo, listCompanyInfo, listDepartmentInfo, listUserInfo, listDomainGroup, listGroupUser);
			remove(strFileName.c_str());
		}

		log("\n\nIMPORTING DATA..........................\n\n");
		log("Importing company.......\n");
		Synchronization.ImportCompanyInfo(listCompanyInfo, listDomainInfo);
		log("Importing department.....\n");
		Synchronization.ImportDepartmentInfo(listDepartmentInfo);
		log("Importing user...........\n");
		Synchronization.ImportUserInfo(listUserInfo);
		log("Importing position.......\n");
		Synchronization.ImportUserPosition(listUserInfo);
		log("Importing group...........\n");
		Synchronization.ImportDomainGroup(listDomainGroup);
		log("Importing group members.......\n");
		Synchronization.ImportGroupUser(listGroupUser);
		log("Final checking...........\n");
		Synchronization.CheckSyncData();
		log("\n\nEND OF IMPORTING DATA...................\n");

		sleep(atoi(argv[1]));
		//break;
	}

	DataSource.RelCacheConn(pCacheConn);
	GroupMember.RelCacheConn(pCacheGroupMember);
	DomainUser.RelCacheConn(pCacheDomainUser);
	DbPool.RelDBConn(pConn);
	return 0;
}
