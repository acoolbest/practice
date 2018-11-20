/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：GroupAction.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include "../ProxyConn.h"
#include "GroupAction.h"
#include "GroupModel.h"
#include "IM.Group.pb.h"
#include "IM.BaseDefine.pb.h"
#include "public_define.h"
#include "IM.Server.pb.h"
#include "UserModel.h"

namespace DB_PROXY {
    
    /**
     *  创建群组
     *
     *  @param pPdu      收到的packet包指针
     *  @param conn_uuid 该包过来的socket 描述符
     */
    void createGroup(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Group::IMGroupCreateReq msg;
        IM::Group::IMGroupCreateRsp msgResp;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            CImPdu* pPduRes = new CImPdu;
            
            uint32_t nUserId = msg.user_id();
            string strGroupName = msg.group_name();
			uint32_t unCompanyId = msg.company_id();

            IM::BaseDefine::GroupType nGroupType = msg.group_type();
            if(IM::BaseDefine::GroupType_IsValid(nGroupType))
            {
                string strGroupAvatar = msg.group_avatar();
                set<uint32_t> setMember;
                uint32_t nMemberCnt = msg.member_id_list_size();
                for(uint32_t i=0; i<nMemberCnt; ++i)
                {
                    uint32_t nUserId = msg.member_id_list(i);
                    setMember.insert(nUserId);
                }
                log("createGroup.%d create %s, userCnt=%u", nUserId, strGroupName.c_str(), setMember.size());
                
				string strGroupUUID = "";
				generate_id(strGroupUUID);

                uint32_t nGroupId = CGroupModel::getInstance()->createGroup(nUserId, strGroupName, strGroupAvatar, nGroupType, setMember, strGroupUUID, unCompanyId);
                msgResp.set_user_id(nUserId);
                msgResp.set_group_name(strGroupName);
				msgResp.set_group_uuid(strGroupUUID);
				msgResp.set_company_id(unCompanyId);

                for(auto it=setMember.begin(); it!=setMember.end();++it)
                {
                    msgResp.add_user_id_list(*it);
                }
                if(nGroupId != INVALID_VALUE)
                {
                    msgResp.set_result_code(0);
                    msgResp.set_group_id(nGroupId);
                }
                else
                {
                    msgResp.set_result_code(1);
                }
                
                
                log("createGroup.%d create %s, userCnt=%u, result:%d", nUserId, strGroupName.c_str(), setMember.size(), msgResp.result_code());
                
                msgResp.set_attach_data(msg.attach_data());
                pPduRes->SetPBMsg(&msgResp);
                pPduRes->SetSeqNum(pPdu->GetSeqNum());
                pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
                pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_CREATE_RESPONSE);
                CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
            }
            else
            {
                log("invalid group type.userId=%u, groupType=%u, groupName=%s", nUserId, nGroupType, strGroupName.c_str());
            }
        }
        else
        {
            log("parse pb failed");
        }
    }
    
    /**
     *  获取正式群列表
     *
     *  @param pPdu      收到的packet包指针
     *  @param conn_uuid 该包过来的socket 描述符
     */
    void getNormalGroupList(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Group::IMNormalGroupListReq msg;
        IM::Group::IMNormalGroupListRsp msgResp;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            CImPdu* pPduRes = new CImPdu;
            
            uint32_t nUserId = msg.user_id();
			uint32_t unLatestTime = msg.latest_update_time();
            
            list<IM::BaseDefine::GroupVersionInfo> lsGroup;
            //CGroupModel::getInstance()->getUserGroup(nUserId, lsGroup, IM::BaseDefine::GROUP_TYPE_NORMAL, unLatestTime);
			CGroupModel::getInstance()->getUserGroup(nUserId, lsGroup, unLatestTime);
            msgResp.set_user_id(nUserId);
            for(auto it=lsGroup.begin(); it!=lsGroup.end(); ++it)
            {
                IM::BaseDefine::GroupVersionInfo* pGroupVersion = msgResp.add_group_version_list();
                pGroupVersion->set_group_id(it->group_id());
                pGroupVersion->set_version(it->version());
				//log("GroupID: %d, Version: %d", it->group_id(), it->version());
            }
            
            log("getNormalGroupList. userId=%u, count=%d", nUserId, msgResp.group_version_list_size());
            
            msgResp.set_attach_data(msg.attach_data());
            pPduRes->SetPBMsg(&msgResp);
            pPduRes->SetSeqNum(pPdu->GetSeqNum());
            pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
            pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_NORMAL_LIST_RESPONSE);
            CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
        }
        else
        {
            log("parse pb failed");
        }
    }
    
    /**
     *  获取群信息
     *
     *  @param pPdu      收到的packet包指针
     *  @param conn_uuid 该包过来的socket 描述符
     */
    void getGroupInfo(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Group::IMGroupInfoListReq msg;
        IM::Group::IMGroupInfoListRsp msgResp;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            CImPdu* pPduRes = new CImPdu;
            uint32_t nUserId = msg.user_id();
            uint32_t nGroupCnt = msg.group_version_list_size();
            
            map<uint32_t, IM::BaseDefine::GroupVersionInfo> mapGroupId;
            for(uint32_t i=0; i<nGroupCnt; ++i)
            {
                IM::BaseDefine::GroupVersionInfo groupInfo = msg.group_version_list(i);
				//mapGroupId[groupInfo.group_id()] = groupInfo;

                if(CGroupModel::getInstance()->isValidateGroupId(groupInfo.group_id()))
                {
                    mapGroupId[groupInfo.group_id()] = groupInfo;
                }
            }
            list<IM::BaseDefine::GroupInfo> lsGroupInfo;
            CGroupModel::getInstance()->getGroupInfo(mapGroupId, lsGroupInfo, nUserId);
            
            msgResp.set_user_id(nUserId);
            for(auto it=lsGroupInfo.begin(); it!=lsGroupInfo.end(); ++it)
            {
                IM::BaseDefine::GroupInfo* pGroupInfo = msgResp.add_group_info_list();
    //            *pGroupInfo = *it;
                pGroupInfo->set_group_id(it->group_id());
                pGroupInfo->set_version(it->version());
                pGroupInfo->set_group_name(it->group_name());
                pGroupInfo->set_group_avatar(it->group_avatar());
                pGroupInfo->set_group_creator_id(it->group_creator_id());
                pGroupInfo->set_group_type(it->group_type());
                pGroupInfo->set_shield_status(it->shield_status());
				pGroupInfo->set_status(it->status());
				pGroupInfo->set_email(it->email());
				pGroupInfo->set_group_uuid(it->group_uuid());
				pGroupInfo->set_company_id(it->company_id());
                uint32_t nGroupMemberCnt = it->group_member_list_size();
                for (uint32_t i=0; i<nGroupMemberCnt; ++i) {
                    uint32_t userId = it->group_member_list(i);
                    pGroupInfo->add_group_member_list(userId);
                }
            }
            
            log("userId=%u, requestCount=%u", nUserId, nGroupCnt);
            
            msgResp.set_attach_data(msg.attach_data());
            pPduRes->SetPBMsg(&msgResp);
            pPduRes->SetSeqNum(pPdu->GetSeqNum());
            pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
            pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_INFO_RESPONSE);
            CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
        }
        else
        {
            log("parse pb failed");
        }
    }
    /**
     *  修改群成员，增加或删除
     *
     *  @param pPdu      收到的packet包指针
     *  @param conn_uuid 该包过来的socket 描述符
     */
    void modifyMember(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Group::IMGroupChangeMemberReq msg;
		IM::Group::IMGroupChangeMemberTempRsp msgResp;
        //IM::Group::IMGroupChangeMemberRsp msgResp;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
			CImPdu* pPduRes = new CImPdu;

            uint32_t nUserId = msg.user_id();
            uint32_t nGroupId = msg.group_id();
            IM::BaseDefine::GroupModifyType nType = msg.change_type();
            if (IM::BaseDefine::GroupModifyType_IsValid(nType) &&
                CGroupModel::getInstance()->isValidateGroupId(nGroupId)) {
                
                uint32_t nCnt = msg.member_id_list_size();
                set<uint32_t> setUserId;
				BOOL bIsQuit = FALSE;

                for(uint32_t i=0; i<nCnt;++i)
                {
					if (((nType == IM::BaseDefine::GroupModifyType::GROUP_MODIFY_TYPE_DEL) || (nType == IM::BaseDefine::GroupModifyType::GROUP_MODIFY_TYPE_DEL_BY_SELF))
						&& (msg.member_id_list(i) == nUserId))
					{
						bIsQuit = TRUE;

					}
                    setUserId.insert(msg.member_id_list(i));
                }

				list<uint32_t> lsCurUserId;
				if (bIsQuit == TRUE)
				{
					uint32_t nCreator ;
					string strName;
					int nResult = 0;
					set<uint32_t> setDeletedUsers;
					bool bRet = CGroupModel::getInstance()->hasModifyPermission(nUserId, nGroupId, nType,nCreator, strName,false);
					if (bRet)
					{
						if (nCreator == nUserId)
						{
							log("%d Removing the group: %d", nUserId, nGroupId);
							CGroupModel::getInstance()->removeGroup(nUserId, nGroupId, lsCurUserId, setDeletedUsers);
							for (auto itNode = setDeletedUsers.begin(); itNode != setDeletedUsers.end(); ++itNode)
							{
								uint32_t nCurrUserId = *itNode;
								msgResp.add_chg_user_id_list(nCurrUserId);
								log("Clear group user count...........");
								CUserModel::getInstance()->clearUserCounter(nCurrUserId, nGroupId, IM::BaseDefine::SESSION_TYPE_GROUP);
							}
							log("Removing session.................");
							CGroupModel::getInstance()->removeSession(nGroupId, setDeletedUsers); //remove sessions  for all users in the group
						}
						else
						{
							log("%d quit the group: %d", nUserId, nGroupId);
							msgResp.add_chg_user_id_list(nUserId);
							CGroupModel::getInstance()->removeMember(nGroupId, setUserId, lsCurUserId);
							CGroupModel::getInstance()->removeSession(nGroupId, setUserId); //remove sessions  for all removed users
							CUserModel::getInstance()->clearUserCounter(nUserId, nGroupId, IM::BaseDefine::SESSION_TYPE_GROUP);
						}
					}
					else
					{
						nResult = 0x2002;
						log("%d couldn't quit the group: %d", nUserId, nGroupId);
					}

					msgResp.set_user_id(nUserId);
					msgResp.set_group_id(nGroupId);
					msgResp.set_change_type(nType);
					msgResp.set_result_code(nResult);
					msgResp.set_creator_id(nCreator); //added on Aug 25th,2015
					msgResp.set_group_name(strName);
					//george.jiang inserted the following lines on July 6th, 2015
					//set current id list
					for (auto itNode = lsCurUserId.begin(); itNode != lsCurUserId.end(); ++itNode)
					{
						msgResp.add_cur_user_id_list(*itNode);
					}
					log("userId=%u quit group: groupId=%u, result=%u, changeCount:%u, currentCount=%u", nUserId, nGroupId, nResult, msgResp.chg_user_id_list_size(), msgResp.cur_user_id_list_size());
					log("CREATOR: %d, GROUP NAME: %s", nCreator, strName.c_str());

					msgResp.set_attach_data(msg.attach_data());
					pPduRes->SetPBMsg(&msgResp);
					pPduRes->SetSeqNum(pPdu->GetSeqNum());
					pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
					pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_CHANGE_MEMBER_RESPONSE);
					CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
				}
				else
				{
					string strGrpName;
					uint32_t nGroupOwner = 0;
					bool bRet = CGroupModel::getInstance()->modifyGroupMember(nUserId, nGroupId, nType, setUserId, lsCurUserId, strGrpName, nGroupOwner);
					msgResp.set_user_id(nUserId);
					msgResp.set_group_id(nGroupId);
					msgResp.set_change_type(nType);
					msgResp.set_result_code(bRet?0:1);
					msgResp.set_creator_id(nGroupOwner); //creator id is set to 0 when adding a member
					msgResp.set_group_name(strGrpName);
					if(bRet)
					{
						for(auto it=setUserId.begin(); it!=setUserId.end(); ++it)
						{
							msgResp.add_chg_user_id_list(*it);
						}

						for(auto it=lsCurUserId.begin(); it!=lsCurUserId.end(); ++it)
						{
							msgResp.add_cur_user_id_list(*it);
						}
					}
					else
					{
						log("group operation failed: USER ID: %d, GROUP ID: %d", nUserId, nGroupId);
						int nResult = 0x02000;
						switch (nType)
						{
							case IM::BaseDefine::GROUP_MODIFY_TYPE_ADD:
								nResult = 0x02001;
								break;
							case IM::BaseDefine::GROUP_MODIFY_TYPE_DEL:
								nResult = 0x2002;
								break;
							case IM::BaseDefine::GROUP_MODIFY_TYPE_DEL_BY_SELF:
								nResult = 0x2001;
						}
						msgResp.set_result_code(nResult);
					}
					log("userId=%u, groupId=%u, result=%u, changeCount:%u, currentCount=%u, group name: %s",nUserId, nGroupId,  bRet?0:1, msgResp.chg_user_id_list_size(), msgResp.cur_user_id_list_size(),strGrpName.c_str());
					msgResp.set_attach_data(msg.attach_data());
					pPduRes->SetPBMsg(&msgResp);
					pPduRes->SetSeqNum(pPdu->GetSeqNum());
					pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
					pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_CHANGE_MEMBER_RESPONSE);
					CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
				}
            }
            else
            {
                log("invalid groupModifyType or groupId. userId=%u, groupId=%u, groupModifyType=%u", nUserId, nGroupId, nType);
				int nResult = 0x2000;
				msgResp.set_user_id(nUserId);
				msgResp.set_group_id(nGroupId);
				msgResp.set_change_type(nType);
				msgResp.set_result_code(nResult);
				msgResp.set_creator_id(0);
				msgResp.set_group_name("");
				msgResp.set_attach_data(msg.attach_data());
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
				pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_CHANGE_MEMBER_RESPONSE);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
            }          
        }
        else
        {
            log("parse pb failed");
        }
    }
    
    /**
     *  设置群组信息推送，屏蔽或者取消屏蔽
     *
     *  @param pPdu      收到的packet包指针
     *  @param conn_uuid 该包过来的socket 描述符
     */
    void setGroupPush(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Group::IMGroupShieldReq msg;
        IM::Group::IMGroupShieldRsp msgResp;
		int nResult = 0;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            uint32_t nGroupId = msg.group_id();
            uint32_t nStatus = msg.shield_status();

			CImPdu* pPduRes = new CImPdu;

            if(CGroupModel::getInstance()->isValidateGroupId(nGroupId))
            {
                bool bRet = CGroupModel::getInstance()->setPush(nUserId, nGroupId, IM_GROUP_SETTING_PUSH, nStatus);
				bRet = CGroupModel::getInstance()->SetShieldStatus(nUserId, nGroupId, nStatus);
				nResult = bRet ? 0 : 1;
                
             /* msgResp.set_user_id(nUserId);
                msgResp.set_group_id(nGroupId);
                msgResp.set_result_code(nResult);*/
            
                log("userId=%u, groupId=%u, result=%u", nUserId, nGroupId, msgResp.result_code());
                
             /*   msgResp.set_attach_data(msg.attach_data());
                pPduRes->SetPBMsg(&msgResp);
                pPduRes->SetSeqNum(pPdu->GetSeqNum());
                pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
                pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_SHIELD_GROUP_RESPONSE);
                CProxyConn::AddResponsePdu(conn_uuid, pPduRes);*/
            }
            else
            {
                log("Invalid group.userId=%u, groupId=%u", nUserId, nGroupId);
				nResult = 0x2000;
            }

			msgResp.set_user_id(nUserId);
			msgResp.set_group_id(nGroupId);
			msgResp.set_result_code(nResult);
			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
			pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_SHIELD_GROUP_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
        }
        else
        {
            log("parse pb failed");
        }
    }
    
    /**
     *  获取一个群的推送设置
     *
     *  @param pPdu      收到的packet包指针
     *  @param conn_uuid 该包过来的socket 描述符
     */
    void getGroupPush(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Server::IMGroupGetShieldReq msg;
        IM::Server::IMGroupGetShieldRsp msgResp;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nGroupId = msg.group_id();
            uint32_t nUserCnt = msg.user_id_size();

            if(CGroupModel::getInstance()->isValidateGroupId(nGroupId))
            {
				CImPdu* pPduRes = new CImPdu;

                list<uint32_t> lsUser;
                for(uint32_t i=0; i<nUserCnt; ++i)
                {
                    lsUser.push_back(msg.user_id(i));
                }
                list<IM::BaseDefine::ShieldStatus> lsPush;
                CGroupModel::getInstance()->getPush(nGroupId, lsUser, lsPush);
                
                msgResp.set_group_id(nGroupId);
                for (auto it=lsPush.begin(); it!=lsPush.end(); ++it) {
                    IM::BaseDefine::ShieldStatus* pStatus = msgResp.add_shield_status_list();
        //            *pStatus = *it;
                    pStatus->set_user_id(it->user_id());
                    pStatus->set_group_id(it->group_id());
                    pStatus->set_shield_status(it->shield_status());
                }
                
                log("groupId=%u, count=%u", nGroupId, nUserCnt);
                
                msgResp.set_attach_data(msg.attach_data());
                pPduRes->SetPBMsg(&msgResp);
                pPduRes->SetSeqNum(pPdu->GetSeqNum());
                pPduRes->SetServiceId(IM::BaseDefine::SID_OTHER);
                pPduRes->SetCommandId(IM::BaseDefine::CID_OTHER_GET_SHIELD_RSP);
                CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
            }
            else
            {
                log("Invalid groupId. nGroupId=%u", nGroupId);
            }		
        }
        else
        {
            log("parse pb failed");
        }
    }

	void modifyGroupName(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Group::IMGroupChangeNameReq msg;
        IM::Group::IMGroupChangeNameAndNotifyRsp msgResp;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            uint32_t nGroupId = msg.group_id();
			string strGroupName = msg.group_new_name();

			CImPdu* pPduRes = new CImPdu;

			msgResp.set_user_id(nUserId);
			msgResp.set_group_id(nGroupId);
			msgResp.set_change_type(IM::BaseDefine::GroupModifyType::GROUP_MODIFY_TYPE_CHANGE_GRPNAME);
			msgResp.set_group_new_name(strGroupName);

            if(CGroupModel::getInstance()->isValidateGroupId(nGroupId))
			{     
              
				list<uint32_t> lsGroupUser;
				bool bRet = CGroupModel::getInstance()->updateGroupName(nUserId, nGroupId, strGroupName, lsGroupUser);
				msgResp.set_result_code(bRet?0:1);
				
				for (auto it = lsGroupUser.begin(); it != lsGroupUser.end(); ++it)
				{
					msgResp.add_cur_user_id_list(*it);
				}
				log("userId=%u, groupId=%u, result=%u, group new name=%s, users:%d",nUserId, nGroupId,  bRet?0:1, strGroupName.c_str(), lsGroupUser.size());
			/*	msgResp.set_attach_data(msg.attach_data());
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
				pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_CHANGE_GRPNAME_RESPONSE);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);*/
            }
            else
            {
                log("invalid groupId. userId=%u, groupId=%u", nUserId, nGroupId);
				int nResult = 0x02000;
				msgResp.set_result_code(nResult);
            } 

			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
			pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_CHANGE_GRPNAME_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
        }
        else
        {
            log("parse pb failed");
        }
	}

	void modifyGroupLeader(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Group::IMGroupTransferLeaderReq msg;
        IM::Group::IMGroupTransferLeaderAndNotifyRsp msgResp;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            uint32_t nGroupId = msg.group_id();
			uint32_t nNewLeaderId = msg.new_leader_id();

			CImPdu* pPduRes = new CImPdu;

			msgResp.set_user_id(nUserId);
			msgResp.set_group_id(nGroupId);
			msgResp.set_new_leader_id(nNewLeaderId);

            if(CGroupModel::getInstance()->isValidateGroupId(nGroupId))
			{     
               
				list<uint32_t> lsGroupUser;
				bool bRet = CGroupModel::getInstance()->updateGroupLeader(nUserId,nGroupId,nNewLeaderId, lsGroupUser);
				
				msgResp.set_result_code(bRet?0:1);
				msgResp.set_change_type(IM::BaseDefine::GroupModifyType::GROUP_MODIFY_TYPE_TRANSFER_LEADER);
				if (bRet)
				{
					for (auto it = lsGroupUser.begin(); it != lsGroupUser.end(); ++it)
					{
						msgResp.add_cur_user_id_list(*it);
					}
				}
				log("userId=%u, groupId=%u, result=%u, new leader id=%u, users:%d",nUserId, nGroupId,  bRet?0:1, nNewLeaderId, lsGroupUser.size());
			/*	msgResp.set_attach_data(msg.attach_data());
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
				pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_TRANSFER_LEADER_RESPONSE);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);*/
            }
            else
            {
                log("invalid groupId. userId=%u, groupId=%u", nUserId, nGroupId);
				int nResult = 0x2000;
				msgResp.set_result_code(nResult);
            }  

			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_GROUP);
			pPduRes->SetCommandId(IM::BaseDefine::CID_GROUP_TRANSFER_LEADER_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
        }
        else
        {
            log("parse pb failed");
        }
	}
}

