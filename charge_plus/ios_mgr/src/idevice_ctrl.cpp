#include <string.h>
#include <vector>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/select.h>  
#include <iostream>
#include <errno.h>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "idevice_ctrl.h"

using std::string;
#define IOS_REMOTE_NAME										"ios_control"
#define IOS_REMOTE_CONTROL_ENUM_DEVICE_CMD					"%s 1 \"idevice_id -l\""

#define IOS_CONTROL_START_SERVER_CMD						"usbmuxd -v"
#define IOS_CONTROL_KILL_SERVER_CMD							"kill -9 $(pgrep usbmuxd)"
#define IOS_CONTROL_REQ_RUNNING_CMD							"ps -e | grep usbmuxd"
#define IOS_CONTROL_ENUM_DEVICE_CMD	                        "idevice_id -l"
#define IOS_CONTROL_GET_STATE_CMD							"idevicepair validate -u %s"
#define IOS_CONTROL_GET_STATE_REQ_CMD						"idevicepair pair -u %s"
#define IOS_CONTROL_IDEVICEINFO_CMD                      	"ideviceinfo -u %s"

// #define ADB_CONNECT_DEVICE_LIST_STR						"连接的设备列表"
#define IOS_PAIRED_SUCCESS_STR								"SUCCESS: Validated pairing with device %s"
#define IOS_PAIRED_SUCCESS_STR_1							"SUCCESS: Paired with device %s"
#define IOS_PAIRED_FAILED_PASSCODE_STR						"ERROR: Could not validate with device %s because a passcode is set. Please enter the passcode on the device and retry.\n"
#define IOS_PAIRED_FAILED_NOT_PAIRED_STR					"ERROR: Device %s is not paired with this host\n"

static uint32_t run_cmd_to_remote(string cmd, FILE* &out_fp)
{
	cmd += " 2>&1";
	out_fp = popen(cmd.c_str(), "r");

	return out_fp == NULL ? CRG_FAIL : CRG_SUCCESS;
}

static uint32_t idevice_kill()
{
	idevice_close();
	return CRG_SUCCESS;
}

uint32_t idevice_start()
{
	uint32_t ret = CRG_FAIL;
	string out_str;
	ret = run_cmd(IOS_CONTROL_KILL_SERVER_CMD, out_str);
	if (ret == CRG_SUCCESS)
	{
		out_str.clear();
		ret = run_cmd(IOS_CONTROL_START_SERVER_CMD, out_str);
	}
	return ret;
}

uint32_t idevice_close()
{
	uint32_t ret = CRG_FAIL;
	string str;
	run_cmd(IOS_CONTROL_KILL_SERVER_CMD, str);
	
	uint32_t wait_count = 50;
	while (wait_count > 0)
	{
		str.clear();
		run_cmd(IOS_CONTROL_REQ_RUNNING_CMD, str);
		if (str.empty())
		{
			ret = CRG_SUCCESS;
			break;
		}
		usleep(100 * 1000); // 100ms
		wait_count--;
	}

	return ret;
}

uint32_t idevice_enum_dev_list(string& out_str)
{
	return run_cmd(IOS_CONTROL_ENUM_DEVICE_CMD, out_str);
}

uint32_t idevice_get_state(string dev_id, string & out_str)
{
	if (dev_id.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char command[MAX_LEN] = { 0 };
	uint32_t cmdlen = MAX_LEN;
	string str;
	char sz_buf[MAX_BUFF_LEN] = { 0 };

	snprintf(command, cmdlen, IOS_CONTROL_GET_STATE_CMD, dev_id.c_str());
	ret = run_cmd(command, str);
	if (ret == CRG_SUCCESS)
	{
		snprintf(sz_buf, sizeof(sz_buf), IOS_PAIRED_SUCCESS_STR, dev_id.c_str());
		if (str.find(sz_buf, 0) == string::npos)
		{
			cmdlen = MAX_LEN;
			str.clear();
			memset(command, 0, sizeof(command));
			snprintf(command, cmdlen, IOS_CONTROL_GET_STATE_REQ_CMD, dev_id.c_str());

			ret = run_cmd(command, str);
			if (ret == CRG_SUCCESS)
			{
				memset(sz_buf, 0, sizeof(sz_buf));
				snprintf(sz_buf, sizeof(sz_buf), IOS_PAIRED_SUCCESS_STR_1, dev_id.c_str());
				out_str = str.find(sz_buf, 0) != string::npos ? IOS_DEVICE_STATUS : IOS_UNAUTHORIZED_STATUS;
			}
		}
		else
			out_str = IOS_DEVICE_STATUS;
	}
	
	return ret;
}

uint32_t idevice_shell_get_prop(string dev_id, string & out_str)
{
	if (dev_id.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char command[MAX_LEN] = { 0 };
	uint32_t cmdlen = MAX_LEN;

	snprintf(command, cmdlen, IOS_CONTROL_IDEVICEINFO_CMD, dev_id.c_str());
	ret = run_cmd(command, out_str);
	
	return ret;
}

/**********************************************************************************************/

uint32_t idevice_enum_dev_list_from_ios_remote(FILE* &out_fp)
{
	char command[MAX_LEN] = { 0 };
	char remote_file[MAX_PATH] = { 0 };

	get_current_path(remote_file, MAX_PATH);
	snprintf(remote_file + strlen(remote_file), sizeof(remote_file) - strlen(remote_file), "/%s", IOS_REMOTE_NAME);

	if (is_exist_file(remote_file) == CRG_NOT_FOUND)
		return CRG_NOT_FOUND;

	snprintf(command, sizeof(command), "chmod 777 %s", remote_file);
	system(command);

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), IOS_REMOTE_CONTROL_ENUM_DEVICE_CMD, remote_file);

	return  run_cmd_to_remote(command, out_fp);
}

uint32_t kill_for_ios_remote(uint32_t remote_pid)
{
	char command[MAX_LEN] = { 0 };
	if(remote_pid > 0)
	{
		snprintf(command, sizeof(command), "kill -9 %d", remote_pid);
	}
	else
	{
		snprintf(command, sizeof(command), "kill -9 $(pgrep %s)", IOS_REMOTE_NAME);
	}
	idevice_kill();
	system(command);
	return CRG_SUCCESS;
}

uint32_t destory_for_ios_remote(FILE* &out_fp)
{
	if (out_fp == NULL)
		return CRG_INVALID_PARAM;

	pclose(out_fp);
	out_fp = NULL;

	return CRG_SUCCESS;
}
