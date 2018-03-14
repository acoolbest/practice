#include <string>
#include <string.h>
#include <unistd.h>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "ios_shm_mgr.h"
#include "ios_mgr.h"

int main(int argc, char *argv[])
{
	uint32_t ret = CRG_FAIL;
	string json_str;
	/*
		argv[1] --- it's func. ("1" == enum device;)
		argv[2] --- it's cmd. 
	*/

	if (argc == 3 && argv[1] != NULL && 0 == strcmp(argv[1], "1") && 0 == strcmp(argv[2], "idevice_id -l"))
	{
		if (init_ios_list_share_mem_model() == CRG_SUCCESS)
		{
			write_ios_pid_data_to_share_mem(getpid());
			vector<ios_dev_info_stru> vec_ios_dev_info;
			clear_ios_dev_data(&vec_ios_dev_info);
			ret = enum_current_ios_list(&vec_ios_dev_info);
			if (ret == CRG_SUCCESS)
			{	
				ret = make_json_for_ios_list(&vec_ios_dev_info, json_str);
				if (ret == CRG_SUCCESS /* && t2 - t1 < RUN_TIME_OUT */)
				{
					ret = write_ios_list_data_to_share_mem(json_str.c_str(), json_str.length());
				}
			}
			else
			{
				ret = write_ios_list_data_to_share_mem(NULL, 0);
			}

			clear_ios_dev_data(&vec_ios_dev_info);
			destory_ios_list_share_mem_model();
		}
	}
	return 0;
}  
