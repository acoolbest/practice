/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：DepartModel.h
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月12日
*   描    述：
*
#pragma once
================================================================*/
#ifndef __DEPARTMODEL_H__
#define __DEPARTMODEL_H__

#include "ImPduBase.h"
#include "IM.BaseDefine.pb.h"

class CDepartModel {
public:
    static CDepartModel* getInstance();
    ~CDepartModel() {}
	void getCompanyId(uint32_t nUserId,uint32_t& nCompanyId);
    void getChgedDeptId(uint32_t nId,uint32_t& nLastTime, list<uint32_t>& lsChangedIds);
    void getDepts(list<uint32_t>& lsDeptIds, list<IM::BaseDefine::DepartInfo>& lsDepts);
    void getDept(uint32_t nDeptId, IM::BaseDefine::DepartInfo& cDept);
	void getCompanyId(string& strUserID, uint32_t& nCompanyId);
	void getChgedDeptId(string& strUserID, uint32_t& nLastTime, list<string>& lsChangedIds);
	void getChgedDeptId(uint32_t nId, uint32_t& nLastTime, list<string>& lsChangedIds);
	void getDepts(list<string>& lsDeptIds, list<IM::BaseDefine::DepartInfo>& lsDepts);
	void getDepts(uint32_t unCompanyId, uint32_t& nLastTime, list<IM::BaseDefine::DepartInfo>& lsDepts);
	void getDept(string& strDeptId, IM::BaseDefine::DepartInfo& cDept);
	void GetDepartments(uint32_t unCompanyId, uint32_t& nLastTime, list<IM::BaseDefine::DepartInfo>& lsDepts);
	//void GetDepartments(uint32_t unCompanyId, uint32_t& nLastTime, list<IM::BaseDefine::DepartInfo>& lsDepts)
private:
    CDepartModel(){};
private:
    static CDepartModel* m_pInstance;
};

#endif /*defined(__DEPARTMODEL_H__) */
