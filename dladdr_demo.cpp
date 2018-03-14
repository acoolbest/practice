#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#if 1 //add by zhzq @ 2017-1-12 11:51:28
#define			CRG_FAIL							0x0000
#define			CRG_SUCCESS							0x0001
#define			CRG_INVALID_PARAM					0x0002
#define			CRG_NOT_FOUND						0x0004
#define			CRG_NOT_ENOUGH_BUFF					0x0006
#define			MAX_PATH							260
#define			ANDROID_PATH						".android"
#define			ANDROID_ADB_INI						"adb_usb.ini"


static uint32_t get_exe_parent_path(char* path, size_t len)
{
	if (path == NULL)
		return CRG_INVALID_PARAM;
	
	char sz_module_path[MAX_PATH] = { 0 };

	readlink("/proc/self/exe", sz_module_path, MAX_PATH);

	if (strrchr(sz_module_path, '/') == NULL){
		return CRG_NOT_FOUND;
	}
	strrchr(sz_module_path, '/')[0] = '\0';
	
	if (strrchr(sz_module_path, '/') == NULL){
		return CRG_NOT_FOUND;
	}
	strrchr(sz_module_path, '/')[0] = '\0';
	
	memcpy(path, sz_module_path, len < MAX_PATH ? len : MAX_PATH);
	return CRG_SUCCESS;
}

static uint32_t get_path_of_adb_usb_ini(char* file_path, size_t len)
{
	int ret = get_exe_parent_path(file_path, len);
	if(ret != CRG_SUCCESS) return ret;
	
	if (sprintf(file_path, "%s/application/%s", file_path, ANDROID_ADB_INI) >= (signed)len) {
        return 1;
    }
	printf("6%s\n", file_path);
	return access(file_path, 4) == -1 ? 1 : 0;
}
#endif

int main()
{
	char buff[260];
	size_t len = 260;
	get_path_of_adb_usb_ini(buff, len);//charge by zhzq @ 2017-1-12 10:50:14
	printf("%s\n", buff);
}