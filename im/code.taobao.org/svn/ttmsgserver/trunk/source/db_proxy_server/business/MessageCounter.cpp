/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：MessageCounter.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include "../ProxyConn.h"
#include "../CachePool.h"
#include "MessageCounter.h"
#include "MessageModel.h"
#include "GroupMessageModel.h"
#include "IM.Message.pb.h"
#include "IM.BaseDefine.pb.h"
#include "IM.Login.pb.h"
#include "IM.Server.pb.h"
#include "UserModel.h"

namespace DB_PROXY {

    void getUnreadMsgCounter(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMUnreadMsgCntReq msg;
        IM::Message::IMUnreadMsgCntRsp msgResp;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            CImPdu* pPduResp = new CImPdu;

            uint32_t nUserId = msg.user_id();

            list<IM::BaseDefine::UnreadInfo> lsUnreadCount;
            uint32_t nTotalCnt = 0;
            
            CMessageModel::getInstance()->getUnreadMsgCount(nUserId, nTotalCnt, lsUnreadCount);
            CGroupMessageModel::getInstance()->getUnreadMsgCount(nUserId, nTotalCnt, lsUnreadCount);
            msgResp.set_user_id(nUserId);
            msgResp.set_total_cnt(nTotalCnt);
            for(auto it= lsUnreadCount.begin(); it!=lsUnreadCount.end(); ++it)
            {
                IM::BaseDefine::UnreadInfo* pInfo = msgResp.add_unreadinfo_list();
    //            *pInfo = *it;
                pInfo->set_session_id(it->session_id());
                pInfo->set_session_type(it->session_type());
                pInfo->set_unread_cnt(it->unread_cnt());
                pInfo->set_latest_msg_id(it->latest_msg_id());
                pInfo->set_latest_msg_data(it->latest_msg_data());
                pInfo->set_latest_msg_type(it->latest_msg_type());
                pInfo->set_latest_msg_from_user_id(it->latest_msg_from_user_id());
            }
            
            
            log("userId=%d, unreadCnt=%u, totalCount=%u", nUserId, msgResp.unreadinfo_list_size(), nTotalCnt);
            msgResp.set_attach_data(msg.attach_data());
            pPduResp->SetPBMsg(&msgResp);
            pPduResp->SetSeqNum(pPdu->GetSeqNum());
            pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
            pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_UNREAD_CNT_RESPONSE);
            CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        else
        {
            log("parse pb failed");
        }
    }

    void clearUnreadMsgCounter(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMMsgDataReadAck msg;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            uint32_t nFromId = msg.session_id();
            IM::BaseDefine::SessionType nSessionType = msg.session_type();
            CUserModel::getInstance()->clearUserCounter(nUserId, nFromId, nSessionType);
            log("userId=%u, peerId=%u, type=%u", nFromId, nUserId, nSessionType);
        }
        else
        {
            log("parse pb failed");
        }
    }
        
    void setDevicesToken(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Login::IMDeviceTokenReq msg;
        IM::Login::IMDeviceTokenRsp msgResp;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            string strToken = msg.device_token();
			string strType;
            CImPdu* pPduResp = new CImPdu;
            CacheManager* pCacheManager = CacheManager::getInstance();
            CacheConn* pCacheConn = pCacheManager->GetCacheConn("token");
            if (pCacheConn)
            {
                IM::BaseDefine::ClientType nClientType = msg.client_type();
                string strValue;
			
				if (IM::BaseDefine::CLIENT_TYPE_IOS == nClientType)
				{
					strValue = "ios:" + strToken;
					strType = "ios";
				}
				else if (IM::BaseDefine::CLIENT_TYPE_ANDROID == nClientType)
				{
					strValue = "android:" + strToken;
					strType = "andriod";
				}
				else
				{
					strType = "other";
					strValue = strToken;
				}
               
				//get user's original device token
                string strOldValue = pCacheConn->get("device_"+int2string(nUserId));
				//string strOldValue = pCacheConn->hget("device_" + int2string(nUserId), strType);
                
                if(!strOldValue.empty())
                {
                    size_t nPos = strOldValue.find(":");
                    if(nPos!=string::npos)
                    {
                        string strOldToken = strOldValue.substr(nPos + 1);
						if (!strOldToken.empty())
						{
							string strReply = pCacheConn->get("device_" + strOldToken);
							if (!strReply.empty())
							{
								string strNewValue("");
								pCacheConn->set("device_" + strOldToken, strNewValue);
								log("USER %d 's ORIGINAL TOKEN IS: %s,which BELONGS TO %s,now CHANGE TO \"\"", nUserId, strOldToken.c_str(), strReply.c_str());
							}
						}
                    }
                }
                
				//get original user that current token binded 
				if (strToken != "")
				{
					string strOldUser = pCacheConn->get("device_" + strToken);
					if (!strOldUser.empty())
					{
						string strNewValue("");
						pCacheConn->set("device_" + strOldUser, strNewValue);
						log("TOKEN: %s is previously BELONGS TO %s,now CHANGE %s's token to \"\" ", strToken.c_str(), strOldUser.c_str(), strOldUser.c_str());
					}
				}
		
               /* pCacheConn->set("device_"+int2string(nUserId), strValue);
                string strNewValue = int2string(nUserId);
                pCacheConn->set("device_"+strToken, strNewValue);*/
            	
				//pCacheConn->hset("device_" + int2string(nUserId), strType, strValue); //one user login with multi-terminals(andriod and ios)

				string strNewValue = int2string(nUserId);
				pCacheConn->set("device_" + int2string(nUserId), strValue);
				if (strToken != "")
				{	
					pCacheConn->set("device_" + strToken, strNewValue);
				}
			
			    // log("setDeviceToken. userId=%u, deviceToken=%s", nUserId, strToken.c_str());
                pCacheManager->RelCacheConn(pCacheConn);

				log("setDeviceToken. SET:userId=%u, deviceToken=%s", nUserId, strValue.c_str());
				log("setDeviceToken. SET:deviceToken=%s, userId=%s", strToken.c_str(), strNewValue.c_str());
            }
            else
            {
                log("no cache connection for token");
            }
            
            msgResp.set_attach_data(msg.attach_data());
            msgResp.set_user_id(nUserId);
            pPduResp->SetPBMsg(&msgResp);
            pPduResp->SetSeqNum(pPdu->GetSeqNum());
            pPduResp->SetServiceId(IM::BaseDefine::SID_LOGIN);
            pPduResp->SetCommandId(IM::BaseDefine::CID_LOGIN_RES_DEVICETOKEN);
            CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        else
        {
            log("parse pb failed");
        }
    }


	void getDevicesToken(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Server::IMGetDeviceTokenReq msg;
		IM::Server::IMGetDeviceTokenRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			CacheManager* pCacheManager = CacheManager::getInstance();
			CacheConn* pCacheConn = pCacheManager->GetCacheConn("token");
			CImPdu* pPduResp = new CImPdu;
			uint32_t nCnt = msg.user_id_size();
			if (pCacheConn)
			{
				//map<string, string> mapTokens;

				for (uint32_t i = 0; i < nCnt; ++i)
				{
					int nUserId = msg.user_id(i);
					string strKey = "device_" + int2string(nUserId);
					string strValue = pCacheConn->get(strKey);	
					//bool bRet = pCacheConn->hgetAll(strKey, mapTokens);
					//if (bRet)
					//{
					//string strKey = it->second;
					//log("KEY2: %s, VALUE:%s", it->first.c_str(), it->second.c_str());
					//string strValue = it->second;
					size_t nPos = strValue.find(":");
					if (nPos != string::npos)
					{
						//string strType = it->first;
						string strType = strValue.substr(0, nPos);
						string strToken = strValue.substr(nPos + 1);
						IM::BaseDefine::ClientType nClientType = IM::BaseDefine::ClientType(0);
						if (strType == "ios")
						{
							nClientType = IM::BaseDefine::CLIENT_TYPE_IOS;
						}
						else if (strType == "android")
						{
							nClientType = IM::BaseDefine::CLIENT_TYPE_ANDROID;
						}

						//george added on Oct 21st,2015
						if (!strToken.empty())
						{
							string strKeyToken = "device_" + strToken;
							string strNewUserID = pCacheConn->get(strKeyToken);
							uint32_t unNewUserID = 0;
							if (!strNewUserID.empty())
							{
								unNewUserID = string2int(strNewUserID);
							}

							if (nUserId == unNewUserID) //double judgement,token--->user matches and user-->token 
							{
								//end of adding
								if (IM::BaseDefine::ClientType_IsValid(nClientType))
								{
									IM::BaseDefine::UserTokenInfo* pToken = msgResp.add_user_token_info();
									pToken->set_user_id(nUserId);
									pToken->set_token(strToken);
									pToken->set_user_type(nClientType);
									uint32_t nTotalCnt = 0;
									CMessageModel::getInstance()->getUnReadCntAll(nUserId, nTotalCnt);
									CGroupMessageModel::getInstance()->getUnReadCntAll(nUserId, nTotalCnt);
									pToken->set_push_count(nTotalCnt);
									pToken->set_push_type(1);
								}
								else
								{
									log("invalid clientType.clientType=%u", nClientType);
								}
							}
							else
							{
								log("USER: %d ----> TOKEN: %s mismatches TOKEN:%s---->USER:%s", nUserId, strToken.c_str(), strToken.c_str(), strNewUserID.c_str());
							}
						}
					}
					else
					{
						//log("invalid value. value=%s", strValue.c_str());
					}
				}
				pCacheManager->RelCacheConn(pCacheConn);
			//}
					/*else
					{
						log("hgetAll failed!");
					}*/
				//}
			
			}
			else
			{
				log("no cache connection for token");
			}

			log("req devices token.reqCnt=%u, resCnt=%u", nCnt, msgResp.user_token_info_size());

			msgResp.set_attach_data(msg.attach_data());
			pPduResp->SetPBMsg(&msgResp);
			pPduResp->SetSeqNum(pPdu->GetSeqNum());
			pPduResp->SetServiceId(IM::BaseDefine::SID_OTHER);
			pPduResp->SetCommandId(IM::BaseDefine::CID_OTHER_GET_DEVICE_TOKEN_RSP);
			CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
		}
		else
		{
			log("parse pb failed");
		}
	}






    //void getDevicesToken(CImPdu* pPdu, uint32_t conn_uuid)
    //{
    //    IM::Server::IMGetDeviceTokenReq msg;
    //    IM::Server::IMGetDeviceTokenRsp msgResp;
    //    if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
    //    {
    //        CacheManager* pCacheManager = CacheManager::getInstance();
    //        CacheConn* pCacheConn = pCacheManager->GetCacheConn("token");
    //        CImPdu* pPduResp = new CImPdu;
    //        uint32_t nCnt = msg.user_id_size();
    //        if (pCacheConn)
    //        {
    //            vector<string> vecTokens;
    //            for (uint32_t i=0; i<nCnt; ++i) {
    //                string strKey = "device_"+int2string(msg.user_id(i));
    //                vecTokens.push_back(strKey);
				//	log("KEY: %s", strKey.c_str());
    //            }
    //            map<string, string> mapTokens;
    //            bool bRet = pCacheConn->mget(vecTokens, mapTokens);
    //            //pCacheManager->RelCacheConn(pCacheConn);
    //            
    //            if(bRet)
    //            {
    //                for (auto it=mapTokens.begin(); it!=mapTokens.end(); ++it)
				//	{
    //                    string strKey = it->first;
				//		log("KEY2: %s, VALUE:%s", it->first.c_str(), it->second.c_str());

    //                    size_t nPos = strKey.find("device_");
    //                    if( nPos != string::npos)
    //                    {
    //                        string strUserId = strKey.substr(nPos + strlen("device_"));
    //                        uint32_t nUserId = string2int(strUserId);
    //                        string strValue = it->second;
    //                        nPos = strValue.find(":");
    //                        if(nPos!=string::npos)
    //                        {
    //                            string strType = strValue.substr(0, nPos);
    //                            string strToken = strValue.substr(nPos + 1);
    //                            IM::BaseDefine::ClientType nClientType = IM::BaseDefine::ClientType(0);
    //                            if(strType == "ios")
    //                            {
    //                                nClientType = IM::BaseDefine::CLIENT_TYPE_IOS;
    //                            }
    //                            else if(strType == "android")
    //                            {
    //                                nClientType = IM::BaseDefine::CLIENT_TYPE_ANDROID;
    //                            }

				//				//george added on Oct 21st,2015
				//				string strKeyToken = "device_" + strToken;
				//				string strNewUserID = pCacheConn->get(strKeyToken);
				//				uint32_t unNewUserID = 0;
				//				if (!strNewUserID.empty())
				//				{
				//					unNewUserID = string2int(strNewUserID);
				//				}

				//				if (nUserId == unNewUserID) //double judgement,token--->user matches and user-->token 
				//				{
				//					//end of adding
				//					if (IM::BaseDefine::ClientType_IsValid(nClientType))
				//					{
				//						IM::BaseDefine::UserTokenInfo* pToken = msgResp.add_user_token_info();
				//						pToken->set_user_id(nUserId);
				//						pToken->set_token(strToken);
				//						pToken->set_user_type(nClientType);
				//						uint32_t nTotalCnt = 0;
				//						CMessageModel::getInstance()->getUnReadCntAll(nUserId, nTotalCnt);
				//						CGroupMessageModel::getInstance()->getUnReadCntAll(nUserId, nTotalCnt);
				//						pToken->set_push_count(nTotalCnt);
				//						pToken->set_push_type(1);
				//					}
				//					else
				//					{
				//						log("invalid clientType.clientType=%u", nClientType);
				//					}
				//				}
				//				else
				//				{
				//					log("USER: %s ----> TOKEN: %s mismatches TOKEN:%s---->USER:%s",strUserId.c_str(), strToken.c_str(), strToken.c_str(), strNewUserID.c_str());
				//				}
    //                        }
    //                        else
    //                        {
    //                            log("invalid value. value=%s", strValue.c_str());
    //                        }
    //                        
    //                    }
    //                    else
    //                    {
    //                        log("invalid key.key=%s", strKey.c_str());
    //                    }
    //                }
    //            }
    //            else
    //            {
    //                log("mget failed!");
    //            }

				//pCacheManager->RelCacheConn(pCacheConn);
    //        }
    //        else
    //        {
    //            log("no cache connection for token");
    //        }
    //        
    //        log("req devices token.reqCnt=%u, resCnt=%u", nCnt, msgResp.user_token_info_size());
    //        
    //        msgResp.set_attach_data(msg.attach_data());
    //        pPduResp->SetPBMsg(&msgResp);
    //        pPduResp->SetSeqNum(pPdu->GetSeqNum());
    //        pPduResp->SetServiceId(IM::BaseDefine::SID_OTHER);
    //        pPduResp->SetCommandId(IM::BaseDefine::CID_OTHER_GET_DEVICE_TOKEN_RSP);
    //        CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
    //    }
    //    else
    //    {
    //        log("parse pb failed");
    //    }
    //}
};


