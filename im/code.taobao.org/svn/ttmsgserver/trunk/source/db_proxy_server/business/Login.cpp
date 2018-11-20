/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：Login.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include <list>
#include "../ProxyConn.h"
#include "../HttpClient.h"
#include "../SyncCenter.h"
#include "Login.h"
#include "TokenValidator.h"
#include "json/json.h"
#include "Common.h"
#include "IM.Server.pb.h"
#include "Base64.h"
#include "InterLogin.h"
#include "ExterLogin.h"
#include "UserModel.h"
#include "security.h"

CInterLoginStrategy g_loginStrategy;

hash_map<string, list<uint32_t> > g_hmLimits;
CLock g_cLimitLock;
namespace DB_PROXY
{
	void doLogin(CImPdu* pPdu, uint32_t conn_uuid)
	{

		CImPdu* pPduResp = new CImPdu;

		IM::Server::IMValidateReq msg;
		IM::Server::IMValidateRsp msgResp;
		string strErrorString;

		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{

			string strAccount = msg.user_name();
			string strPass1 = msg.password();
			string strIp = msg.client_ip();
			uint32_t unIsActivated = msg.is_activated();

			msgResp.set_user_name(strAccount);
			msgResp.set_attach_data(msg.attach_data());

			do
			{
				CAutoLock cAutoLock(&g_cLimitLock);
				list<uint32_t>& lsErrorTime = g_hmLimits[strAccount];
				uint32_t tmNow = time(NULL);

				//清理超过30分钟的错误时间点记录
				/*
				 清理放在这里还是放在密码错误后添加的时候呢？
				 放在这里，每次都要遍历，会有一点点性能的损失。
				 放在后面，可能会造成30分钟之前有10次错的，但是本次是对的就没办法再访问了。
				 */
				auto itTime = lsErrorTime.begin();
				for (; itTime != lsErrorTime.end(); ++itTime)
				{
					if (tmNow - *itTime > IM_MAX_ALLOWED_LOCK_TIME * 60)
					{
						break;
					}
				}
				if (itTime != lsErrorTime.end())
				{
					lsErrorTime.erase(itTime, lsErrorTime.end());
				}

				// 判断30分钟内密码错误次数是否大于10
				//if(lsErrorTime.size() > 10)
				if (lsErrorTime.size() > IM_MAX_ALLOWED_LOGIN_TIME)
				{
					itTime = lsErrorTime.begin();
					if (tmNow - *itTime <= 30 * 60)
					{
						msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_LOCK_USER);
						strErrorString = "您的帐号已被锁定，请10分钟后再登录";
						log("已被锁定，请10分钟后登录或联系管理员(%d)", IM::BaseDefine::REFUSE_REASON_LOCK_USER);
						msgResp.set_result_string(strErrorString);
						pPduResp->SetPBMsg(&msgResp);
						pPduResp->SetSeqNum(pPdu->GetSeqNum());
						pPduResp->SetServiceId(IM::BaseDefine::SID_OTHER);
						pPduResp->SetCommandId(IM::BaseDefine::CID_OTHER_VALIDATE_RSP);
						CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
						return;
					}
				}
			} while (false);

			//log("%s request login\t password: %s", strAccount.c_str(), strPass1.c_str());


			uint32_t unUserId = 0;
			int nStatus = 0;

			char *pszPassword = NULL;
			uint32_t unPassLength = 0;
			DecryptMsg(strPass1.c_str(), strPass1.length(), &pszPassword, unPassLength);
			string strPass = "";
			if (pszPassword)
			{
				strPass = pszPassword;
				Free(pszPassword);
				pszPassword = NULL;
			}

			if (g_loginStrategy.doLogin(strAccount, strPass, unUserId, nStatus, unIsActivated,strIp))
			{
				IM::BaseDefine::UserInfo cUser;

				//get basic user information
				g_loginStrategy.GetBasicUserInfo(unUserId, cUser);

				IM::BaseDefine::UserInfo* pUser = msgResp.mutable_user_info();
				string strUserUuid = cUser.user_uuid();

				pUser->set_user_id(cUser.user_id());
				pUser->set_user_uuid(strUserUuid);
				pUser->set_user_name(cUser.user_name());
				pUser->set_user_real_name(cUser.user_real_name());
				pUser->set_avatar_url(cUser.avatar_url());
				pUser->set_user_gender(cUser.user_gender());
				pUser->set_birthday(cUser.birthday());
				pUser->set_mobile_phone(cUser.mobile_phone());
				pUser->set_status(cUser.status());
				pUser->set_keyword(cUser.keyword());
				pUser->set_user_nick_name(cUser.user_nick_name());
				pUser->set_signature(cUser.signature());
				pUser->set_email(cUser.email());
				pUser->set_account(cUser.account());
				//log("Account1; %s, Account2; %s", pUser->account().c_str(), cUser.account().c_str());

				//get black lists
				list<IM::BaseDefine::ShieldUser> lsBlackList;
				g_loginStrategy.GetShieldUsers(unUserId, lsBlackList);
				for (auto it = lsBlackList.begin(); it != lsBlackList.end(); it++)
				{
					IM::BaseDefine::ShieldUser *pShieldUser = pUser->add_black_list();
					pShieldUser->set_user_id(it->user_id());
					pShieldUser->set_shield_status(it->shield_status());
				}
				log("Black List Size: %d", lsBlackList.size());

				//get company list
				list<IM::BaseDefine::CompanyInfo> lsCompanyList;
				g_loginStrategy.GetCompanyInfo(unUserId, lsCompanyList);
				for (auto it = lsCompanyList.begin(); it != lsCompanyList.end(); it++)
				{
					IM::BaseDefine::CompanyInfo *pCompanyInfo = pUser->add_company_info_list();
					string strCompanyUuid = it->company_uuid();
					string strCompanyNo = it->company_no();
					string strCompanyName = it->company_name();
					string strCompanyFullName = it->company_full_name();
					string strIndustryType = it->industry_type();
					pCompanyInfo->set_company_id(it->company_id());
					pCompanyInfo->set_company_uuid(strCompanyUuid);
					pCompanyInfo->set_company_no(strCompanyNo);
					pCompanyInfo->set_company_name(strCompanyName);
					pCompanyInfo->set_company_full_name(strCompanyFullName);
					pCompanyInfo->set_industry_type(strIndustryType);
					pCompanyInfo->set_company_is_joined(it->company_is_joined());
					pCompanyInfo->set_company_is_invite(it->company_is_invite());
				}
				log("Joined company count: %d", lsCompanyList.size());


				//get vip list
				list<uint32_t> lsImportantPerson;
				g_loginStrategy.GetVips(unUserId, lsImportantPerson);
				for (auto it = lsImportantPerson.begin(); it != lsImportantPerson.end(); it++)
				{
					pUser->add_vip_id_list(*it);
				}
				log("VIP count: %d", lsImportantPerson.size());


				//get user details
				list<IM::BaseDefine::UserDetails> lsUserDetails;
				g_loginStrategy.GetUserDetails(strUserUuid, lsUserDetails);
				for (auto it = lsUserDetails.begin(); it != lsUserDetails.end(); it++)
				{
					IM::BaseDefine::UserDetails *pAddInfo = pUser->add_add_info_list();
					pAddInfo->set_details_uuid(it->details_uuid());
					pAddInfo->set_details_type(it->details_type());
					pAddInfo->set_details_title(it->details_title());
					pAddInfo->set_details_content(it->details_content());
				}
				log("User Additional Informations: %d", pUser->add_info_list_size());

				//get staff information for each company
				for (auto it = lsCompanyList.begin(); it != lsCompanyList.end(); it++)
				{
					uint32_t unCompanyId = it->company_id();

					//get basic staff information
					IM::BaseDefine::StaffInfo StaffInfo;
					g_loginStrategy.GetStaffInfo(unUserId, unCompanyId, StaffInfo);

					IM::BaseDefine::StaffInfo* pStaffInfo = pUser->add_staff_info_list();
					string strStaffUuid = StaffInfo.staff_uuid();
					pStaffInfo->set_staff_uuid(strStaffUuid);
					pStaffInfo->set_job_number(StaffInfo.job_number());
					pStaffInfo->set_staff_name(StaffInfo.staff_name());
					pStaffInfo->set_mobile_phone(StaffInfo.mobile_phone());
					pStaffInfo->set_company_id(unCompanyId);
					pStaffInfo->set_keyword(StaffInfo.keyword());
					pStaffInfo->set_user_id(unUserId);
					pStaffInfo->set_user_uuid(strUserUuid);
					pStaffInfo->set_flags(StaffInfo.flags());
					pStaffInfo->set_buy_product(StaffInfo.buy_product());
					pStaffInfo->set_account(StaffInfo.account());
					pStaffInfo->set_user_name(StaffInfo.user_name());
					pStaffInfo->set_email(StaffInfo.email());
					pStaffInfo->set_default_email(StaffInfo.default_email());

					//get staff details;
					
					list<IM::BaseDefine::UserDetails> lsStaffDetails;
					g_loginStrategy.GetStaffDetails(strStaffUuid, lsStaffDetails);
					for (auto it = lsStaffDetails.begin(); it != lsStaffDetails.end(); it++)
					{
						IM::BaseDefine::UserDetails *pStaffDetails = pStaffInfo->add_add_info_list();
						pStaffDetails->set_details_uuid(it->details_uuid());
						pStaffDetails->set_details_type(it->details_type());
						pStaffDetails->set_details_title(it->details_title());
						pStaffDetails->set_details_content(it->details_content());
					}
					log("Staff additional information: %d", pStaffInfo->add_info_list_size());

					//get staff duty
					list<IM::BaseDefine::DutyInfo> lsDutyInfo;
					map<string, list<string>> mapPosition;
					g_loginStrategy.GetStaffDuty(strStaffUuid, unCompanyId, lsDutyInfo, mapPosition);
					for (auto it = lsDutyInfo.begin(); it != lsDutyInfo.end(); it++) //get departments for a user
					{
						IM::BaseDefine::DutyInfo *pDutyInfo = pStaffInfo->add_duty_list();
						pDutyInfo->set_depart_uuid(it->depart_uuid());
						pDutyInfo->set_job_grade(it->job_grade());

						auto it1 = mapPosition.find(it->depart_uuid());
						if (it1 != mapPosition.end())
						{
							for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) //get position for a department
							{
								pDutyInfo->add_position_uuid_list(*it2);
							}
							log("Position List Size: %d for Depart: %s", pDutyInfo->position_uuid_list_size(), it->depart_uuid().c_str());
						}
					}
					log("Duty List Size: %d", pStaffInfo->duty_list_size());
				}

				msgResp.set_result_code(0);
				msgResp.set_result_string("成功");

				//如果登陆成功，则清除错误尝试限制
				CAutoLock cAutoLock(&g_cLimitLock);
				list<uint32_t>& lsErrorTime = g_hmLimits[strAccount];
				lsErrorTime.clear();
			}
			else
			{
				//密码错误，记录一次登陆失败
				uint32_t tmCurrent = time(NULL);
				bool bFound = false;
				CAutoLock cAutoLock(&g_cLimitLock);

				if (nStatus != LOGIN_RESULT_LOGIN_BY_ALIAS_NAME) //login by alias name(maybe incorrect password when login)
				{
					list<uint32_t>& lsErrorTime = g_hmLimits[strAccount];
					lsErrorTime.push_front(tmCurrent);
				}

				log("get result false");
				if (nStatus == LOGIN_RESULT_DISABLED_USER)
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_DISABLED_USER);
					strErrorString = "您的帐号已被禁用，请联系管理员处理";
					log("disabled user(%d)", IM::BaseDefine::REFUSE_REASON_DISABLED_USER);
					//msgResp.set_result_string(strErrorString);
				}
				//else if (nStatus == IM::BaseDefine::REFUSE_REASON_INVALID_DOMAIN_USER)
				//{
				//	msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_INVALID_DOMAIN_USER);
				//	strErrorString = "非法域用户";
				//	log("非法域用户(%d)", IM::BaseDefine::REFUSE_REASON_INVALID_DOMAIN_USER);
				//	//msgResp.set_result_string(strErrorString);
				//}
				else if (nStatus == LOGIN_RESULT_USER_NO_FOUND) //invalid user name
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_USER_NOT_EXIST);
					log("user name does not exists:(%d)", IM::BaseDefine::REFUSE_REASON_USER_NOT_EXIST);
					//strErrorString = "用户不存在";
					strErrorString = "帐号或密码错误";
					//msgResp.set_result_string(strErrorString);
				}
				else if (nStatus == LOGIN_REEULT_BIND_PHONE)
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_INVALID_USER_PASSWORD);
					strErrorString = "帐号或密码错误";
					log("phone is binded(%d)", IM::BaseDefine::REFUSE_REASON_INVALID_USER_PASSWORD);
				}
				else if (nStatus == -1)
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_USER_NOT_EXIST);
					log("user is deleted:(%d)", IM::BaseDefine::REFUSE_REASON_USER_NOT_EXIST);
					strErrorString = "您的帐号已被删除，请联系管理员处理";
					//msgResp.set_result_string(strErrorString);
				}
				else if (nStatus == LOGIN_RESULT_LOCKED_BY_EC) //locked by ec
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_LOCK_USER);
					log("user is locked by ec:(%d)", IM::BaseDefine::REFUSE_REASON_LOCK_USER);
					strErrorString = "您的帐号已被锁定，请联系管理员处理";
					//msgResp.set_result_string(strErrorString);
				}
				else if (nStatus == LOGIN_RESULT_FORCE_CHG_PASSWORD)
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_FORCE_CHANGE_PASSWORD);
					log("force to change password:(%d)", IM::BaseDefine::REFUSE_REASON_FORCE_CHANGE_PASSWORD);
					strErrorString = "您的密码为初始密码，请您立即修改密码";
					//msgResp.set_result_string(strErrorString);
				}
				else if (nStatus == LOGIN_RESULT_EXPIRED_PASSWORD)
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_FORCE_CHANGE_PASSWORD);
					log("expired password and force to change password:(%d)", IM::BaseDefine::REFUSE_REASON_FORCE_CHANGE_PASSWORD);
					strErrorString = "您的密码已经过期，请您立即修改密码";
				}
				else if (nStatus == LOGIN_RESULT_NO_BUY_PRODUCT)
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_NOT_BUY_PRODUCT);
					log("user no privillege to use the product:(%d)", IM::BaseDefine::REFUSE_REASON_NOT_BUY_PRODUCT);
					strErrorString = "您没有当前产品使用权限，请联系管理员处理";
					//msgResp.set_result_string(strErrorString);
				}
				else if (nStatus == LOGIN_RESULT_INACTIVE_USER)
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_INACTIVE_USER);
					log("inactive user:(%d)", IM::BaseDefine::REFUSE_REASON_INACTIVE_USER);
					strErrorString = "未被激活，请先激活";
					//msgResp.set_result_string(strErrorString);
				}
				else if (nStatus == LOGIN_RESULT_LIMITED_IP)
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_LIMITED_IP);
					log("limited ip:(%d)", IM::BaseDefine::REFUSE_REASON_LIMITED_IP);
					strErrorString = "您当前访问的IP受限";

				}
				//else if (nStatus == LOGIN_RESULT_LOGIN_BY_ALIAS_NAME) //login by alias name
				//{
				//	msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_INVALID_ALIAS_PASSWORD);
				//	log("invalid alias name and passwod:(%d)", IM::BaseDefine::REFUSE_REASON_INVALID_ALIAS_PASSWORD);
				//	strErrorString = cUser.user_name();
				//	//msgResp.set_result_string(cUser.user_name());
				//}
				else //invalid user name and password
				{
					msgResp.set_result_code(IM::BaseDefine::REFUSE_REASON_INVALID_USER_PASSWORD);
					strErrorString = "帐号或密码错误";
					log("invalid account/password(%d)", IM::BaseDefine::REFUSE_REASON_INVALID_USER_PASSWORD);
					//msgResp.set_result_string(strErrorString);
				}
				msgResp.set_result_string(strErrorString);
			}
		}
		else
		{
			msgResp.set_result_code(0x0002);
			//msgResp.set_result_string("服务端内部错误");
			msgResp.set_result_string("登录错误，请联系管理员处理");
			log("failed to login,please contact the administrator(2)");
		}


		pPduResp->SetPBMsg(&msgResp);
		pPduResp->SetSeqNum(pPdu->GetSeqNum());
		pPduResp->SetServiceId(IM::BaseDefine::SID_OTHER);
		pPduResp->SetCommandId(IM::BaseDefine::CID_OTHER_VALIDATE_RSP);
		CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
	}
};


