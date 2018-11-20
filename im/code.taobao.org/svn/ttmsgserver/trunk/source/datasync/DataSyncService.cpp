// DataSyncService.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include "jsoncpp\json\json.h"
#include "CachePool.h"
#include "DBPool.h"
#include <string.h>

#define CACHE_POOL_NAME	"im_sync_json"
#define LOCAL_CACHE_SERVER_IP	"127.0.0.1"
#define LOCAL_CACHE_SERVER_PORT	6379
#define LOCAL_CACHE_DB_NO	8
#define LOCAL_CACHE_DB_MAX_COUNT	16
#define FILE_NAME_DOMAIN_PREFIX	"server_domain"
#define FILE_NAME_USER_PREFIX	"domain_user"

typedef list<string> LIST_STRING;
typedef LIST_STRING::iterator  IT_LIST_STRING;

int GetDomainUser(Json::Value &root, string& strParent);

int main(int argc, _TCHAR* argv[])
{
	CachPool NewPool(CACHE_POOL_NAME, LOCAL_CACHE_SERVER_IP, LOCAL_CACHE_SERVER_PORT, LOCAL_CACHE_DB_NO, LOCAL_CACHE_DB_MAX_COUNT);
	const char *pszCachePoolName = NewPool.GetPoolName();
	CacheManager *pCacheManager = CacheManager::getInstance();
	CacheConn* pCacheConn = pCacheManager->GetCacheConn(pszCachePoolName);
	if (pCacheConn)
	{
		int nSize = pCacheConn->llen(CACHE_POOL_NAME);
		LIST_STRING listFile;
		pCacheConn->lrange(CACHE_POOL_NAME, 0, nSize, listFile);
		pCacheManager->RelCacheConn(pCacheConn);
	}
	
	for (auto IT_LIST_STRING it = listFile.begin(); it != listFile.end(); ++it)
	{
		printf("File Name: %s", it->c_str());
		ParseJsonFromFile(it->c_str());
	}
	return 0;
}


int ParseJsonFromFile(const char *pszFileName)
{
	Json::Reader reader;
	// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...   
	Json::Value root;

	printf("FILE NAME: %s", pszFileName);

	std::ifstream is;
	is.open(pszFileName, std::ios::binary);
	if (reader.parse(is, root))
	{
		if (strstr(pszFileName, FILE_NAME_DOMAIN_PREFIX))
		{
			int nElements = root.size();
			for (int i = 0; i < nElements; ++i)
			{
				std::string strDomainID = root[i]["domainId"].asString();
				std::string strDomainName = root[i]["domainName"].asString();
				int nStatus = root[i]["status"].asInt();
				int nExpireDate = 0;
				string strProductID;
				if (!root[i]["expiredDate"].isNull())
				{
					nExpireDate = root[i]["expiredDate"].asUInt();
				}

				printf("Index: %d\tDomainID: %s\tDomain Name: %s\tStatus:%d\tExpireDate:%d\n", i, strDomainID.c_str(), strDomainName.c_str(), nStatus, nExpireDate);

				Json::Value DomainProducts = root[i]["domainProducts"];
				int nCount = DomainProducts.size();
				for (int j = 0; j < nCount; ++j)
				{
					if (DomainProducts[j]["domainProductId"].isNull())
					{
						is.close();
						return -1;
					}
					else
					{
						strProductID = DomainProducts[j]["domainProductId"];
						printf("ProductID: %s", strProductID.c_str());
					}
				}
			}
		}
		else if (strstr(pszFileName, FILE_NAME_USER_PREFIX))
		{
			Json::Value Attributes = root["attributes"];
			int nTypeID = Attributes["type"].asInt();
			string strID = root["id"].asString();
			string strCompany = root["text"].asString();

			printf("TypeID: %d\t Company Name: %s", nTypeID, strCompany.c_str());
			GetDomainUser(root, "");
		}
		is.close();
		return 0;

	}






//int ParseJsonFromFile(const char *pszFileName)
//{
//	Json::Reader reader;
//	// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...   
//	Json::Value root;
//
//	printf("FILE NAME: %s", pszFileName);
//
//	std::ifstream is;
//	is.open(pszFileName, std::ios::binary);
//	if (reader.parse(is, root))
//	{
//		if (strstr(pszFileName, FILE_NAME_DOMAIN_PREFIX))
//		{
//			int nElements = root.size();
//			for (int i = 0; i < nElements; ++i)
//			{
//				std::string strDomainID = root[i]["domainId"].asString();
//				std::string strDomainName = root[i]["domainName"].asString();
//				int nStatus = root[i]["status"].asInt();
//				int nExpireDate = 0;
//				string strProductID;
//				if (!root[i]["expiredDate"].isNull())
//				{
//					nExpireDate = root[i]["expiredDate"].asUInt();
//				}
//
//				printf("Index: %d\tDomainID: %s\tDomain Name: %s\tStatus:%d\tExpireDate:%d\n", i, strDomainID.c_str(), strDomainName.c_str(), nStatus, nExpireDate);
//
//				Json::Value DomainProducts = root[i]["domainProducts"];
//				int nCount = DomainProducts.size();
//				for (int j = 0; j < nCount; ++j)
//				{
//					if (DomainProducts[j]["domainProductId"].isNull())
//					{
//						is.close();
//						return -1;
//					}
//					else
//					{
//						strProductID = DomainProducts[j]["domainProductId"];
//						printf("ProductID: %s", strProductID.c_str());
//					}
//				}
//			}
//		}
//		else if (strstr(pszFileName, FILE_NAME_USER_PREFIX))
//		{
//			Json::Value Attributes = root["attributes"];
//			int nTypeID = Attributes["type"].asInt();
//			string strID = root["id"].asString();
//			string strCompany = root["text"].asString();
//
//			printf("TypeID: %d\t Company Name: %s", nTypeID, strCompany.c_str());
//
//			Json::Value Children = root["children"];
//			int nCount = Children.size();
//			for (int i = 0; i < nCount; ++i)
//			{
//				Json::Value Attribute1 = Children[i]["attributes"];
//				//string strID = Children[i]["id"];
//				int nType = Attribute1["type"].asInt();
//				if (nType == 1)
//				{
//					string strID = Children[i]["id"];
//					Json::Value ProductID = Children[i]["productIds"];
//					for (int j = 0; j < ProductID.size(); ++j)
//					{
//						if (ProductID[j]["productId"].compare("IM") == 0)
//						{
//							string strPassword = Children[i]["password"];
//							string strUserName = Children[i]["username"];
//							string strDomainID = Children[i]["domainId"];
//							string strAvatarUrl = Children[i]["headPortrait"];
//							string strFullName = Children[i]["fullname"];
//							int nSex = Children[i]["sex"];
//							string strPhone = Children[i]["phone"];
//							int nStatus = Children[i]["status"];
//							string strPosition;
//							string strKeyword = Children[i]["keyword"];
//							Json::Value Position = Children[i]["positionNames"];
//							for (int k = 0; k < Position.size() - 1; ++k)
//							{
//								strPosition = Position[k] + ",";
//							}
//							strPosition += Position[Position.size() - 1];
//							Json::Value Birthday = Children[i]["birthday"];
//							unsigned int unBirthday = Birthday[time].:asUInt();
//							break;
//						}
//					}
//				}
//				else if (nType == 0)
//				{
//					string strDepartment = Children[i]["text"].asString();
//					string strID = Children[i]["id"];
//				}
//			}
//
//
//		//		bool bContinue = true;
//		//		while (bContinue)
//		//		{
//		//			string strID = Children[i]["id"].asString();
//		//			
//		//			Json::Value Attribute = Children[i]["attributes"];
//		//			int nTypeID = Attribute["type"].asInt();
//		//		}
//		//	}
//
//		//}
//
//
//		////子节点数组
//		//"children":[
//		//{
//		//	//信息：id部门唯一标识，text部门名称
//		//	"id": "892ec5cacd0344e8803d4527b7953f01",
//		//		"text" : "部门名称",
//		//		"attributes" : {
//		//		"type": 0
//		//	},
//		//	//部门下的用户和子部门
//		//	"children" : [
//		//		{
//		//		}
//		//	]
//		//},
//	
//	
//		////std::string code;
//		////if (!root["files"].isNull())  // 访问节点，Access an object value by name, create a null member if it does not exist.   
//		////	code = root["uploadid"].asString();
//
//		////// 访问节点，Return the member named key if it exist, defaultValue otherwise.   
//		////code = root.get("uploadid", "null").asString();
//
//		////// 得到"files"的数组个数   
//		////int file_size = root["files"].size();
//
//		////// 遍历数组   
//		////for (int i = 0; i < file_size; ++i)
//		////{
//		////	Json::Value val_image = root["files"][i]["images"];
//		////	int image_size = val_image.size();
//		////	for (int j = 0; j < image_size; ++j)
//		////	{
//		////		std::string type = val_image[j]["type"].asString();
//		////		std::string url = val_image[j]["url"].asString();
//		////	}
//		////}
//	}
//	is.close();
//	return 0;
//
//}


int GetDomainUser(Json::Value &root, string& strParent)
{
	if (root["children"].isNull())
	{
		return -1;
	}

	if (strParent != "")
	{
		printf("Parent: %s\n", strParent.c_str());
	}
	else
	{
		printf("ROOT.............\n");
	}

	Json::Value Children = root["children"];
	int nCount = Children.size(); //get chiildrens
	for (int i = 0; i < nCount; ++i) //iteratoring
	{
		Json::Value Attribute1 = Children[i]["attributes"];
		//string strID = Children[i]["id"];
		int nType = Attribute1["type"].asInt();
		if (nType == 1)
		{
			string strID = Children[i]["id"];
			string strText = Children[i]["text"];
			Json::Value ProductID = Attribute1["productIds"];

			printf("ID: %s\t Text: %s\n", strID.c_str(),strText.c_str());
			for (int j = 0; j < ProductID.size(); ++j)
			{
				if (ProductID[j]["productId"].compare("IM") == 0)
				{
					string strPassword = Attribute1["password"];
					string strUserName = Attribute1["username"];
					string strDomainID = Attribute1["domainId"];
					string strAvatarUrl = Attribute1["headPortrait"];
					string strFullName = Attribute1["fullname"];
					int nSex = Attribute1["sex"];
					string strPhone = Attribute1["phone"];
					int nStatus =Attribute1["status"];
					string strPosition;
					string strKeyword = Attribute["keyword"];
					Json::Value Position = Attribute1["positionNames"];
					for (int k = 0; k < Position.size() - 1; ++k)
					{
						strPosition = Position[k] + ",";
					}
					strPosition += Position[Position.size() - 1];
					Json::Value Birthday = Attribute1["birthday"];
					unsigned int unBirthday = Birthday[time].asUInt();
					printf("Password: %s\tUserName:%s\tDomaindID; %s\tAvatarURL: %s=\tFullName: %s\n",strPassword.c_str(), strUserName.c_str(), strDomainID.c_str(), strAvatarUrl.c_str(),strFullName.c_str());
					printf("Sex: %d\tPhone: %s\tStatus: %d\tKeyword: %s\tPosition: %sBirthday: %d\n", nSex, strPhone.c_str(), nStatus, strKeyword.c_str(), strPosition.c_str(), unBirthday);
					break;
				}
			}
		}
		else if (nType == 0)
		{
			string strID = Children[i]["id"];
			string strDepartment = Children[i]["text"].asString();
			printf("DepartmentID: %s\tDepartment Name: %s\n", strID.c_str(), strDepartment.c_str());
			GetDomainUser(Children, strID);
		}
	}
	return 0;
}