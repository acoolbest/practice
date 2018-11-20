/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：DepartAction.cpp
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月13日
*   描    述：
*
================================================================*/
#include "DepartAction.h"
#include "DepartModel.h"
#include "IM.Buddy.pb.h"
#include "../ProxyConn.h"

namespace DB_PROXY{
    void getChgedDepart(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Buddy::IMDepartmentReq msg;
        IM::Buddy::IMDepartmentRsp msgResp;
        if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{          
            uint32_t unUserId = msg.user_id();
			if(unUserId <= 0)
			{
				log("nUserId <= 0,failed");
				return;
			}

            uint32_t nLastUpdate = msg.latest_update_time();
			uint32_t unCompanyId = msg.company_id();
			//uint32_t unCompanyId = 0;
			//CDepartModel::getInstance()->getCompanyId(nUserId, unCompanyId); //get company id

			list<IM::BaseDefine::DepartInfo> lsDeparts;
            //CDepartModel::getInstance()->getDepts(unCompanyId,nLastUpdate, lsDeparts);
			//CDepartModel::getInstance()->GetDepartments(unUserId, nLastUpdate, lsDeparts);
			CDepartModel::getInstance()->GetDepartments(unCompanyId, nLastUpdate, lsDeparts);
        
			uint32_t unCount = lsDeparts.size();
			uint32_t unTotalCount = 0;
            
            msgResp.set_user_id(unUserId);
            msgResp.set_latest_update_time(0);
			msgResp.set_company_id(unCompanyId);
			msgResp.set_attach_data(msg.attach_data());

            for(auto it=lsDeparts.begin(); it!=lsDeparts.end(); ++it)
            {
                IM::BaseDefine::DepartInfo* pDeptInfo = msgResp.add_dept_list();
				pDeptInfo->set_dept_uuid(it->dept_uuid());
				pDeptInfo->set_company_id(it->company_id());
				pDeptInfo->set_parent_dept_uuid(it->parent_dept_uuid());
				pDeptInfo->set_dept_name(it->dept_name());
				pDeptInfo->set_dept_status(it->dept_status());
				pDeptInfo->set_priority(it->priority());
				pDeptInfo->set_dept_level(it->dept_level());
				pDeptInfo->set_contact_status(it->contact_status());
				pDeptInfo->set_company_uuid(it->company_uuid());

				if ((++unTotalCount) % MAX_DEPART_COUNT_EACH_TIME == 0)
				{
					if (unTotalCount == unCount)
					{
						msgResp.set_latest_update_time(nLastUpdate);
					}

					CImPdu* pPduRes = new CImPdu;
					pPduRes->SetPBMsg(&msgResp);
					pPduRes->SetSeqNum(pPdu->GetSeqNum());
					pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
					pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_DEPARTMENT_RESPONSE);
					CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
					log("userId=%u, last_update=%u, 0, cnt=%u, curr count: %u", unUserId, nLastUpdate, unCount, unTotalCount);

					msgResp.clear_dept_list();
					//unMaxCount = 0;
				}
            }
			//log("userId=%u, last_update=%u, cnt=%u", nUserId, nLastUpdate, lsDeparts.size());

			if (unTotalCount % MAX_DEPART_COUNT_EACH_TIME != 0 || unCount == 0)
			{
				CImPdu* pPduRes = new CImPdu;
				//msgResp.set_attach_data(msg.attach_data());
				msgResp.set_latest_update_time(nLastUpdate);
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
				pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_DEPARTMENT_RESPONSE);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
				log("userId=%u, last_update=%u, cnt=%u, curr count: %u", unUserId, nLastUpdate, unCount, unTotalCount);
			}
        }
        else
        {
            log("parse pb failed");
        }
    }
}