#include "PositionAction.h"
#include "PositionModel.h"
#include "IM.Buddy.pb.h"
#include "../ProxyConn.h"

namespace DB_PROXY{
	void getChangedPosition(CImPdu* pPdu, uint32_t conn_uuid)
	{
		IM::Buddy::IMPositionReq msg;
		IM::Buddy::IMPositionRsp msgResp;
		if (msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
		{

			CImPdu* pPduRes = new CImPdu;

			uint32_t nUserId = msg.user_id();
			uint32_t nLastUpdate = msg.latest_update_time();
			uint32_t unCompanyId = msg.company_id();

			CPositionModel *pPosition = CPositionModel::getInstance();
			list<IM::BaseDefine::PositionInfo> listPositions;

			if (pPosition)
			{
				/*pPosition->getCompanyId(nUserId, nCompanyId);
				pPosition->getPositions(nCompanyId, nLastUpdate, listPositions);*/
				//pPosition->GetPositions(nUserId, nLastUpdate, listPositions);
				pPosition->GetPositions(unCompanyId, nLastUpdate, listPositions);
				pPosition = NULL;
			}


			msgResp.set_user_id(nUserId);
			msgResp.set_attach_data(msg.attach_data());
			msgResp.set_latest_update_time(0);
			msgResp.set_company_id(unCompanyId);
			
			uint32_t nCount = listPositions.size();
			uint32_t unTotalCount = 0;

			for (list<IM::BaseDefine::PositionInfo>::iterator it = listPositions.begin(); it != listPositions.end(); it++)
			{
				IM::BaseDefine::PositionInfo *pPositionInfo = msgResp.add_position_list();
				pPositionInfo->set_position_uuid(it->position_uuid());
				pPositionInfo->set_position_type(it->position_type());
				pPositionInfo->set_position_name(it->position_name());
				pPositionInfo->set_status(it->status());
				pPositionInfo->set_sort(it->sort());
				pPositionInfo->set_company_id(it->company_id());

				if ((++unTotalCount) % MAX_POSITION_COUNT_EACH_TIME == 0)
				{
					if (unTotalCount == nCount)
					{
						msgResp.set_latest_update_time(nLastUpdate);
					}

					CImPdu* pPduRes = new CImPdu;
					pPduRes->SetPBMsg(&msgResp);
					pPduRes->SetSeqNum(pPdu->GetSeqNum());
					pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
					pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_POSITION_RESPONSE);
					CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
					log("user id: %u, last update timea; %u, total posiitons: %u, actual positions: %u", nUserId, nLastUpdate, nCount, unTotalCount);

					//unMaxCount = 0;
					msgResp.clear_position_list();
				}		
			}
		
			if (unTotalCount % MAX_POSITION_COUNT_EACH_TIME != 0 || nCount == 0)
			{
				msgResp.set_latest_update_time(nLastUpdate);
				log("user id: %u, last update timea; %u, total posiitons: %u, actual positions: %u", nUserId, nLastUpdate, nCount, unTotalCount);
				CImPdu* pPduRes = new CImPdu;
				pPduRes->SetPBMsg(&msgResp);
				pPduRes->SetSeqNum(pPdu->GetSeqNum());
				pPduRes->SetServiceId(IM::BaseDefine::SID_BUDDY_LIST);
				pPduRes->SetCommandId(IM::BaseDefine::CID_BUDDY_LIST_POSITION_RESPONSE);
				CProxyConn::AddResponsePdu(conn_uuid, pPduRes);
			}
		}
		else
		{
			log("parse pb failed");
		}
	}
}