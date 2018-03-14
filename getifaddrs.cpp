#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <iostream>
#include <vector>


#define			MAX_BUFF_LEN						260
using std::vector;
using std::string;

struct network_info_stru{
	char interface_name[MAX_BUFF_LEN];
	char ip_address[INET_ADDRSTRLEN];
	char broadcast[INET_ADDRSTRLEN];
	char subnet_mask[INET_ADDRSTRLEN];
	char network_id[INET_ADDRSTRLEN];
};

int getSubnetMask(vector<network_info_stru> & out_vec_data)
{
	struct ifaddrs *ifa = NULL, *ifList;
	struct in_addr network_id = {0};
	unsigned long ulong_addr = 0;
	network_info_stru network_info = {0};
	
	if (getifaddrs(&ifList) < 0)
	{
		return -1;
	}
	for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)
	{
		if(ifa->ifa_addr->sa_family == AF_INET)
		{
			strcpy(network_info.interface_name, ifa->ifa_name);
			strcpy(network_info.ip_address, inet_ntoa(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr));
			strcpy(network_info.subnet_mask, inet_ntoa(((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr));
			strcpy(network_info.broadcast, inet_ntoa(((struct sockaddr_in *)ifa->ifa_broadaddr)->sin_addr));

			ulong_addr = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr & ((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr.s_addr;
			memcpy(&network_id, &ulong_addr, sizeof(ulong_addr));
			strcpy(network_info.network_id, inet_ntoa(network_id));
	
			out_vec_data.push_back(network_info);
			memset(&network_info, 0, sizeof(network_info));
		}
	}

	freeifaddrs(ifList);

	return 0;
}

int main(void)
{
	vector<network_info_stru> vec_net_data;
	getSubnetMask(vec_net_data);
	for(uint32_t i=0;i<vec_net_data.size();i++)
	{
		printf("\ninterfaceName\t%s\n", vec_net_data[i].interface_name);
		printf("ipAddress\t%s\n", vec_net_data[i].ip_address);
		printf("broadcast\t%s\n", vec_net_data[i].broadcast);
		printf("subnetMask\t%s\n", vec_net_data[i].subnet_mask);
		printf("networkID\t%s\n", vec_net_data[i].network_id);
	}
	vec_net_data.clear();
	vector<network_info_stru>(vec_net_data).swap(vec_net_data);
	return 0;
}
