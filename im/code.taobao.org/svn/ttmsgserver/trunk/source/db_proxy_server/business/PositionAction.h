#ifndef __POSITIONACTION_H__
#define __POSITIONACTION_H__
#include "ImPduBase.h"

#define MAX_POSITION_COUNT_EACH_TIME	30

namespace DB_PROXY
{

	void getChangedPosition(CImPdu* pPdu, uint32_t conn_uuid);
};


#endif /*defined(__DEPARTACTION_H__) */

