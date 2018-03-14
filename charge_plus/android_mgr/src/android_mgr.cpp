#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <sys/shm.h>  
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "adb_ctrl.h"
#include "android_shm_mgr.h"
#include "android_mgr.h"

static uint32_t parse_dev_list(string dev_list, vector<android_dev_info_stru>* p_out_vec_android_dev_info)
{
	if (dev_list.empty() || p_out_vec_android_dev_info == NULL)
		return CRG_INVALID_PARAM;

	int ret = CRG_NOT_FOUND;
	uint32_t i = 0;
	string::size_type index = 0;
	android_dev_info_stru android_dev_info = { 0 };
	vector<string> parsed_dev_list, parsed_sub_dev_list;

	string_split(dev_list, string("\n"), &parsed_dev_list);

	if (parsed_dev_list.size() < 2)
		goto _PARSE_ANDROID_DEV_LIST_ERR_;

	while (parsed_dev_list.size() > 0)
	{
		string::size_type fret = parsed_dev_list[0].find(ANDROID_CONNECT_DEVICE_LIST_STR, 0);

		parsed_dev_list.erase(parsed_dev_list.begin());
		vector<string>(parsed_dev_list).swap(parsed_dev_list);

		if (fret != string::npos)
			break;
	}

	if (parsed_dev_list.size() == 0)
		goto _PARSE_ANDROID_DEV_LIST_ERR_;

	do
	{
		if (parsed_dev_list[i].length() == 0)
			continue;

		parsed_sub_dev_list.clear();
		vector<string>(parsed_sub_dev_list).swap(parsed_sub_dev_list);

		string_split(parsed_dev_list[i], string(" "), &parsed_sub_dev_list);
		if (parsed_sub_dev_list.size() < 3)
			continue;

		// "  " replace to " "
		while ((index = parsed_sub_dev_list[0].find(string("  "), 0)) != string::npos)
		{
			parsed_sub_dev_list[0].replace(index, 2, " ");
		}
		/*
		// get field
		vecParsedField.clear();
		vecParsedField.swap(vector<string>());
		string_split(vecParsedSubDevList[0], string(" "), &vecParsedField);
		if (vecParsedField.size() != 2)
		continue;
		*/
		memset((void*)&android_dev_info, 0, sizeof(android_dev_info_stru));
		memcpy(android_dev_info.sz_dev_id, parsed_sub_dev_list[0].c_str(), parsed_sub_dev_list[0].length());
		memcpy(android_dev_info.sz_state, parsed_sub_dev_list[1].c_str(), parsed_sub_dev_list[1].length());
		memcpy(android_dev_info.sz_path, parsed_sub_dev_list[2].c_str(), parsed_sub_dev_list[2].length());
		//GetDevicePidVid(&AndroidDevInfo);
		p_out_vec_android_dev_info->push_back(android_dev_info);
		/*
		if (!(string(AndroidDevInfo.szDeviceId)).empty()){
		if (InsertAndroidDeviceMap(pDeviceInfo, AndroidDevInfo) != CRG_SUCCESS)
		{
		write_log("Insert android Deviceinfo to map failed:%s\n", AndroidDevInfo.szDeviceId);
		}
		}*/
	} while (++i && i < parsed_dev_list.size());

	ret = p_out_vec_android_dev_info->size() == 0 ? CRG_NOT_FOUND : CRG_SUCCESS;

_PARSE_ANDROID_DEV_LIST_ERR_:

	parsed_dev_list.clear();
	vector<string>(parsed_dev_list).swap(parsed_dev_list);
	parsed_sub_dev_list.clear();
	vector<string>(parsed_sub_dev_list).swap(parsed_sub_dev_list);

	//	vecParsedField.clear();
	//	vecParsedField.swap(vector<string>());
	return ret;

}

uint32_t enum_current_android_list(vector<android_dev_info_stru>* p_out_vec_android_dev_info)
{
	if (p_out_vec_android_dev_info == NULL)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	string dev_list;
	ret = adb_enum_dev_list(dev_list);
	if (ret == CRG_SUCCESS)
	{
		ret = parse_dev_list(dev_list, p_out_vec_android_dev_info);
	}

	return ret;
}

void clear_android_dev_data(vector<android_dev_info_stru>* vec_android_dev_info)
{
	while (vec_android_dev_info->size() > 0)
	{
		memset((void*)&((*vec_android_dev_info)[0]), 0, sizeof(android_dev_info_stru));
		vec_android_dev_info->erase(vec_android_dev_info->begin());
	}
	vector<android_dev_info_stru>(*vec_android_dev_info).swap(*vec_android_dev_info);
}
