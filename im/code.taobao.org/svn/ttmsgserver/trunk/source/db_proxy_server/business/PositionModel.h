#ifndef __POSITIONMODEL_H__
#define __POSITIONMODEL_H__

#include "ImPduBase.h"
#include "IM.BaseDefine.pb.h"

class CPositionModel
{
	public:
		static CPositionModel* getInstance();
		~CPositionModel() {};
		void getCompanyId(uint32_t	nUserId, uint32_t &nCompanyId);
		//void getChangedPositionId(uint32_t unCompanyId, uint32_t& nLastTime, list<string>& lsChangedIds);
		void getPositions(uint32_t unCompanyId, uint32_t& nLastTime, list<IM::BaseDefine::PositionInfo>& lsPositions);
		//void GetPositions(uint32_t unUserId, uint32_t& nLastTime, list<IM::BaseDefine::PositionInfo>& lsPositions);
		void GetPositions(uint32_t unCompanyId, uint32_t& nLastTime, list<IM::BaseDefine::PositionInfo>& lsPositions);
		
	private:
		CPositionModel(){};
	private:
		static CPositionModel* m_pInstance;
};

#endif 
