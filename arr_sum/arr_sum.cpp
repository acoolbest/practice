/**
������Ԫ��Ϊn, ��ô�����еĳ��ȣ� ��>=1, ��<=n.
������Ԫ��Ϊm, ��ô�����еĺͿ϶�>=m��
���m��ֵ>0, �����е�ͷ��β��ֵ��>0��
���m��ֵ<=0��������о���Ԫ��m����
**/
#include <iostream>
using namespace std;
int main()
{
	int arr[10]={1,5,-7,3,2,-7,4,-1,-1,2};
	int i=0,index=0,index_max=0,len=0,len_max=0;
	int sum=0,sum_max=0;
	int max_arr = arr[0], max_arr_index = 0;;
	for(i=0; i<10; i++)
	{
		if(max_arr<arr[i]) // ��ȡ������ֵ�е����ֵ
		{
			max_arr = arr[i];
			max_arr_index = i;
		}
		
		sum+=arr[i];
		if(sum<0)
		{
			index = i+1;
			sum = 0;
			len = 0;
		}
		else
		{
			len++;
		}
		if(sum_max<sum)
		{
			sum_max = sum;
			index_max = index;
			len_max = len;
		}
	}
	if(max_arr <= 0)
	{
		sum_max = max_arr;
		index_max = max_arr_index;
		len_max = 1;
	}
	//cout<<sum_max<<endl<<index_max<<endl<<len_max<<endl;
	cout << "sum_max = " << sum_max << endl;
	for(i=index_max;i<len_max;i++)
	{
		cout << arr[i] << " ";
	}
	cout << endl;
	return 0;
}