#include <valarray>
#include <iostream>
#include <algorithm>

namespace columnSortBase {
template<typename value_type>
void subColumnSort(std::valarray<value_type> &ary, std::size_t r, std::size_t s)
{
	for (std::size_t i = 0; i < s; ++i)
	{
		auto column = ary[std::slice(i, r, s)];
		std::valarray<value_type> temp(column);
		std::sort(std::begin(temp), std::end(temp));
		column = temp;
	}
}

template<typename value_type>
void transpose(std::valarray<value_type> &ary, std::size_t r, std::size_t s)
{
	std::valarray<value_type> temp(ary.size());
	for (std::size_t i = 0, curRow = 0; i < s; ++i)
		for (std::size_t j = 0; j < r; j += s)
			temp[std::slice(curRow++ * s, s, 1)] = ary[std::slice(j * s + i, s, s)];
	ary = temp;
}

template<typename value_type>
void reverseTranspose(std::valarray<value_type> &ary, std::size_t r, std::size_t s)
{
	std::valarray<value_type> temp(ary.size());
	for (std::size_t i = 0, curRow = 0; i < s; ++i)
		for (std::size_t j = 0; j < r; j += s)
			temp[std::slice(j * s + i, s, s)] = ary[std::slice(curRow++ * s, s, 1)];
	ary = temp;
}

template<typename value_type>
std::valarray<value_type> matrixPermutation(std::valarray<value_type> &ary, std::size_t r, std::size_t s)
{
	std::valarray<value_type> temp(ary.size() + r);
	for (std::size_t i = 0; i < r / 2; ++i)
	{
		temp[std::slice((s + 1) * (r / 2 + i), s, 1)] = ary[std::slice(i * s, s, 1)];
		temp[std::slice((s + 1) * i + 1, s, 1)] = ary[std::slice((i + r / 2) * s, s, 1)];
	}
	return temp;
}

template<typename value_type>
std::valarray<value_type> reserveMatrixPermutation(std::valarray<value_type> &ary, std::size_t r, std::size_t s)
{
	std::valarray<value_type> temp(ary.size() - r);
	for (std::size_t i = 0; i < r / 2; ++i)
	{
		temp[std::slice(i * s, s, 1)] = ary[std::slice((s + 1) * (r / 2 + i), s, 1)];
		temp[std::slice((i + r / 2) * s, s, 1)] = ary[std::slice((s + 1) * i + 1, s, 1)];
	}
	return temp;
}
}	// namespace columnSortBase
