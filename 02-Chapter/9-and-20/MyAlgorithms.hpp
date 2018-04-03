#pragma once
#include <algorithm>
#include <iostream>

template <typename T, typename OutputIterator>
void rank(T first, T end, OutputIterator dest)
{
	std::fill_n(dest, end - first, 0);
	for(T beg = first + 1; beg < end; ++beg)
	{
		for(T ite = first; ite != beg; ++ite)
			if(*ite <= *beg) ++*(dest + (beg - first));
			else ++*(dest + (ite - first));
	}
}

template <typename T, typename OutputIterator>
void rankSort(T first, T end, OutputIterator dest)
{
	static std::size_t count = 0;
	count = 0;
	for(T beg = first; beg != end; ++beg)
	{
		while(beg - first != dest[beg - first])
		{
			using std::swap;
			swap(*beg, first[dest[beg - first]]);
			swap(dest[beg - first], dest[dest[beg - first]]);
			++count;
		}
	}
	std::cout << "count: " << count << std::endl;
}
