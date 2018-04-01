#pragma once
#include <algorithm>
#include <iterator>

template <typename Iterator, typename T>
T accumulate(Iterator first, Iterator end, T initVal)
{
	while(first != end)
	{
		initVal += *first;
		++first;
	}
	return initVal;
}

template <typename Iterator, typename T, typename BinaryOperation>
T accumulate(Iterator first, Iterator end, T initVal, BinaryOperation op)
{
	while(first != end)
	{
		initVal = op(initVal, *first);
		++first;
	}
	return initVal;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator ccopy(InputIterator first, InputIterator end, OutputIterator dest)
{
	while(first != end)
	{
		*dest = *first;
		++first, ++dest;
	}
	return dest;
}

template <typename T>
void permutation(T first, T end, std::ostream &os)
{
	while(std::next_permutation(first, end));
	do {
		for(T beg = first; beg != end; ++beg)
			os << *beg;
		os << std::endl;
	} while(std::next_permutation(first, end));
}
