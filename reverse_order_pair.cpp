/*
https://blog.csdn.net/liuxiao214/article/details/78526692
g++ reverse_order_pair.cpp -std=c++11
归并排序

时间复杂度：O(Nlog(N)) 
额外空间复杂度：O(N) 
是否可实现稳定性：是

二划分和合并的过程。

1、首先划分划分划分，一直划分到不能划分，即每个组都只有一个数值。

2、然后合并，合并的过程就是每个二划分排序的过程。

3、在合并的时候，开辟一个辅助数组，其大小等于这两个合并数列的大小。

4、设置两个指针分别指向每个数列的首部，然后比较得到其中较小的值，并将这个值放入辅助数组中。然后取出小值的那个数列的指针可以继续向前走，与另一个数列的指针所指向的值继续比较。

5、这样比较完成后，如果两个数列中有个数列的数值有剩余，即其指针没有走到末尾，则将这个数列直接赋到辅助数组末尾即可。

6、然后将辅助数组中的值拷贝回原数组中刚才合并的那两个数列的位置上。

eg:

3 7        5 9
合并：
设置辅助数组：
3  （5比3大，3放入数组中）
3 5  （7比5大，5放入数组中）
3 5 7   （9比7大，7放入数组中）
3 5 7 9  （数列1结束，直接将数列2放入辅助数组末尾，即9放入数组中）
完成一次合并排序。

下一次，3 5 7 9 可以和 1 4 6 8进行合并排序。

*/
#include <iostream>
#include <vector>

using namespace std;

class Merge
{
public:
    void MergeSort(vector<int>&a)
    {
        if (a.size() < 2)
            return;
        MergeSort(a, 0, a.size() - 1);
    }
    void MergeSort(vector<int>&a, int l, int r)
    {
        if (l == r)
            return;
        int mid = l + (r - l) / 2;
        MergeSort(a, l, mid);
        MergeSort(a, mid + 1, r);
        merge(a, l, mid, r);
    }
    void merge(vector<int>&a, int l, int mid, int r)
    {
        vector<int>help(r - l + 1);
        int i = 0;
        int p1 = l;
        int p2 = mid + 1;
        while (p1<=mid && p2<=r)
            help[i++] = a[p1] < a[p2] ? a[p1++] : a[p2++];
        while (p1 <= mid)
            help[i++] = a[p1++];
        while (p2 <= r)
            help[i++] = a[p2++];
        for (i = 0; i < help.size(); i++)
            a[l + i] = help[i];
    }
};

int main()
{
	Merge M;
	//vector<int> a = {3,7,5,9,1,4,6,8};
	vector<int> a = {9,3,7};
	M.MergeSort(a);
	for(auto &i:a) cout << i << endl;
	return 0;
}