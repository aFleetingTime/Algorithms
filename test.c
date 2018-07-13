#include <cstdlib>
#include <iostream>
#include <algorithm>

#define SIZE 1000000

int test(int *a, int n)
{
	std::pair<int, int> v(a[1], a[0]);
	if (v.first < v.second)
		std::swap(v.first, v.second);
	for (int i = 2; i < n; ++i)
		if (a[i] < v.second)
			v.first = std::exchange(v.second, a[i]);
	return v.first;
}

void swap(int *x, int *y)  
{  
    int temp = *x;  
    *x = *y;  
    *y = temp;  
}  
  
int Partition(int *A, int p, int r)  
{  
    int i, j, x;  
      
    i = p;  
    j = r + 1;  
  
    x = A[p];  
  
    while(1)  
    {  
        while(A[++i] < x);  
        while(A[--j] > x);  
  
        if(i >= j)  
            break;  
        swap(&A[i],&A[j]);  
    }  
    A[p] = A[j];  
    A[j] = x;  
      
    return j;  
}  
  
void QuickSort(int *A, int p, int r)  
{  
    int q;  
    if(p < r)  
    {  
        q = Partition(A,p,r);  
        QuickSort(A,p,q-1);  
        QuickSort(A,q+1,r);  
    }  
}  
  
void PrintResult(int *A, int size)  
{  
    int i;  
    for(i = 0; i < size; i++)  
    {  
        printf("%-2d",A[i]);  
    }  
    putchar('\n');  
  
}  
int hoare_partition(int *source, int head, int tail) {
    int x = source[head];
    int i = head - 1;
    int j = tail + 1;
    while (1) {
        do {
            j--;
        } while (source[j] > x);
        do {
            i++;
        } while (source[i] < x);
        if (i < j) {
            swap(source + i, source + j);
        } else return j;
    }
}

void hoare_quick_sort(int *source, int head, int tail) {
    if (head >= tail)
        return;
    int mid = hoare_partition(source, head, tail);
    hoare_quick_sort(source, head, mid);
    hoare_quick_sort(source, mid + 1, tail);
}

int main(void)  
{  
	int a[] = {10,9,8,7,6,5,4,3,2,1,5,11,5,84,87,0,33,55};
	std::cout << test(a, 18) << std::endl;
}  
