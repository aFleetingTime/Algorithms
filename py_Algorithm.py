# 插入排序
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

if __name__ == '__main__':
    lst = []
    insertSort(lst)
    print(lst)
