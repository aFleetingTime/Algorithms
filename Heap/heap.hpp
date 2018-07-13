#pragma once
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <iterator>

namespace heap {
template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void heapify(RandomIt first, RandomIt last, RandomIt current, Compare comp = {})
{
	RandomIt left, right, oldtemp;
	while (true)
	{
		oldtemp = current;
		left = first + ((current - first) * 2 + 1), right = left + 1;
		if (left < last && comp(*current, *left))
			current = left;
		if (right < last && comp(*current, *right))
			current = right;
		if (current == oldtemp)
			break;
		std::swap(*oldtemp, *current);
	}
}

template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void makeHeap(RandomIt first, RandomIt last, Compare comp = {})
{
	for (RandomIt beg = first + (last - first) / 2, fprev = std::prev(first); fprev < beg; --beg)
		heapify(first, last, beg, comp);
}

template<typename RandomIt, typename Compare = std::less_equal<typename std::iterator_traits<RandomIt>::value_type>>
bool isHeap(RandomIt first, RandomIt last, std::size_t size, Compare comp = {})
{
	RandomIt left(last - size + ((size - (last - first)) * 2 + 1)), right(left + 1);
	if (left < last && !comp(*left, *first))
		return false;
	if (right < last && !comp(*right, *first))
		return false;
	return ((left < last) ? isHeap(left, last, size, comp) : true) && ((right < last) ? isHeap(right, last, size, comp) : true);
}
template<typename RandomIt, typename Compare = std::less_equal<typename std::iterator_traits<RandomIt>::value_type>>
bool isHeap(RandomIt first, RandomIt last, Compare comp = {}) {
	return isHeap(first, last, last - first, comp);
}

template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void popHeap(RandomIt first, RandomIt last, Compare comp = {})
{
	std::iter_swap(first, std::prev(last));
	heapify(first, std::prev(last), first, comp);
}

template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void sortHeap(RandomIt first, RandomIt last, Compare comp = {})
{
	for (auto fnext = std::next(first); fnext < last; --last)
		popHeap(first, last, comp);
}
}
