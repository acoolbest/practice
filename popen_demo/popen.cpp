#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

using namespace std;

uint32_t run_cmd(string cmd, string& out_str)
{
	if (cmd.length() == 0) {
		printf("cmd is null.\n");
		return 0;
	}
	uint32_t ret = 0;
	FILE * fp = NULL;
	char buf[260] = { 0 };

	cmd += " 2>&1";
	fp = popen(cmd.c_str(), "r");

	if (fp == NULL)
		return ret;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		out_str += buf;
		memset(buf, 0, sizeof(buf));
	}
	printf("RunCmdRecv:%s\n", out_str.c_str());

	ret = 1;

	pclose(fp);

	return ret;
}

uint32_t get_hwaddr(string & out_str)
{
	string cmd = "ifconfig | busybox awk '/^e/{print $5}' | head -n 1";
	return run_cmd(cmd, out_str);
}

int main()
{
	string out_str = "";
	get_hwaddr(out_str);
	cout << out_str << endl;
	return 0;
}