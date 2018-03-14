/* ************************************************************************
 *       Filename:  mutating_algorithms.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2017年03月28日 15时38分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (), 
 *        Company:  
 * ************************************************************************/


#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;


int main(int argc, char *argv[])
{
	int arr[6] = {1,12,3,2,1215,90};
	int arr1[7];
	int arr2[6] = {2,5,6,9,0,-56};

	copy(arr, (arr+6), arr1);
	
	for(int i=0;i<7;i++)
		cout << arr1[i] << " ";
	cout << endl;

	reverse(arr, arr+6);
	
	for(int i=0;i<7;i++)
		cout << arr[i] << " ";
	cout << endl;

	copy(arr,arr+6,ostream_iterator<int>(cout, " **"));
	
	for(int i=0;i<7;i++)
		cout << arr[i] << " ";
	cout << "--------------------------" << endl;


	swap_ranges(arr,arr+6,arr2);
	
	cout << endl;

	copy(arr, arr+6, ostream_iterator<int>(cout, " ##"));

	cout << endl;

	copy(arr2, arr2+6, ostream_iterator<int>(cout, " @@"));

	return 0;
}

