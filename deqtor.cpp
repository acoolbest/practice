/* ************************************************************************
 *       Filename:  deqtor.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年03月27日 17时41分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <deque>

using namespace std;
typedef deque<int> INTDEQUE;

void put_deque(INTDEQUE deque, char *name)
{
	INTDEQUE::iterator pdeque;
	cout << "The content of " << name << ":";
	for(pdeque = deque.begin(); pdeque != deque.end(); pdeque++)
		cout << *pdeque << " ";
	cout << endl;
}

int main(int argc, char *argv[])
{
	INTDEQUE deq1;
	INTDEQUE deq2(10,6);
	INTDEQUE::iterator i;
	put_deque(deq1, "deq1");
	put_deque(deq2, "deq2");
	
	deq1.push_back(2);
	deq1.push_back(4);
	cout << "deq1 push_back 2, 4" << endl;
	put_deque(deq1, "deq1");
	
	deq1.push_front(5);
	deq1.push_front(7);
	cout << "deq1 push front 5, 7" << endl;
	put_deque(deq1, "deq1");
	
	deq1.insert(deq1.begin()+1, 3, 9);
	cout << "deq1 insert begin()+1 3,9" << endl;
	put_deque(deq1, "deq1");
	
	cout << "deq1.at(4) = " << deq1.at(4) << endl;
	
	deq1.at(1) = 10;
	deq1[2] = 12;
	cout << "deq1.at(1) = 10; deq1[2] = 12" << endl;
	put_deque(deq1, "deq1");	

	return 0;
}


