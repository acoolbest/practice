/************************************************************************
 *       Filename:  vector_demo.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年05月11日 15时08分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/

/**
{"id":"3118273","time":"0","portCode":"12449863","type":"2","dataSource":"1"}
{"id":"3118343","time":"0","portCode":"12449863","type":"1","dataSource":"1","deviceid":"YTDIBISGSK4HDMZP","timeout":120}
**/
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

typedef struct mq{
	int a;
	vector<string> vec_id;
}MQ_TEST;

int main(int argc, char *argv[])
{
	vector<int> v( 5, 1 ), v1(3, 2), v2; 
	for( int i = 0; i < 5; i++ ) 
	{
		cout << "Element " << i << " is " << v.at(i) << endl;
	}
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	for( int i = 0; i < 3; i++ ) 
	{
		cout << "Element " << i << " is " << v1.at(i) << endl;
	}
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	cout << "v2.size(): " << v2.size() << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	vector<int>(v1).swap(v2);
	vector<int>(v).swap(v1);
	
	for( int i = 0; i < 5; i++ ) 
	{
		cout << "Element " << i << " is " << v.at(i) << endl;
	}
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	for( int i = 0; i < 5; i++ ) 
	{
		cout << "Element " << i << " is " << v1.at(i) << endl;
	}
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	for( int i = 0; i < 3; i++ ) 
	{
		cout << "Element " << i << " is " << v2.at(i) << endl;
	}
	
#if 0
	MQ_TEST m;
	
	printf("3333, %ld, %ld, %ld, %ld\n",sizeof(int), sizeof(m.vec_id), m.vec_id.size(), sizeof(m));
	
	memset(&m, 0, sizeof(m));
	
	printf("3333, %ld, %ld, %ld, %ld\n",sizeof(int), sizeof(m.vec_id), m.vec_id.size(), sizeof(m));
	
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("456");
	m.vec_id.push_back("456");
	m.vec_id.push_back("456");
	m.vec_id.push_back("456");
	m.vec_id.push_back("456");
	m.vec_id.push_back("12121212456");
	m.vec_id.push_back("456");
	m.vec_id.push_back("1212456");
	m.vec_id.push_back("456");
	m.vec_id.push_back("451212121212126");
	m.vec_id.push_back("41212121212122121212156");
	m.vec_id.push_back("456");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("1212212112121212");
	m.vec_id.push_back("456");
	m.vec_id.push_back("456");
	m.vec_id.push_back("456");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("456");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("456");
	m.vec_id.push_back("123456789001111111111111111111");
	m.vec_id.push_back("456");
	
	printf("3333, %ld, %ld, %ld, %ld\n",sizeof(int), sizeof(m.vec_id), m.vec_id.size(), sizeof(m));
	int len = m.vec_id.size();
	for(int i=0;i<len;i++)
		cout << " 1 " << m.vec_id[i] << endl;
	memset(&m, 0, sizeof(m));
	
	printf("4444, %ld, %ld, %ld, %ld\n",sizeof(int), sizeof(m.vec_id), m.vec_id.size(), sizeof(m));
	return 0;
#endif
#if 0
	int i = 0;
	while(1)
	{
		mq *str_mq = (mq *)malloc(sizeof(mq));

		if(str_mq == NULL)
		{
			cout << "malloc failed!" << endl;
			return -1;
		}
		cout << i << "  1111111111" << endl;

		str_mq->vec_id.clear();
		getchar();
		cout << i << "  1111111111" << endl;
		vector<string>(str_mq->vec_id).swap(str_mq->vec_id);
		getchar();
		cout << i << "  1111111111" << endl;
		free(str_mq);
		getchar();
		cout << i << "  1111111111" << endl;
		str_mq = NULL;
		i++;
	}
	return 0;
#endif
}


