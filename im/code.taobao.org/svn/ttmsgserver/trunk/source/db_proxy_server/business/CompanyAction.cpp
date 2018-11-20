/*================================================================
*     Copyright (c) 2015�� lanhu. All rights reserved.
*   
*   �ļ����ƣ�DepartAction.cpp
*   �� �� �ߣ�Zhang Yuanhao
*   ��    �䣺bluefoxah@gmail.com
*   �������ڣ�2015��03��13��
*   ��    ����
*
================================================================*/
#include "CompanyAction.h"
#include "CompanyModel.h"
#include "IM.Buddy.pb.h"
#include "../ProxyConn.h"

namespace DB_PROXY{
	void getCompanyInfo(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMCompanyReq msg;
		IM::Buddy::IMCompanyRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength())) 
		{

			CImPdu* pPduRes = new CImPdu;

			uint32_t nUserId = msg.user_id();
			uint32_t nLastUpdate = msg.latest_update_time();

			uint32_t nCompanyId = 0;
			//CCompanytModel::getInstance()->getChgedCompanyId(nUserId,nCompanyId);

			//IM::BaseDefine::CompanyInfo lsDeparts;
			//CCompanytModel::getInstance()->getCompany(nCompanyId, lsDeparts);

			msgResp.set_user_id(nUserId);
			msgResp.set_latest_update_time(nLastUpdate);
			////IM::BaseDefine::CompanyInfo *pCompanyInfo = msgResp.mutable_dept_list();
			//IM::BaseDefine::CompanyInfo *pCompanyInfo = msgResp.add_company_info_list();
			//if (pCompanyInfo)
			//{
			//	pCompanyInfo->set_company_name(lsDeparts.company_name());
			//	pCompanyInfo->set_company_id(nCompanyId);
			//}

			/*IM::BaseDefine::CompanyInfo *pCompanyInfo = new IM::BaseDefine::CompanyInfo;
			pCompanyInfo->CopyFrom(lsDeparts);
			msgResp.set_allocated_dept_list(pCompanyInfo);*/

			//log("companyId=%u, companyname=%s", nCompanyId, pCompanyInfo->company_name().c_str());
			msgResp.set_attach_data(msg.attach_data());
			pPduRes->SetPBMsg(&msgResp);
			pPduRes->SetSeqNum(pPdu->GetSeqNum());
			pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
			pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_COMPANY_RESPONSE);
			CProxyConn::AddResponsePdu(conn_uuid, pPduRes);

		}
		else
		{
			log("parse pb failed");
		}
	}
}