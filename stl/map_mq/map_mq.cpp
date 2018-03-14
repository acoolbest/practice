/* ************************************************************************
 *       Filename:  map_mq.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年04月24日 11时20分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <map>
#include <vector>
#include <stdint.h>
#include <string.h>
using namespace std;

struct activemq_command_stru
{
	char portcode[20];
	vector<string> vec_app_bundle_id;
};

static map<string, activemq_command_stru> 		g_map_mq_cmd;

uint32_t is_string_exist(map<string, activemq_command_stru> src_map, string find_key, activemq_command_stru & out_val)
{
	uint32_t ret = 0;
	map<string, activemq_command_stru>::iterator iter;
	iter = src_map.find(find_key);
	if (iter != src_map.end())
	{
		out_val = iter->second;
		ret = 1;
	}
	return ret;
}

uint32_t insert_string_map(map<string, activemq_command_stru>* out, string key, activemq_command_stru val)
{
	if (out == NULL)
		return 0;

	pair<map<string, activemq_command_stru>::iterator, bool> insert_pair;

	insert_pair = out->insert(pair<string, activemq_command_stru>(key, val));

	return insert_pair.second ? 1 : 0;
}



uint32_t update_power_on_data_to_map(activemq_command_stru stru_mq_cmd)
{
	g_map_mq_cmd[stru_mq_cmd.portcode] = stru_mq_cmd;
}

void print_map(map<string, activemq_command_stru> map_mq)
{
	cout << map_mq.size() << endl;
	for(map<string, activemq_command_stru>::iterator iter = map_mq.begin(); iter != map_mq.end(); iter++)
	{
		cout << iter->first << endl;
		cout << iter->second.portcode << endl;
		for(int i=0;i<iter->second.vec_app_bundle_id.size();i++)
			cout << iter->second.vec_app_bundle_id[i] << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	}
}
int main(int argc, char *argv[])
{
	string a = "com.aaa";
	string b = "com.bbb";
	string c = "com.ccc";
	char qrcode[3][20] = {"11111111", "22222222", "33333333"};
	activemq_command_stru stru_cmd = {0};
	strcpy(stru_cmd.portcode, qrcode[0]);
	stru_cmd.vec_app_bundle_id.push_back(a);
	stru_cmd.vec_app_bundle_id.push_back(b);
	update_power_on_data_to_map(stru_cmd);
	print_map(g_map_mq_cmd);
	
	
	cout << "%%%%%%%%%%%%%%%1%%%%%%%%%%%%%%%%%%" << endl;
	
	strcpy(stru_cmd.portcode, qrcode[1]);
	stru_cmd.vec_app_bundle_id.push_back(a);
	stru_cmd.vec_app_bundle_id.push_back(b);
	update_power_on_data_to_map(stru_cmd);
	print_map(g_map_mq_cmd);
	
	cout << "%%%%%%%%%%%%%%%2%%%%%%%%%%%%%%%%%%" << endl;
	
	stru_cmd.vec_app_bundle_id.clear();
	vector<string>(stru_cmd.vec_app_bundle_id).swap(stru_cmd.vec_app_bundle_id);
	update_power_on_data_to_map(stru_cmd);
	print_map(g_map_mq_cmd);
	
	cout << "%%%%%%%%%%%%%%3%%%%%%%%%%%%%%%%%%%" << endl;
	strcpy(stru_cmd.portcode, qrcode[0]);
	update_power_on_data_to_map(stru_cmd);
	print_map(g_map_mq_cmd);
	
	cout << "%%%%%%%%%%%%%4%%%%%%%%%%%%%%%%%%%%" << endl;
	g_map_mq_cmd.clear();
	map<string, activemq_command_stru>(g_map_mq_cmd).swap(g_map_mq_cmd);
	
	print_map(g_map_mq_cmd);
	
	cout << "%%%%%%%%%%%%%5%%%%%%%%%%%%%%%%%%%%" << endl;
	
	strcpy(stru_cmd.portcode, qrcode[2]);
	stru_cmd.vec_app_bundle_id.push_back(c);
	cout << "insert ret : " << insert_string_map(&g_map_mq_cmd, qrcode[2], stru_cmd) << endl;
	print_map(g_map_mq_cmd);
	
	cout << "%%%%%%%%%%%%%6%%%%%%%%%%%%%%%%%%%%" << endl;
	stru_cmd.vec_app_bundle_id.push_back(a);
	cout << "insert ret : " << insert_string_map(&g_map_mq_cmd, qrcode[2], stru_cmd) << endl;
	print_map(g_map_mq_cmd);
	cout << "%%%%%%%%%%%%%7%%%%%%%%%%%%%%%%%%%%" << endl;
	activemq_command_stru out_val = {0};
	if(is_string_exist(g_map_mq_cmd, qrcode[2], out_val))
		cout << out_val.portcode << endl;
	
	cout << "%%%%%%%%%%%%%8%%%%%%%%%%%%%%%%%%%%" << endl;
	activemq_command_stru stru_cmd1 = {0};
	stru_cmd1 = stru_cmd;
	cout << stru_cmd1.portcode << endl;
	for(int i=0;i<stru_cmd1.vec_app_bundle_id.size();i++)
			cout << stru_cmd1.vec_app_bundle_id[i] << endl;
	
	return 0;
}


