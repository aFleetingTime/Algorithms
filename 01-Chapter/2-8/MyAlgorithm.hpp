#pragma once
#include <algorithm>

template <typename T>
std::size_t count(const T *beg, const T *end, const T &val)
{
	std::size_t cnt = 0;
	while(beg != end)
		if(*beg++ == val)
			++cnt;
	return cnt;
}

template <typename T>
void fill(T *beg, T *end, const T &val) {
	while(&(*beg++ = val) != end);
}

template <typename T>
T inner_product(const T *beg, const T *end, const T *beg2, T val)
{
	while(beg != end)
	{
		val = val + *beg * *beg2;
		++beg, ++beg2;
	}
	return val;
}

template <typename T>
bool is_sorted(const T *beg, const T *end)
{
	while(beg != end - 1)
	{
		if(!(*beg < *(beg + 1)) && *(beg + 1) < *beg)
			return false;
		++beg;
	}
	return true;
}

template <typename T>
void iota(T *beg, T *end, T val)
{
	while(beg != end)
		*beg++ = val++;
}

template <typename T>
std::pair<const T*, const T*> mismatch(const T *beg, const T *end, const T *beg2)
{
	while(beg != end)
	{
		if(*beg != *beg2)
			break;
		++beg, ++beg2;
	}
	return { beg, beg2 };
}
