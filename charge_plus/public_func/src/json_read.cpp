#include <fstream>
#include <sys/types.h> 
#include <sys/stat.h> 
#include "def.h"
#include "public_function.h"
#include "json_read.h"

int read_config(user_config& cnfg)
{
	int ret = CRG_FAIL;
	char sz_config_path[MAX_PATH] = { 0 };
	ret = get_path_of_userinfo(sz_config_path, MAX_PATH);
	if (ret != CRG_FOUND)
	{
		write_log("file is not exist: %s file%s%s", USERINFO_NAME_JSON, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		return ret;
	}
	string content = "";
	read_file(sz_config_path, content);
	bool valid = libjson::is_valid(content);
	if(valid){
		JSONNode n = libjson::parse(content);
		JSONNode::const_iterator i = n.begin();
		string str;
		while(i != n.end()){  
			std::string node_name = i -> name();
			if(node_name=="appsVersion"){
				cnfg.appsVersion = i->as_string();
			} 
			else if(node_name=="driverVersion"){
				cnfg.driverVersion = i->as_string();
			} 
			else if(node_name=="webVersion"){
				cnfg.webVersion = i->as_string();
			} 
			else if(node_name=="installVersion"){
				cnfg.installVersion = i->as_string();
			} 
			else if(node_name=="province"){
				cnfg.province = i->as_string();
			}
			else if(node_name=="terminal"){
				cnfg.terminal = i->as_string();
			}
			else if(node_name=="activeDriverPath"){
				cnfg.driver_path = i->as_string();
			} 
			else if(node_name=="activeWebPath"){
				cnfg.webPath = i->as_string();
			} 
			else if(node_name=="activeAppsPath"){
				cnfg.appPath = i->as_string();
			} 
			else if(node_name=="activeRunPath"){
				cnfg.cmdPath = i->as_string();
			}
			i++;
		}
		ret =CRG_SUCCESS;
	}else{
		write_log("load json file is failure, please check json format, file: %s\n", sz_config_path);
		return CRG_FAIL;
	}
	memset(sz_config_path, 0, sizeof(sz_config_path));
	ret = get_path_of_config_json(sz_config_path, MAX_PATH);
	if (ret != CRG_FOUND)
	{
		write_log("file is not exist: %s file%s%s", CONFIG_NAME_JSON, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		return ret;
	}
	content = "";
	read_file(sz_config_path, content);
	valid = libjson::is_valid(content);
	if(valid){
		JSONNode n = libjson::parse(content);
		JSONNode::const_iterator i = n.begin();
		string str;
		while(i != n.end()){  
			std::string node_name = i -> name();
			if(node_name=="requestTimeInterva"){
				cnfg.time = atoi(i->as_string().c_str());
			} 
			else if(node_name=="verhttpRequestAdress"){
				cnfg.ver_url = i->as_string();
			} 
			else if(node_name=="loghttpRequestAdress"){
				cnfg.log_url = i->as_string();
			} 
			else if(node_name=="sucesshttpRequestAdress"){
				cnfg.success_url = i->as_string();
			} 
			else if(node_name=="failhttpRequestAdress"){
				cnfg.fail_url = i->as_string();
			} 
			else if(node_name=="hearthttpRequestAdress"){
				cnfg.heart_url = i->as_string();
			} 
			else if(node_name=="userName"){
				cnfg.account = i->as_string();
			} 
			else if(node_name=="passWord"){
				cnfg.password = i->as_string();
			}
			else if(node_name=="tryDownloadCount"){
				cnfg.try_download_count = atoi(i->as_string().c_str());
			}
			else if(node_name=="releaseVersion"){
				cnfg.release_version = i->as_string();
			}
			else if(node_name=="releaseDate"){
				cnfg.release_date = i->as_string();
			}
			i++;
		}
		if (cnfg.try_download_count > 50 || cnfg.try_download_count < 1)
		{
			write_log("cnfg.try_download_count == %d is wrong£¬default is 6\n", cnfg.try_download_count);
			cnfg.try_download_count = 6;
		}
		ret =CRG_SUCCESS;
	}else{
		write_log("load json file is failure, please check json format, file: %s\n", sz_config_path);
		return CRG_FAIL;
	}
	return ret;
}

int read_net_traffic(net_traffic_stru & cnfg)
{
	int ret = CRG_FAIL;
	char sz_config_path[MAX_PATH] = { 0 };
	ret = get_path_of_net_traffic(sz_config_path, MAX_PATH);
	if (ret != CRG_FOUND)
	{
		write_log("file is not exist: %s file%s%s", NET_TRAFFIC_JSON, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		return ret;
	}
	string content = "";
	read_file(sz_config_path, content);
	bool valid = libjson::is_valid(content);
	if(valid){
		JSONNode n = libjson::parse(content);
		JSONNode::const_iterator i = n.begin();
		string str;
		while(i != n.end()){  
			std::string node_name = i -> name();
			if(node_name=="netTrafficNew"){
				cnfg.net_new = i->as_string();
			} 
			else if(node_name=="netTrafficOld"){
				cnfg.net_old = i->as_string();
			}
			i++;
		}
		ret = CRG_SUCCESS;
	}else{
		write_log("load json file is failure, please check json format, file: %s\n", sz_config_path);
		ret = CRG_FAIL;
	}
	return ret;
}

int write_net_traffic(net_traffic_stru cnfg)
{	
	int ret = CRG_FAIL;
	char sz_config_path[MAX_PATH] = { 0 };
	ret = get_path_of_net_traffic(sz_config_path, MAX_PATH);
	if (ret != CRG_FOUND)
	{
		write_log("file is not exist: %s file%s%s", NET_TRAFFIC_JSON, PRINT_POINT_STR, PRINT_ERR_FILE_STR);
		return ret;
	}
	string content = "";
	read_file(sz_config_path, content);
	bool valid = libjson::is_valid(content);
	if(valid){
		JSONNode n = libjson::parse(content);
		JSONNode::iterator i = n.begin(); 
		string str;
		while(i != n.end()){  
			std::string node_name = i -> name();
			if(node_name=="netTrafficNew"){
				JSONNode  an = JSONNode(node_name,cnfg.net_new);
				i->swap(an);
			} 
			else if(node_name=="netTrafficOld"){
				JSONNode  an = JSONNode(node_name,cnfg.net_old);
				i->swap(an);
			}
			i++;
		}
		string js = n.write_formatted();
		write_file(sz_config_path,js); 
		ret = CRG_SUCCESS;
	}else{
		write_log("load json file is failure, please check json format, file: %s\n", sz_config_path);
	}
	return ret;
}


