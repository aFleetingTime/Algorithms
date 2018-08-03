from time import *

def insertSort(lst):
    start = 1
    for i in range(len(lst) - 1):
        if lst[i] > lst[i + 1]:
            break
        start += 1
    for cur in range(start, len(lst)):
        key = lst[cur]
        prev = cur - 1
        while prev >= 0 and lst[prev] > key:
            lst[prev + 1] = lst[prev]
            prev -= 1
        lst[prev + 1] = key

def quicksort(items, p, r):
    if p < r:
        q = partition(items, p, r)
        quicksort(items, p, q-1)
        quicksort(items, q+1, r)

def partition(items, p, r):
    x = items[r]
    i = p-1
    count = 0
    for j in range(p, r):
        if items[j] == x:
            count += 1

        if items[j] <= x:
            i = i + 1
            items[i],items[j] = items[j],items[i]

    items[i+1],items[r] = items[r],items[i+1]
    return i+1-count/2

def quick_algorithm(a,b,c):
    a=a%c
    ans=1
    while b!=0:
        if b&1:
            ans=(ans*a)%c
        b>>=1
        a=(a*a)%c
    return ans

def LCS_Length(X,Y):#输出最长公共子序列的长度以及记录最优解的构造过程
    m=len(X)-1
    n=len(Y)-1
    a=[[0]*(n+1)]*(m+1)
    b=[[None]*(n+1)]*(m+1)
    c=numpy.array(a)#c用于保存c[i,j]的最长公共子序列的长度
    for i in range(1,m+1):
        for j in range(1,n+1):
            if X[i]==Y[j]:
                c[i,j]=c[i-1,j-1]+1
            elif c[i-1,j]>=c[i,j-1]:
                c[i,j]=c[i-1,j]
            else:
                c[i,j]=c[i,j-1]
    return c

def print_LCS(c,X,Y,i,j):#只利用c
    if i==0 or j==0:
        return
    elif X[i]==Y[j]:
        print_LCS(c,X,Y,i-1,j-1)
        print X[i]
    elif c[i-1,j]>=c[i,j-1]:
        print_LCS(c,X,Y,i-1,j)
    else:
        print_LCS(c,X,Y,i,j-1)

if __name__=="__main__":
    X=[1,0,0,1,0,1,0,1]
    Y=[0,1,0,1,1,0,1,1,0]
    A=[None]
    A.extend(X)
    B=[None]
    B.extend(Y)
c=LCS_Length(A,B)
print_LCS(c,A,B,8,9)


