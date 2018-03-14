#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <sys/shm.h>  
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "idevice_ctrl.h"
#include "ios_shm_mgr.h"
#include "ios_mgr.h"

static uint32_t parse_dev_list(string dev_list, vector<ios_dev_info_stru>* p_out_vec_ios_dev_info)
{
	if (dev_list.empty() || p_out_vec_ios_dev_info == NULL)
		return CRG_INVALID_PARAM;

	int ret = CRG_NOT_FOUND;
	uint32_t i = 0;
	ios_dev_info_stru ios_dev_info = { 0 };
	vector<string> parsed_dev_list;

	string_split(dev_list, string("\n"), &parsed_dev_list);

	if (parsed_dev_list.size() < 1)
		goto _PARSE_IOS_DEV_LIST_ERR_;

	if (parsed_dev_list[0].find(IOS_CONNECT_DEVICE_LIST_STR, 0) != string::npos)
	{
		parsed_dev_list.erase(parsed_dev_list.begin());
		vector<string>(parsed_dev_list).swap(parsed_dev_list);
	}

	if (parsed_dev_list.size() == 0)
		goto _PARSE_IOS_DEV_LIST_ERR_;

	do
	{
		if (parsed_dev_list[i].length() == 0)
			continue;

		memset((void*)&ios_dev_info, 0, sizeof(ios_dev_info_stru));
		strcpy(ios_dev_info.sz_unique_dev_id, parsed_dev_list[i].c_str());
		// IOSGetDeviceInformation(parsed_dev_list[i].c_str(), &ios_dev_info);

		p_out_vec_ios_dev_info->push_back(ios_dev_info);

	} while (++i && i < parsed_dev_list.size());
	
	ret = p_out_vec_ios_dev_info->size() == 0 ? CRG_NOT_FOUND : CRG_SUCCESS;

_PARSE_IOS_DEV_LIST_ERR_:

	parsed_dev_list.clear();
	vector<string>(parsed_dev_list).swap(parsed_dev_list);

	return ret;
}

uint32_t enum_current_ios_list(vector<ios_dev_info_stru>* p_out_vec_ios_dev_info)
{
	if (p_out_vec_ios_dev_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	string dev_list;
	ret = idevice_enum_dev_list(dev_list);
	if (ret == CRG_SUCCESS)
	{
		ret = parse_dev_list(dev_list, p_out_vec_ios_dev_info);
	}

	return ret;
}

void clear_ios_dev_data(vector<ios_dev_info_stru>* p_vec_ios_dev_info)
{
	while (p_vec_ios_dev_info->size() > 0)
	{
		memset((void*)&((*p_vec_ios_dev_info)[0]), 0, sizeof(ios_dev_info_stru));
		p_vec_ios_dev_info->erase(p_vec_ios_dev_info->begin());
	}

/*	for (vector<ios_dev_info_stru>::iterator i = p_vec_ios_dev_info->begin(); i != p_vec_ios_dev_info->end();)
	{
		p_vec_ios_dev_info->erase(i++);
	}
*/
	vector<ios_dev_info_stru>(*p_vec_ios_dev_info).swap(*p_vec_ios_dev_info);
}
