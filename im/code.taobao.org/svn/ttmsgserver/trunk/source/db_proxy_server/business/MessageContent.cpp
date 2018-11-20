/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：MessageContent.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include "../ProxyConn.h"
#include "../CachePool.h"
#include "../DBPool.h"
#include "MessageContent.h"
#include "MessageModel.h"
#include "GroupMessageModel.h"
#include "Common.h"
#include "GroupModel.h"
#include "ImPduBase.h"
#include "IM.Message.pb.h"
#include "SessionModel.h"
#include "RelationModel.h"
#include "UserModel.h"

namespace DB_PROXY {

    void getMessage(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMGetMsgListReq msg;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            uint32_t nPeerId = msg.session_id();
            uint32_t nMsgId = msg.msg_id_begin();
            uint32_t nMsgCnt = msg.msg_cnt();
            IM::BaseDefine::SessionType nSessionType = msg.session_type();
            if(IM::BaseDefine::SessionType_IsValid(nSessionType))
            {
                CImPdu* pPduResp = new CImPdu;
                IM::Message::IMGetMsgListRsp msgResp;

                list<IM::BaseDefine::MsgInfo> lsMsg;

                if(nSessionType == IM::BaseDefine::SESSION_TYPE_SINGLE)//获取个人消息
                {
                    CMessageModel::getInstance()->getMessage(nUserId, nPeerId, nMsgId, nMsgCnt, lsMsg);
                }
                else if(nSessionType == IM::BaseDefine::SESSION_TYPE_GROUP)//获取群消息
                {
                    if(nUserId == 0 || CGroupModel::getInstance()->isInGroup(nUserId, nPeerId))
                    {
                        CGroupMessageModel::getInstance()->getMessage(nUserId, nPeerId, nMsgId, nMsgCnt, lsMsg);
                    }
                }

                msgResp.set_user_id(nUserId);
                msgResp.set_session_id(nPeerId);
                msgResp.set_msg_id_begin(nMsgId);
                msgResp.set_session_type(nSessionType);
                for(auto it=lsMsg.begin(); it!=lsMsg.end();++it)
                {
                    IM::BaseDefine::MsgInfo* pMsg = msgResp.add_msg_list();
        //            *pMsg = *it;
                    pMsg->set_msg_id(it->msg_id());
                    pMsg->set_from_session_id(it->from_session_id());
                    pMsg->set_create_time(it->create_time());
                    pMsg->set_msg_type(it->msg_type());
                    pMsg->set_msg_data(it->msg_data());
					pMsg->set_msg_flag(it->msg_flag());
//                    log("userId=%u, peerId=%u, msgId=%u", nUserId, nPeerId, it->msg_id());
                }

                log("userId=%u, peerId=%u, msgId=%u, msgCnt=%u, count=%u", nUserId, nPeerId, nMsgId, nMsgCnt, msgResp.msg_list_size());
                msgResp.set_attach_data(msg.attach_data());
                pPduResp->SetPBMsg(&msgResp);
                pPduResp->SetSeqNum(pPdu->GetSeqNum());
                pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
                pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_LIST_RESPONSE);
                CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
            }
            else
            {
                log("invalid sessionType. userId=%u, peerId=%u, msgId=%u, msgCnt=%u, sessionType=%u",
                    nUserId, nPeerId, nMsgId, nMsgCnt, nSessionType);
            }
        }
        else
        {
            log("parse pb failed");
        }
    }

    void sendMessage(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMMsgData msg;
		int nResult = 0;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nFromId = msg.from_user_id();
            uint32_t nToId = msg.to_session_id();
            uint32_t nCreateTime = msg.create_time();
            IM::BaseDefine::MsgType nMsgType = msg.msg_type();
			uint32_t nMsgId = INVALID_VALUE;
            uint32_t nMsgLen = msg.msg_data().length();

			//get at user list.Added by george on April 21st, 2016
			uint32_t unAtUsers = msg.at_user_list_size();
			string strAtUsers = "";
			uint32_t unMsgFlag = IM::BaseDefine::MSG_FLAG_NORMAL;
			int nResult1 = 0;
			int nResult2 = 0;

			if (unAtUsers == 1)
			{
				unMsgFlag = IM::BaseDefine::MSG_FLAG_AT_MSG;

				if (msg.at_user_list(0) == 9999)  //at all members
				{
					strAtUsers = "userid <> " + int2string(nFromId);
				}
				else
				{
					strAtUsers = "userid = " + int2string(msg.at_user_list(0));
				}
			}
			else if (unAtUsers > 1)
			{
				unMsgFlag = IM::BaseDefine::MSG_FLAG_AT_MSG;
				strAtUsers = "userid in (" + int2string(msg.at_user_list(0));
				for (int i = 1; i < unAtUsers; i++)
				{
					strAtUsers += "," + msg.at_user_list(i);
				}
				strAtUsers += ")";
			}

			log("At users: %s", strAtUsers.c_str());
			//end of getting at user lists
      
            uint32_t nNow = (uint32_t)time(NULL);
			CImPdu* pPduResp = new CImPdu;

            if (IM::BaseDefine::MsgType_IsValid(nMsgType))
            {
                if(nMsgLen != 0)
                {
                    //uint32_t nMsgId = INVALID_VALUE;
                    uint32_t nSessionId = INVALID_VALUE;
                    uint32_t nPeerSessionId = INVALID_VALUE;

                    CMessageModel* pMsgModel = CMessageModel::getInstance();
                    CGroupMessageModel* pGroupMsgModel = CGroupMessageModel::getInstance();
                    if(nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_TEXT)
					{
                        CGroupModel* pGroupModel = CGroupModel::getInstance();
						//if (pGroupModel->isValidateGroupId(nToId) && pGroupModel->isInGroup(nFromId, nToId))
						if (!pGroupModel->isValidateGroupId(nToId))
						{
							nResult = MESSAGE_CONTENT_INVALID_GROUP_ID;
							log("invalid groupid: %u, user id: %u", nToId, nFromId);
						}
						else if (!pGroupModel->isInGroup(nFromId, nToId))
						{
							nResult = MESSAGE_CONTENT_INVALID_GROUP_USER;
							log("invalid group member: %u, user id: %u", nToId, nFromId);
						}
						else
                        {
                            nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
                            if (INVALID_VALUE == nSessionId)
							{
                                nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
                            }

							if (nSessionId != INVALID_VALUE)
							{
								nMsgId = pGroupMsgModel->getMsgId(nToId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pGroupMsgModel->sendMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data(), unMsgFlag))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(strAtUsers, nFromId, nToId, nMsgId, nNow); //update session for at users,Added by george on April 21st,2016
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send group text message: from %d to group %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("failed to get group message id : from %d to group %d", nFromId, nToId);	
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("failed to add session id for group; %d", nToId);
							}
                        }
                       /* else
                        {
                            log("invalid groupId. fromId=%u, groupId=%u", nFromId, nToId);
							nResult = 0x2000;
                        }*/
                    }
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_AUDIO)
					{
                        CGroupModel* pGroupModel = CGroupModel::getInstance();
						//if (pGroupModel->isValidateGroupId(nToId) && pGroupModel->isInGroup(nFromId, nToId))
						if (!pGroupModel->isValidateGroupId(nToId))
						{
							nResult = MESSAGE_CONTENT_INVALID_GROUP_ID;
							log("invalid groupid: %u, user id: %u", nToId, nFromId);
						}
						else if (!pGroupModel->isInGroup(nFromId, nToId))
						{
							nResult = MESSAGE_CONTENT_INVALID_GROUP_USER;
							log("invalid group member: %u, user id: %u", nToId, nFromId);
						}
						else
                        {
                            nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
                            if (INVALID_VALUE == nSessionId)
							{
                                nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
                            }
                            if(nSessionId != INVALID_VALUE)
                            {
                                nMsgId = pGroupMsgModel->getMsgId(nToId);
                                if(nMsgId != INVALID_VALUE)
                                {
									if (pGroupMsgModel->sendAudioMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg.msg_data().c_str(), nMsgLen, unMsgFlag))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(strAtUsers, nFromId, nToId, nMsgId, nNow); //update session for at users,Added by george on April 21st,2016
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send group audio message: from %d to group %d", nFromId, nToId);
									}
                                }
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("failed to get group message id: from %d to group %d", nFromId, nToId);
								}
                            }
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("failed to add session id for group; %d", nToId);
							}
                        }
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_FILE)
					{
						CGroupModel* pGroupModel = CGroupModel::getInstance();
						//if (pGroupModel->isValidateGroupId(nToId) && pGroupModel->isInGroup(nFromId, nToId))
						if (!pGroupModel->isValidateGroupId(nToId))
						{
							nResult = MESSAGE_CONTENT_INVALID_GROUP_ID;
							log("invalid groupid: %u, user id: %u", nToId, nFromId);
						}
						else if (!pGroupModel->isInGroup(nFromId, nToId))
						{
							nResult = MESSAGE_CONTENT_INVALID_GROUP_USER;
							log("invalid group member: %u, user id: %u", nToId, nFromId);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
							}

							if (nSessionId != INVALID_VALUE)
							{
								nMsgId = pGroupMsgModel->getMsgId(nToId);
								if (nMsgId != INVALID_VALUE)
								{
									string strFileDetails;
									if (pGroupMsgModel->sendFileMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg.msg_data().c_str(), nMsgLen, strFileDetails, unMsgFlag))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(strAtUsers, nFromId, nToId, nMsgId, nNow); //update session for at users,Added by george on April 21st,2016
										msg.set_msg_data(strFileDetails);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send group file message: from %d to group %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("failed to get group message id: from %d to group %d", nFromId, nToId);				
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("failed to add session id for group; %d", nToId);
							}
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_SYSTEM)
					{
						CGroupModel* pGroupModel = CGroupModel::getInstance();
						if (pGroupModel->isValidateGroupId(nToId))
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
							}
							if (nSessionId != INVALID_VALUE)
							{
								nMsgId = pGroupMsgModel->getMsgId(nToId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pGroupMsgModel->sendMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data(), unMsgFlag))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(strAtUsers, nFromId, nToId, nMsgId, nNow); //update session for at users,Added by george on April 21st,2016
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send group system message: from %d to group %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("failed to get group message id: from %d to group %d", nFromId, nToId);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("failed to add session id for group; %d", nToId);
							}
						}
						else
						{
							nResult = MESSAGE_CONTENT_INVALID_GROUP_ID;
							log("invalid groupId. fromId=%u, groupId=%u", nFromId, nToId);
						}
					}
					//else if (nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_WORKFLOW)
					//{
					//	CGroupModel* pGroupModel = CGroupModel::getInstance();
					//	if (pGroupModel->isValidateGroupId(nToId))
					//	{
					//		nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
					//		if (INVALID_VALUE == nSessionId)
					//		{
					//			nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
					//		}
					//		if (nSessionId != INVALID_VALUE)
					//		{
					//			nMsgId = pGroupMsgModel->getMsgId(nToId);
					//			if (nMsgId != INVALID_VALUE)
					//			{
					//				if (pGroupMsgModel->sendMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data(), unMsgFlag))
					//				{
					//					CSessionModel::getInstance()->updateSession(nSessionId, nNow);
					//					CSessionModel::getInstance()->updateSession(strAtUsers, nFromId, nToId, nMsgId, nNow); //update session for at users,Added by george on April 21st,2016
					//				}
					//				else
					//				{
					//					nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
					//					log("failed to send group workflow message: from %d to group %d", nFromId, nToId);
					//				}
					//			}
					//			else
					//			{							
					//				nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
					//				log("failed to get group message id: from %d to group %d", nFromId, nToId);
					//			}
					//		}
					//		else
					//		{
					//			nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
					//			log("failed to add session id for group; %d", nToId);
					//		}
					//	}
					//	else
					//	{
					//		nResult = MESSAGE_CONTENT_INVALID_GROUP_ID;
					//		log("invalid groupId. fromId=%u, groupId=%u", nFromId, nToId);
					//		
					//	}
					//}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_EMAIL)
					{
						CGroupModel* pGroupModel = CGroupModel::getInstance();
						if (pGroupModel->isValidateGroupId(nToId))
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
							}
							if (nSessionId != INVALID_VALUE)
							{
								nMsgId = pGroupMsgModel->getMsgId(nToId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pGroupMsgModel->sendMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data(), unMsgFlag))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(strAtUsers, nFromId, nToId, nMsgId, nNow); //update session for at users,Added by george on April 21st,2016
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send group mail message: from %d to group %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("failed to get group message id: from %d to group %d", nFromId, nToId);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("failed to add session id for group; %d", nToId);
							}
						}
						else
						{
							log("invalid groupId. fromId=%u, groupId=%u", nFromId, nToId);
							nResult = MESSAGE_CONTENT_INVALID_GROUP_ID;
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_FILE_STATUS)
					{
						CGroupModel* pGroupModel = CGroupModel::getInstance();
						if (pGroupModel->isValidateGroupId(nToId))
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
							}
							if (nSessionId != INVALID_VALUE)
							{
								nMsgId = pGroupMsgModel->getMsgId(nToId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pGroupMsgModel->sendMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data(), unMsgFlag))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(strAtUsers, nFromId, nToId, nMsgId, nNow); //update session for at users,Added by george on April 21st,2016
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send group file status message: from %d to group %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("failed to get group message id: from %d to group %d", nFromId, nToId);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("failed to add session id for group; %d", nToId);
							}
						}
						else
						{
							log("invalid groupId. fromId=%u, groupId=%u", nFromId, nToId);
							nResult = MESSAGE_CONTENT_INVALID_GROUP_ID;
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_BULLETIN) //added by george on Dec 31st, 2015
					{
						CGroupModel* pGroupModel = CGroupModel::getInstance();
						if (pGroupModel->isValidateGroupId(nToId))
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
							}
							if (nSessionId != INVALID_VALUE)
							{
								nMsgId = pGroupMsgModel->getMsgId(nToId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pGroupMsgModel->sendMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data(), unMsgFlag))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(strAtUsers, nFromId, nToId, nMsgId, nNow); //update session for at users,Added by george on April 21st,2016
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send group bulletin message: from %d to group %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("failed to get group message id: from %d to group %d", nFromId, nToId);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("failed to add session id for group; %d", nToId);
							}
						}
						else
						{
							log("invalid groupId. fromId=%u, groupId=%u", nFromId, nToId);
							nResult = MESSAGE_CONTENT_INVALID_GROUP_ID;
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_TEXT)
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						//if (nFromId != nToId && pUserModel && pUserModel->IsValidUser(nFromId) && pUserModel->IsValidUser(nToId))
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}        
						else
						{
                            nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                            if (INVALID_VALUE == nSessionId)
							{
                                nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                            }
                            nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                            if(INVALID_VALUE ==  nPeerSessionId)
                            {
                                nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                            }
                            uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
                            if(nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
                            {
                                nMsgId = pMsgModel->getMsgId(nRelateId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data()))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single text message. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
									}
                                }
                                else
                                {
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
                                    log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                                }
                            }
                            else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
                                log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                            }
                        }
                      /*  else
                        {
                            log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
							nResult = 0x0002;
                        }*/
                        
                    }
					else if(nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_AUDIO)
					{   
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else 
						{
                            nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                            if (INVALID_VALUE == nSessionId)
							{
                                nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                            }
                            nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                            if(INVALID_VALUE ==  nPeerSessionId)
                            {
                                nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                            }
                            uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
                            if(nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
                            {
                                nMsgId = pMsgModel->getMsgId(nRelateId);
                                if(nMsgId != INVALID_VALUE)
								{
									if (pMsgModel->sendAudioMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg.msg_data().c_str(), nMsgLen))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single audio message: from user %d tu user %d", nFromId, nToId);
									}
                                }
                                else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
                                    log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
                                }
                            }
                            else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
                                log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                            }
                        }
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_FILE)
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nPeerSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
							if (nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
							{
								nMsgId = pMsgModel->getMsgId(nRelateId);
								if (nMsgId != INVALID_VALUE)
								{
									string strFileDetails;
									if (pMsgModel->sendFileMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg.msg_data().c_str(), nMsgLen, strFileDetails))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
										msg.set_msg_data(strFileDetails);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single file message: from user %d tu user %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
							}
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_SYSTEM)
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nPeerSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
							if (nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
							{
								nMsgId = pMsgModel->getMsgId(nRelateId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data()))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single system message: from user %d tu user %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
							}
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_WORKFLOW) //added by george on Nov 11th, 2015
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nPeerSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
							if (nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
							{
								nMsgId = pMsgModel->getMsgId(nRelateId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data()))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single workflow message: from user %d tu user %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
							}
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_EMAIL) //added by george on Dec 21st, 2015
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nPeerSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
							if (nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
							{
								nMsgId = pMsgModel->getMsgId(nRelateId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data()))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single email message: from user %d tu user %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
							}
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_FILE_STATUS)
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nPeerSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
							if (nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
							{
								uint32_t nResult = pMsgModel->sendFileStatusMessage(nRelateId, nFromId, msg.msg_data().c_str(), msg.msg_data().length()); //added by george on May 5th,2016

								if (nResult > 0) //change file status from ready to received
								{
									nMsgId = pMsgModel->getMsgId(nRelateId);
									if (nMsgId != INVALID_VALUE)
									{
										if (pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data(), nResult))
										{
											CSessionModel::getInstance()->updateSession(nSessionId, nNow);
											CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
										}
										else
										{
											nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
											log("failed to send single file status message: from user %d tu user %d", nFromId, nToId);
										}
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_FILE_IS_RECEIVED;
									log("file is received: Relateid: %u, from id: %u, to id: %u", nRelateId, nFromId, nToId);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
							}
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_BULLETIN) //added by george on Dec 31st, 2015
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nPeerSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
							if (nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
							{
								nMsgId = pMsgModel->getMsgId(nRelateId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data()))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single bulletin message: from user %d tu user %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
							}
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_NEW_FRIEND) //added by george on July 19th, 2016
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nPeerSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
							if (nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
							{
								nMsgId = pMsgModel->getMsgId(nRelateId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data()))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single message(new friend): from user %d tu user %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
							}
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_NEW_STAFF) //added by george on Aug 1st, 2015
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nPeerSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
							if (nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
							{
								nMsgId = pMsgModel->getMsgId(nRelateId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data()))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single message(new staff): from user %d tu user %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
							}
						}
					}
					else if (nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_ATTENDANCE) //added by george on Aug 1st, 2015
					{
						CUserModel *pUserModel = CUserModel::getInstance();
						nResult1 = pUserModel->IsValidUser(nFromId);
						nResult2 = pUserModel->IsValidUser(nToId);

						if (nFromId == nToId)
						{
							nResult = MESSAGE_CONTENT_SEND_TO_SELF_ERROR;
							log("send msg to self. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
						}
						else if (nResult1 != 1)
						{
							nResult = GetResultCodeForSender(nResult1);
						}
						else if (nResult2 != 1)
						{
							nResult = GetResultCodeForPeer(nResult2);
						}
						else
						{
							nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
							if (INVALID_VALUE == nPeerSessionId)
							{
								nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
							}
							uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
							if (nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
							{
								nMsgId = pMsgModel->getMsgId(nRelateId);
								if (nMsgId != INVALID_VALUE)
								{
									if (pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data()))
									{
										CSessionModel::getInstance()->updateSession(nSessionId, nNow);
										CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
									}
									else
									{
										nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
										log("failed to send single message(attendance): from user %d tu user %d", nFromId, nToId);
									}
								}
								else
								{
									nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
									log("msgId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
								}
							}
							else
							{
								nResult = MESSAGE_CONTENT_INTERNAL_ERROR;
								log("sessionId or relateId is invalid. fromId=%u, toId=%u, nRelateId=%u, nSessionId=%u, nMsgType=%u", nFromId, nToId, nRelateId, nSessionId, nMsgType);
							}
						}
					}

                    log("fromId=%u, toId=%u, type=%u, msgId=%u, sessionId=%u", nFromId, nToId, nMsgType, nMsgId, nSessionId);
                }
                else
                {
                    log("msgLen error. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
					nResult = MESSAGE_CONTENT_INVALID_MSG_LEN;
                }
            }
            else
            {
                log("invalid msgType.fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nMsgType);
				nResult = MESSAGE_CONTENT_INVALID_MSG_TYPE;
            }

			if (nMsgId == INVALID_VALUE) //invalid msgid,then msg data is set by error code
			{
				/*char szErrorCode[16];
				memset(szErrorCode, 0, sizeof(szErrorCode));
				sprintf(szErrorCode, "%lu", nResult);
				msg.set_msg_data(szErrorCode);*/
				string strErrorString = int2string(nResult);
			/*	switch (nResult)
				{
					case MESSAGE_CONTENT_INTERNAL_ERROR:
						strErrorString = "系统内部错误";
						break;

					case MESSAGE_CONTENT_SENDER_DEL_USER:
						strErrorString = "您已被删除";
						break;
					
					case MESSAGE_CONTENT_SENDER_DISABLE_USER:
						strErrorString = "您已被禁用";
						break;

					case MESSAGE_CONTENT_SENDER_NONIM_USER:
						strErrorString = "您未开通IM产品";
						break;

					case MESSAGE_CONTENT_PEER_DEL_USER:
						strErrorString = "对方已被删除";
						break;

					case MESSAGE_CONTENT_PEER_DISABLE_USER:
						strErrorString = "对方已被禁用";
						break;

					case MESSAGE_CONTENT_PEER_NONIM_USER:
						strErrorString = "对方未开通IM产品";
						break;

					case MESSAGE_CONTENT_INVALID_GROUP_ID:
						strErrorString = "非法群组";
						break;

					case MESSAGE_CONTENT_INVALID_GROUP_USER:
						strErrorString = "您为非法群成员";
						break;

					case MESSAGE_CONTENT_UNKNOWN_ERROR:
						strErrorString = "求知错误";
						break;

					case MESSAGE_CONTENT_FILE_IS_RECEIVED:
						strErrorString = "文件已被接收过";
						break;

					case MESSAGE_CONTENT_INVALID_MSG_LEN:
						strErrorString = "非法消息长度";
						break;

					case MESSAGE_CONTENT_INVALID_MSG_TYPE:
						strErrorString = "非法消息类别";
						break;

				}
				strErrorString.append(":");
				strErrorString += int2string(nResult);*/
				msg.set_msg_data(strErrorString);
			}

			msg.set_msg_id(nMsgId);
			pPduResp->SetPBMsg(&msg);
			pPduResp->SetSeqNum(pPdu->GetSeqNum());
			pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
			pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_DATA);
			CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        else
        {
            log("parse pb failed");
        }
    }

    void getMessageById(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMGetMsgByIdReq msg;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            IM::BaseDefine::SessionType nType = msg.session_type();
            uint32_t nPeerId = msg.session_id();
            list<uint32_t> lsMsgId;
            uint32_t nCnt = msg.msg_id_list_size();
            for(uint32_t i=0; i<nCnt; ++i)
            {
                lsMsgId.push_back(msg.msg_id_list(i));
            }
            if (IM::BaseDefine::SessionType_IsValid(nType))
            {
                CImPdu* pPduResp = new CImPdu;
                IM::Message::IMGetMsgByIdRsp msgResp;

                list<IM::BaseDefine::MsgInfo> lsMsg;
                if(IM::BaseDefine::SESSION_TYPE_SINGLE == nType)
                {
                    CMessageModel::getInstance()->getMsgByMsgId(nUserId, nPeerId, lsMsgId, lsMsg);
                }
                else if(IM::BaseDefine::SESSION_TYPE_GROUP)
                {
                    CGroupMessageModel::getInstance()->getMsgByMsgId(nUserId, nPeerId, lsMsgId, lsMsg);
                }
                msgResp.set_user_id(nUserId);
                msgResp.set_session_id(nPeerId);
                msgResp.set_session_type(nType);
                for(auto it=lsMsg.begin(); it!=lsMsg.end(); ++it)
                {
                    IM::BaseDefine::MsgInfo* pMsg = msgResp.add_msg_list();
                    pMsg->set_msg_id(it->msg_id());
                    pMsg->set_from_session_id(it->from_session_id());
                    pMsg->set_create_time(it->create_time());
                    pMsg->set_msg_type(it->msg_type());
                    pMsg->set_msg_data(it->msg_data());
                }
                log("userId=%u, peerId=%u, sessionType=%u, reqMsgCnt=%u, resMsgCnt=%u", nUserId, nPeerId, nType, msg.msg_id_list_size(), msgResp.msg_list_size());
                msgResp.set_attach_data(msg.attach_data());
                pPduResp->SetPBMsg(&msgResp);
                pPduResp->SetSeqNum(pPdu->GetSeqNum());
                pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
                pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_GET_BY_MSG_ID_RES);
                CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
            }
            else
            {
                log("invalid sessionType. fromId=%u, toId=%u, sessionType=%u, msgCnt=%u", nUserId, nPeerId, nType, nCnt);
            }
        }
        else
        {
            log("parse pb failed");
        }
    }

    void getLatestMsgId(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMGetLatestMsgIdReq msg;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            IM::BaseDefine::SessionType nType = msg.session_type();
            uint32_t nPeerId = msg.session_id();
            if (IM::BaseDefine::SessionType_IsValid(nType)) {
                CImPdu* pPduResp = new CImPdu;
                IM::Message::IMGetLatestMsgIdRsp msgResp;
                msgResp.set_user_id(nUserId);
                msgResp.set_session_type(nType);
                msgResp.set_session_id(nPeerId);
                uint32_t nMsgId = INVALID_VALUE;
                if(IM::BaseDefine::SESSION_TYPE_SINGLE == nType)
                {
                    string strMsg;
                    IM::BaseDefine::MsgType nMsgType;
                    CMessageModel::getInstance()->getLastMsg(nUserId, nPeerId, nMsgId, strMsg, nMsgType, 1);
                }
                else
                {
                    string strMsg;
                    IM::BaseDefine::MsgType nMsgType;
                    uint32_t nFromId = INVALID_VALUE;
                    CGroupMessageModel::getInstance()->getLastMsg(nPeerId, nMsgId, strMsg, nMsgType, nFromId);
                }
                msgResp.set_latest_msg_id(nMsgId);
                log("userId=%u, peerId=%u, sessionType=%u, msgId=%u", nUserId, nPeerId, nType,nMsgId);
                msgResp.set_attach_data(msg.attach_data());
                pPduResp->SetPBMsg(&msgResp);
                pPduResp->SetSeqNum(pPdu->GetSeqNum());
                pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
                pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_GET_LATEST_MSG_ID_RSP);
                CProxyConn::AddResponsePdu(conn_uuid, pPduResp);

            }
            else
            {
                log("invalid sessionType. userId=%u, peerId=%u, sessionType=%u", nUserId, nPeerId, nType);
            }
        }
        else
        {
            log("parse pb failed");
        }
    }

	//don't forget to change when getMessageById changes
	void getResendMessageById(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Message::IMGetResendMsgByIdReq msg;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{
			uint32_t nUserId = msg.user_id();
			uint32_t unResendUserID = msg.resend_user_id();
			IM::BaseDefine::SessionType nType = msg.session_type();
			uint32_t nPeerId = msg.session_id();
			list<uint32_t> lsMsgId;
			uint32_t nCnt = msg.msg_id_list_size();
			for (uint32_t i = 0; i<nCnt; ++i)
			{
				lsMsgId.push_back(msg.msg_id_list(i));
			}
			if (IM::BaseDefine::SessionType_IsValid(nType))
			{
				CImPdu* pPduResp = new CImPdu;
				IM::Message::IMGetResendMsgByIdRsp msgResp;

				list<IM::BaseDefine::MsgInfo> lsMsg;
				if (IM::BaseDefine::SESSION_TYPE_SINGLE == nType)
				{
					CMessageModel::getInstance()->getMsgByMsgId(nUserId, nPeerId, lsMsgId, lsMsg, false);
				}
				else if (IM::BaseDefine::SESSION_TYPE_GROUP)
				{
					CGroupMessageModel::getInstance()->getMsgByMsgId(nUserId, nPeerId, lsMsgId, lsMsg, false);
				}
				msgResp.set_resend_user_id(unResendUserID);
				msgResp.set_user_id(nUserId);
				msgResp.set_session_id(nPeerId);
				msgResp.set_session_type(nType);
				for (auto it = lsMsg.begin(); it != lsMsg.end(); ++it)
				{
					IM::BaseDefine::MsgInfo* pMsg = msgResp.add_msg_list();
					pMsg->set_msg_id(it->msg_id());
					pMsg->set_from_session_id(it->from_session_id());
					pMsg->set_create_time(it->create_time());
					pMsg->set_msg_type(it->msg_type());
					pMsg->set_msg_data(it->msg_data());
				}
				log("userId=%u, peerId=%u, sessionType=%u, reqMsgCnt=%u, resMsgCnt=%u", nUserId, nPeerId, nType, msg.msg_id_list_size(), msgResp.msg_list_size());
				msgResp.set_attach_data(msg.attach_data());
				pPduResp->SetPBMsg(&msgResp);
				pPduResp->SetSeqNum(pPdu->GetSeqNum());
				pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
				pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_RESEND_MSG_RES);
				CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
			}
			else
			{
				log("invalid sessionType. fromId=%u, toId=%u, sessionType=%u, msgCnt=%u", nUserId, nPeerId, nType, nCnt);
			}
		}
		else
		{
			log("parse pb failed");
		}
	}


	int GetResultCodeForSender(int nStatus)
	{
		int nErrorCode = MESSAGE_CONTENT_UNKNOWN_ERROR;
		switch (nStatus)
		{
			case 0:
				nErrorCode = MESSAGE_CONTENT_SENDER_DISABLE_USER;
				break;
			case -1:
				nErrorCode = MESSAGE_CONTENT_SENDER_DEL_USER;
				break;
			case -2:
				nErrorCode = MESSAGE_CONTENT_SENDER_NONIM_USER;
				break;
			default:
				log("unknown error for sender");
		}
		return nErrorCode;
	}


	int GetResultCodeForPeer(int nStatus)
	{
		int nErrorCode = MESSAGE_CONTENT_UNKNOWN_ERROR;
		switch (nStatus)
		{
			case 0:
				nErrorCode = MESSAGE_CONTENT_PEER_DISABLE_USER;
				break;
			case -1:
				nErrorCode = MESSAGE_CONTENT_PEER_DEL_USER;
				break;
			case -2:
				nErrorCode = MESSAGE_CONTENT_PEER_NONIM_USER;
				break;
			default:
				log("unknown error for receiver");
		}

		return nErrorCode;
	}
};


