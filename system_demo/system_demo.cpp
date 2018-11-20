#include <stdio.h>
#include <stdlib.h>
/*
函数说明
	system()会调用fork()产生子进程，由子进程来调用/bin/sh -c string来执行参数string字符串所代表的命令，此命令执行完后随即返回原调用的进程。
  
返回值
	=-1:	出现错误  
	=0:		调用成功但是没有出现子进程  
	>0:		成功退出的子进程的id
	如果system()在调用/bin/sh时失败则返回127，其他失败原因返回-1。
	若参数string为空指针(NULL)，则返回非零值。
	如果system()调用成功则最后会返回执行shell命令后的返回值，但是此返回值也有可能为 system()调用/bin/sh失败所返回的127，因此最好能再检查errno来确认执行成功

其他
	子进程的结束状态返回后存于status，底下有几个宏可判别结束情况  
	WIFEXITED(status)如果子进程正常结束则为非0值。  
	WEXITSTATUS(status)取得子进程exit()返回的结束代码，一般会先用WIFEXITED 来判断是否正常结束才能使用此宏。  
	WIFSIGNALED(status)如果子进程是因为信号而结束则此宏值为真 

	WTERMSIG(status)取得子进程因信号而中止的信号代码，一般会先用WIFSIGNALED 来判断后才使用此宏。  
	WIFSTOPPED(status)如果子进程处于暂停执行情况则此宏值为真。一般只有使用WUNTRACED 时才会有此情况。  
	WSTOPSIG(status)取得引发子进程暂停的信号代码，一般会先用WIFSTOPPED 来判断后才使用此宏。
*/

bool crg_system(const char * command)
{
	int status = system(command);

	if(status == -1)
		printf("system call error status[%d], command[%s]\n", status, command);
	else
		if(WIFEXITED(status))
		{  
			if(0 == WEXITSTATUS(status))  
				return true;
			else  
				printf("system call rum failed, WEXITSTATUS[%d] command[%s]\n", WEXITSTATUS(status), command);
		}  
		else
			printf("system call exit failed, WEXITSTATUS[%d], command[%s]\n", WEXITSTATUS(status), command);

    return false;
}

int main(int argc, char * argv[])
{
	bool ret = false;
	ret = crg_system("ls");
	ret = crg_system("find ./ -name \"ababababab\" &");
	return 0;
}
