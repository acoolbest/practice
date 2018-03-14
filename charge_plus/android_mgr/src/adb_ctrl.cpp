#include <string.h>
#include <vector>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/select.h>  
#include <iostream>
#include <errno.h>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "adb_ctrl.h"

using std::string;
#define ANDROID_REMOTE_NAME									"android_control"
#define ADB_CONTROL_START_SERVER_CMD						"adb start-server"
#define ADB_CONTROL_STOP_SERVER_CMD							"adb kill-server"
#define ADB_CONTROL_KILL_SERVER_CMD							"kill -9 $(pgrep adb)"
#define ADB_CONTROL_REQ_RUNNING_CMD							"ps -e | grep adb"
#define ADB_CONTROL_ENUM_DEVICE_CMD							"adb devices -l"
#define ADB_REMOTE_CONTROL_ENUM_DEVICE_CMD					"%s 1 \"adb devices -l\""
#define ADB_CONTROL_GET_STATE_CMD							"adb -s %s get-state"
#define ADB_CONTROL_SHELL_GET_PROP_CMD						"adb -s %s shell getprop"

#define ADB_CONTROL_PULL_DEVICE_ID_CMD						"adb -s %s pull /data/local/tmp/%s %s"
#define ADB_CONTROL_PUSH_DEVICE_ID_CMD						"adb -s %s push %s /data/local/tmp/"
#define ADB_CONTROL_SET_INSTALL_LOCATION_CMD				"adb -s %s shell pm set-install-location %d"
#define ADB_CONTROL_GET_INSTALL_LOCATION_CMD				"adb -s %s shell pm get-install-location"

#define ADB_CONTROL_INSTALL_APP_1_CMD						"adb -s %s install \"%s\""
#define ADB_CONTROL_INSTALL_APP_2_CMD						"adb -s %s install -s \"%s\""

#define ADB_ENUM_INSTALLED_APP_LIST_CMD						"adb -s %s shell pm list packages"

#define ADB_CONTROL_RUN_APP_CMD								"adb -s %s shell monkey -p \"%s\" 1"

// #define ADB_CONNECT_DEVICE_LIST_STR							"连接的设备列表"
#define ADB_SUCCESS_STR										"Success"
#define ADB_ERROR_STR										"Error:"
#define ADB_ERROR_UNKNOWN_HOST_SRV_STR						"error: unknown host service"
#define	ADB_INSTALL_FAILED_ALREADY_EXISTS_STR				"INSTALL_FAILED_ALREADY_EXISTS"							// 程序已经存在
#define	ADB_INSTALL_CANCELED_BY_USER_STR					"INSTALL_CANCELED_BY_USER"								// 用户取消
#define	ADB_INSTALL_FAILED_CANCELLED_BY_USER_STR			"INSTALL_FAILED_CANCELLED_BY_USER"						// 用户取消
#define ADB_INSTALL_FAILED_INSUFF_STORAGE_STR				"INSTALL_FAILED_INSUFFICIENT_STORAGE"					// 没有足够的存储空间
//
#define ADB_INSTALL_FAILED_INVALID_APK_STR					"INSTALL_FAILED_INVALID_APK"							// 无效的APK
#define ADB_INSTALL_FAILED_INVALID_URI_STR					"INSTALL_FAILED_INVALID_URI"							// 无效的链接
#define ADB_INSTALL_FAILED_DUPLICATE_PACKAGE_STR			"INSTALL_FAILED_DUPLICATE_PACKAGE"						// 已存在同名程序
#define ADB_INSTALL_FAILED_NO_SHARED_USER_STR				"INSTALL_FAILED_NO_SHARED_USER"							// 要求的共享用户不存在
#define ADB_INSTALL_FAILED_UPDATE_INCOMPATIBLE_STR			"INSTALL_FAILED_UPDATE_INCOMPATIBLE"					// 版本不能共存
#define ADB_INSTALL_FAILED_SHARED_USER_INCOMPATIBLE_STR		"INSTALL_FAILED_SHARED_USER_INCOMPATIBLE"				// 需求的共享用户不存在
#define ADB_INSTALL_FAILED_MISSING_SHARED_LIBRARY_STR		"INSTALL_FAILED_MISSING_SHARED_LIBRARY"					// 需求的共享库已丢失
#define ADB_INSTALL_FAILED_REPLACE_COULDNT_DELETE_STR		"INSTALL_FAILED_REPLACE_COULDNT_DELETE"					// 需求的共享库无效
#define ADB_INSTALL_FAILED_DEXOPT_STR						"INSTALL_FAILED_DEXOPT"									// dex优化验证失败
#define ADB_INSTALL_FAILED_OLDER_SDK_STR					"INSTALL_FAILED_OLDER_SDK"								// 系统版本过旧
#define ADB_INSTALL_FAILED_CONFLICTING_PROVIDER_STR			"INSTALL_FAILED_CONFLICTING_PROVIDER"					// 存在同名的内容提供者
#define ADB_INSTALL_FAILED_NEWER_SDK_STR					"INSTALL_FAILED_NEWER_SDK"								// 系统版本过新
#define ADB_INSTALL_FAILED_TEST_ONLY_STR					"INSTALL_FAILED_TEST_ONLY"								// 调用者不被允测试的测试程序
#define ADB_INSTALL_FAILED_CPU_ABI_INCOMPATIBLE_STR			"INSTALL_FAILED_CPU_ABI_INCOMPATIBLE"					// 包含的本机代码不兼容
#define ADB_INSTALL_FAILED_CPU_ABIINSTALL_FAILED_MISSING_FEATURE_STR		"CPU_ABIINSTALL_FAILED_MISSING_FEATURE"	// 使用了一个无效的特性
#define ADB_INSTALL_FAILED_CONTAINER_ERROR_STR					"INSTALL_FAILED_CONTAINER_ERROR"					// SD卡访问失败
#define ADB_INSTALL_FAILED_INVALID_INSTALL_LOCATION_STR			"INSTALL_FAILED_INVALID_INSTALL_LOCATION"			// 无效的安装路径
#define ADB_INSTALL_FAILED_MEDIA_UNAVAILABLE_STR			"INSTALL_FAILED_MEDIA_UNAVAILABLE"						// SD卡不存在
#define ADB_INSTALL_FAILED_INTERNAL_ERROR_STR				"INSTALL_FAILED_INTERNAL_ERROR"							// 系统问题导致安装失败
#define ADB_INSTALL_FAILED_VERIFICATION_TIMEOUT_BY_USER_STR			"INSTALL_FAILED_VERIFICATION_TIMEOUT"			// 安装确认超时
#define ADB_INSTALL_FAILED_VERIFICATION_FAILURE_BY_USER_STR			"INSTALL_FAILED_VERIFICATION_FAILURE"			// 不允许未签名的应用安装。应设置允许非市场的应用程序安装

//
#define ADB_INSTALL_FAILED_NOT_FOUND_PM_STR					"Package Manager"
#define ADB_INSTALL_FAILED_NO_SPACE_LEFT_STR				"No space left on device"

#define ADB_INSTALL_FAILED_DEVICE_NOT_FOUND_STR				"error: device not found"
#define ADB_INSTALL_FAILED_DEVICE_OFF_LINE_STR				"error: device offline"

#define ADB_RUN_FAILED_DEVICE_NOT_FOUND_STR					"No activities found to run"

#define ADB_PROCESS_NAME									"adb"

#define ADB_INSTALL_APP_TIMEOUT								(5*60*1000)				// 限定安装APP的时间为5分钟，否则超时

static uint32_t run_cmd_to_remote(string cmd, FILE* &out_fp)
{
	cmd += " 2>&1";
	out_fp = popen(cmd.c_str(), "r");

	return out_fp == NULL ? CRG_FAIL : CRG_SUCCESS;
}

uint32_t adb_enum_dev_list(string& out_str)
{
	uint32_t ret = CRG_FAIL;
	ret = run_cmd(ADB_CONTROL_ENUM_DEVICE_CMD, out_str);
	return ret;
}

uint32_t adb_start()
{
	uint32_t ret = CRG_FAIL;
	string str;
	run_cmd(ADB_CONTROL_STOP_SERVER_CMD, str);
	ret = run_cmd(ADB_CONTROL_START_SERVER_CMD, str);
	if (ret == CRG_SUCCESS)
	{
		ret = str.find(ADB_ERROR_STR, 0) == string::npos ? CRG_SUCCESS : CRG_FAIL;
		if (ret != CRG_FAIL && str.find(ADB_ERROR_UNKNOWN_HOST_SRV_STR, 0) != string::npos)
		{
			// port 5037 is used by other process. try to kill it

		}
		// ret = _tcsstr(buf, ADB_CONNECT_DEVICE_LIST_STR) == NULL ? FALSE : TRUE;
	}
	
	return ret;
}

uint32_t adb_close()
{
	uint32_t ret = CRG_FAIL;
	string str;
	ret = run_cmd(ADB_CONTROL_STOP_SERVER_CMD, str);
	
	if (ret == CRG_SUCCESS)
	{
		ret = str.find(ADB_ERROR_STR, 0) == std::string::npos ? CRG_SUCCESS : CRG_FAIL;
		if (ret != CRG_FAIL && str.find(ADB_ERROR_UNKNOWN_HOST_SRV_STR, 0) != std::string::npos)
		{
			// port 5037 is used by other process. try to kill it

		}
	}

	return ret;
}

static uint32_t adb_kill()
{
	uint32_t ret = CRG_FAIL;
	string str;
	run_cmd(ADB_CONTROL_KILL_SERVER_CMD, str);

	uint32_t wait_count = 50;
	while (wait_count > 0)
	{
		str.clear();
		run_cmd(ADB_CONTROL_REQ_RUNNING_CMD, str);
		if (str.empty())
		{
			ret = CRG_SUCCESS;
			break;
		}
		usleep(100*1000); // 100ms
		wait_count--;
	}
	
	return ret;
}

uint32_t adb_get_state(string dev_path, string & out_str)
{
	if (dev_path.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char command[MAX_LEN] = { 0 };
	uint32_t cmdlen = MAX_LEN;

	snprintf(command, cmdlen, ADB_CONTROL_GET_STATE_CMD, dev_path.c_str());
	ret = run_cmd(command, out_str);
	if (ret == CRG_SUCCESS)
	{
		ret = out_str.find(ADB_ERROR_STR, 0) == string::npos ? CRG_SUCCESS : CRG_FAIL;
	}
	
	return ret;
}

uint32_t adb_shell_get_prop(string dev_path, string & out_str)
{
	if (dev_path.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char command[MAX_LEN] = { 0 };
	uint32_t cmdlen = MAX_LEN;
	// uint32_t adb_pid = 0;

	snprintf(command, cmdlen, ADB_CONTROL_SHELL_GET_PROP_CMD, dev_path.c_str());
	ret = run_cmd(command, out_str);
	if (ret == CRG_SUCCESS)
	{
		ret = out_str.find(ADB_ERROR_STR, 0) == string::npos ? CRG_SUCCESS : CRG_FAIL;
	}
	return ret;
}

// 设置安装的位置，0：自动安装，1：手机内存， 2： 内存卡
uint32_t adb_set_install_location(string dev_path, uint32_t location)
{
	if (dev_path.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char command[MAX_LEN] = { 0 };
	uint32_t len = MAX_LEN;
	string out_str;

	snprintf(command, len, ADB_CONTROL_SET_INSTALL_LOCATION_CMD, dev_path.c_str(), location);
	ret = run_cmd(command, out_str);

	if (ret != CRG_SUCCESS || out_str.find(ADB_ERROR_STR, 0) != string::npos)
	{
		location = 1;
		len = MAX_LEN;
		out_str.clear();
		memset(command, 0, sizeof(command));
		snprintf(command, len, ADB_CONTROL_SET_INSTALL_LOCATION_CMD, dev_path.c_str(), location);
		
		ret = run_cmd(command, out_str);
		if (ret == CRG_SUCCESS)
		{
			ret = out_str.find(ADB_ERROR_STR, 0) == string::npos ? CRG_SUCCESS : CRG_FAIL;
		}
	}
	
	return ret;
}

uint32_t adb_enum_installed_app_list(string dev_path, string & out_str)
{
	if (dev_path.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char command[MAX_LEN] = { 0 };
	uint32_t len = MAX_LEN;
	
	snprintf(command, len, ADB_ENUM_INSTALLED_APP_LIST_CMD, dev_path.c_str());
	ret = run_cmd(command, out_str);

	if (ret == CRG_SUCCESS) // 
	{
		// Is need do it ?
		ret = out_str.find(ADB_ERROR_STR, 0) == string::npos ? CRG_SUCCESS : CRG_FAIL;
	}
	
	return ret;
}

uint32_t adb_install_one_app(string dev_path, string apk_path, string & out_err_str)
{
	if (dev_path.length() == 0 || apk_path.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char command[MAX_LEN] = { 0 };
	uint32_t len = MAX_LEN;
	string out_str;

	snprintf(command, len, ADB_CONTROL_INSTALL_APP_1_CMD, dev_path.c_str(), apk_path.c_str());
	if (run_cmd(command, out_str) != CRG_SUCCESS || out_str.find(ADB_INSTALL_FAILED_INSUFF_STORAGE_STR, 0) != string::npos)
	{
		out_str.clear();
		memset(command, 0, len);
		snprintf(command, len, ADB_CONTROL_INSTALL_APP_2_CMD, dev_path.c_str(), apk_path.c_str());
		run_cmd(command, out_str);
	}
	
	if (out_str.find(ADB_SUCCESS_STR, 0) != string::npos)
		ret = CRG_SUCCESS;
	else if (out_str.find(ADB_INSTALL_FAILED_ALREADY_EXISTS_STR, 0) != string::npos)
		ret = CRG_SUCCESS;
	else if (out_str.find(ADB_INSTALL_FAILED_NOT_FOUND_PM_STR, 0) != string::npos)
	{
		// 手机由关机到开机时会导致could not find Package Manager
		ret = CRG_NOT_START_SYSTEM;
	}
	else if (out_str.find(ADB_INSTALL_FAILED_NO_SPACE_LEFT_STR, 0) != string::npos)
		ret = CRG_NOT_ENOUGH_BUFF;
	else if (out_str.find(ADB_INSTALL_CANCELED_BY_USER_STR, 0) != string::npos)
		ret = CRG_CANCELED_BY_USER;
	else if (out_str.find(ADB_INSTALL_FAILED_CANCELLED_BY_USER_STR, 0) != string::npos)
		ret = CRG_CANCELED_BY_USER;
	else if (out_str.find(ADB_INSTALL_FAILED_INTERNAL_ERROR_STR, 0) != string::npos)
		ret = CRG_INTERNAL_ERROR_STR;
	else if (out_str.find(ADB_INSTALL_FAILED_VERIFICATION_TIMEOUT_BY_USER_STR, 0) != string::npos)
		ret = CRG_VERIFICATION_TIMEOUT_BY_USER;
	else if (out_str.find(ADB_INSTALL_FAILED_VERIFICATION_FAILURE_BY_USER_STR, 0) != string::npos)
		ret = CRG_VERIFICATION_FAILURE_BY_USER;
	else if (out_str.find(ADB_INSTALL_FAILED_DEVICE_NOT_FOUND_STR, 0) != string::npos)
		ret = CRG_DEVICE_NOT_FOUND;
	else if (out_str.find(ADB_INSTALL_FAILED_DEVICE_OFF_LINE_STR, 0) != string::npos)
		ret = CRG_DEVICE_OFF_LINE;
	else if (out_str.find(ADB_INSTALL_FAILED_INVALID_APK_STR, 0) != string::npos)
		ret = CRG_DEVICE_INVALID_APK;

	else
	{
		out_err_str = out_str;
		ret = CRG_FAIL;
	}

	return ret;
}

uint32_t adb_run_one_app(string dev_path, string apk_package)
{
	if (dev_path.length() == 0 || apk_package.length() == 0)
		return CRG_INVALID_PARAM;

	uint32_t ret = CRG_FAIL;
	char command[MAX_LEN] = { 0 };
	uint32_t len = MAX_LEN;
	string out_str;

	snprintf(command, len, ADB_CONTROL_RUN_APP_CMD, dev_path.c_str(), apk_package.c_str());
	
	ret = run_cmd(command, out_str);
	
	return (ret == CRG_SUCCESS && out_str.find(ADB_RUN_FAILED_DEVICE_NOT_FOUND_STR, 0) == string::npos) ? CRG_SUCCESS : CRG_FAIL;
}
/**********************************************************************************************/

uint32_t adb_enum_dev_list_from_android_remote(FILE* &out_fp)
{
	char command[MAX_LEN] = { 0 };
	char remote_file[MAX_PATH] = { 0 };

	get_current_path(remote_file, MAX_PATH);
	snprintf(remote_file + strlen(remote_file), sizeof(remote_file) - strlen(remote_file), "/%s", ANDROID_REMOTE_NAME);

	if (is_exist_file(remote_file) == CRG_NOT_FOUND)
		return CRG_NOT_FOUND;

	snprintf(command, sizeof(command), "chmod 777 %s", remote_file);
	system(command);

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), ADB_REMOTE_CONTROL_ENUM_DEVICE_CMD, remote_file);

	return  run_cmd_to_remote(command, out_fp);
}

uint32_t kill_for_android_remote(uint32_t remote_pid)
{
	char command[MAX_LEN] = { 0 };
	if(remote_pid > 0)
	{
		snprintf(command, sizeof(command), "kill -9 %d", remote_pid);
	}
	else
	{
		snprintf(command, sizeof(command), "kill -9 $(pgrep %s)", ANDROID_REMOTE_NAME);
	}
	adb_kill();
	system(command);
	return CRG_SUCCESS;
}

uint32_t destory_for_android_remote(FILE* &out_fp)
{
	if (out_fp == NULL)
		return CRG_INVALID_PARAM;

	pclose(out_fp);
	out_fp = NULL;

	return CRG_SUCCESS;
}
