/**
  g++ arp_test.cpp -o arp_test -Wall -lpcap
 **/

#include <pcap.h>
#include <libnet.h>

#include <arpa/inet.h>
#include <linux/if_ether.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ether.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <net/ethernet.h>
#include <netpacket/packet.h>	//sockaddr_ll sll

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;
#if 1
#define CAP_LEN 2048  
#define FILENAME "rh"  
#define _DEBUG_  1
int main()  
{  
	bpf_u_int32 localnet,netmask;  
	struct bpf_program fcode;  
	pcap_handler printer;  
	pcap_dumper_t*p;  
	char ebuf[PCAP_ERRBUF_SIZE];  
	char * device;  
	u_char * pcap_userdata;  
	pcap_t * pd;  
	int dev_flag=1;  
	int cap_len=CAP_LEN;  

	int i;  
	device=pcap_lookupdev(ebuf);  
	if (device == NULL)  
		exit(printf("%s n", ebuf));  
#ifdef _DEBUG_  
	printf("device is %s \n",device);  
#endif  
	pd=pcap_open_live(device,cap_len,dev_flag,1000,ebuf);  
	if(pd == NULL)  
		exit(printf("%s\n",ebuf));  
	i=pcap_snapshot(pd);  
	if(cap_len < i) {  
		printf("snaplen raised from %d to %d \n", cap_len, i);  
		cap_len=i;  
	}  
	if(pcap_lookupnet(device, &localnet, &netmask, ebuf) < 0) {  
		localnet=0;  
		netmask=0;  
		printf("%s\n", ebuf);  
	}  
	if (pcap_compile(pd, &fcode, "", 1, netmask) < 0)  
		exit(printf("Error %s\n","pcap_compile"));  
	if (pcap_setfilter(pd,&fcode) < 0)  
		exit(printf("Error %s\n","pcap_setfilter"));  
	p=pcap_dump_open(pd,FILENAME);  
	if(p == NULL)  
		exit(printf("Error:%s\n","pcap_dump_open"));  
	printer=pcap_dump;  
	pcap_userdata=(u_char *)p;  
	if(pcap_loop(pd, -1, printer, pcap_userdata) < 0)  
		exit(printf("Error, %s\n","pcap_loop"));  
	pcap_close(pd);  
	exit(0);  
}  
#else
//注意到接收的数据包头中代表类型,数据长度的字段采用的是big-endian
//所以对于2B/4B的数据要用ntohs()转换为本机形式
//ARP包结构
struct arppkt{
	unsigned short hdtyp;   //硬件类型.值0001
	unsigned short protyp;
	unsigned char hdsize;
	unsigned char prosize;
	unsigned short op;
	u_char smac[6];
	u_char sip[4];
	u_char dmac[6];
	u_char dip[4];
};
void packet_handler(const pcap_pkthdr *header,const u_char *pkt_data,ostream& out);
int main(int argc,char *argv[ ])
{
	if(argc!=2)
	{
		cout<<"Usage:arpparse logfilename"<<endl;
		cout<<"press any key to continue."<<endl;
		getchar();
		return 0;
	}
	pcap_if_t *alldevs;
	pcap_if_t *d;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32 netmask;
	char packet_filter[]="src foo";
	struct bpf_program fcode;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	if(pcap_findalldevs(&alldevs,errbuf)==-1)
	{
		cout<<"Error in pcap_findalldevs:"<<errbuf;
		return 0;
	}
	for(d=alldevs;d;d=d->next)
	{ 
		if((adhandle=pcap_open_live(d->name,1000,1,300,errbuf))==NULL)
		{
			cout<<"\nUnable to open the adapter.";
			pcap_freealldevs(alldevs);
			return 0;
		}
		if(pcap_datalink(adhandle)==DLT_EN10MB && d->addresses !=NULL)
			break;
	}
	if(d==NULL)
	{
		cout<<"\nNo interfaces found! Make sure Winpcap is installed.\n";
		return 0;
	}
	//获得子网掩码
	netmask=((sockaddr_in *) (d->addresses->netmask))->sin_addr.s_addr;
	//netmask=((sockaddr_in *)(d->netmask))->sin_addr.s_un.s_addr;s_addr
	//编译过滤器，只捕获ARP包
	if(pcap_compile(adhandle,&fcode,packet_filter,1,netmask)<0)
	{   cout<<"\nUnable to compile the packet filter.Check the syntax.\n";
		pcap_freealldevs(alldevs);
		return 0;
	}
	//设置过滤器
	if(pcap_setfilter(adhandle,&fcode)<0)
	{   cout<<"\nError setting the filter.\n";
		pcap_freealldevs(alldevs);
		return 0;
	}
	cout<<"\t\tlistening on "<<d->description<<"..."<<endl<<endl;
	//显示提示信息及每项含义
	ofstream fout(argv[1],ios::app);   //日志记录文件
	//为了查看日志时的方便，其中加入了日期记录
	time_t t;
	time(&t);
	fout.seekp(0,ios::end);
	if(fout.tellp()!=0)
		fout<<endl;
	fout<<"\t\tARP request(1)/reply(2) on"<<ctime(&t);
	cout << "Sour Ip Addr" << "  " << "Sour MAC Address" << "   " << "Des Ip Addr" << "   " << "Des MAC Address" << "   " << "OP" << "   " << "Time" <<endl;
	fout << "Sour Ip Addr" << "  " << "Sour MAC Address" << "   " << "Des Ip Addr" << "   " << "Des MAC Address" << "   " << "OP" << "   " << "Time" <<endl;
	//释放设备列表
	pcap_freealldevs(alldevs);
	int result;
	while((result=pcap_next_ex(adhandle,&header,&pkt_data))>=0)
	{   
		if(result==0)
			continue;
		packet_handler(header,pkt_data,cout);
		packet_handler(header,pkt_data,fout);
	}
}
void packet_handler(const pcap_pkthdr *header,const u_char *pkt_data,ostream& out)
{
	arppkt* arph = (arppkt *)(pkt_data +14);
	for(int i=0;i<3;i++)
		out<<int(arph->sip[i])<<'.';
	out.setf(ios::left);
	out<<setw(3)<<int(arph->sip[3])<<"  ";

	char oldfillchar = out.fill('0');

	out.setf(ios::uppercase);
	for(int i=0;i<5;i++)
		out<<hex<<setw(2)<<int(arph->smac[i])<<'-';
	out<<hex<<setw(2)<<int(arph->smac[5])<<"  ";
	out.fill(oldfillchar);
	out.unsetf(ios::hex|ios::uppercase);

	for(int i=0;i<3;i++)
		out<<int(arph->dip[3])<<'.';
	out.unsetf(ios::left);

	//输出目的MAC地址
	out.fill('0');
	out.setf(ios::uppercase);
	for(int i=0;i<5;i++)
		out<<hex<<setw(2)<<int(arph->dmac[i])<<'-';
	out.fill(oldfillchar);
	out.unsetf(ios::hex|ios::uppercase);
	out<<ntohs(arph->op)<<"   ";
	struct tm *ltime;
	ltime=localtime(&header->ts.tv_sec);
	out.fill('0');
	out<<ltime->tm_hour<<':'<<setw(2)<<ltime->tm_min<<':'<<setw(2)<<ltime->tm_sec;
	out.fill(oldfillchar);
	out<<endl;
}
#endif