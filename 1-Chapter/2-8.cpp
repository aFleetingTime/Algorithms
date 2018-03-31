#include <iostream>
#include <algorithm>
#include <iterator>
#include <numeric>

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

int main()
{
	int array1[18] { 76,15,48,4,68,484,31,98,47,76,11,36,21,88,17,93,55 };
	int array2[15] { };
	std::cout << "--------- value ---------" << std::endl;
	std::copy(std::cbegin(array1), std::cend(array1), std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n--------- count ---------" << std::endl;
	std::cout << count(std::cbegin(array1), std::cend(array1), 76) << std::endl;
	std::cout << std::count(std::cbegin(array1), std::cend(array1), 76) << std::endl;
	std::cout << "\n--------- fill ---------" << std::endl;
	fill(std::begin(array2), std::end(array2), 76);
	std::copy(std::cbegin(array2), std::cend(array2), std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n--------- inner_product ---------" << std::endl;
	std::cout << inner_product(std::cbegin(array1), std::cend(array1), std::cbegin(array2), 0) << std::endl;
	std::cout << std::inner_product(std::cbegin(array1), std::cend(array1), std::cbegin(array2), 0) << std::endl;
	std::cout << "\n--------- iota ---------" << std::endl;
	iota(std::begin(array2), std::end(array2), 1);
	std::copy(std::cbegin(array2), std::cend(array2), std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n--------- is_sorted ---------" << std::endl;
	std::cout << std::boolalpha << "array1: " << is_sorted(std::cbegin(array1), std::cend(array1))
			  << "  array2: " << is_sorted(std::cbegin(array2), std::cend(array2)) << std::endl;
	std::cout << "std::is_sorted: " << "array1: " << std::is_sorted(std::cbegin(array1), std::cend(array1))
			  << "  array2: " << std::is_sorted(std::cbegin(array2), std::cend(array2)) << std::endl;
	std::cout << "\n--------- mismatch ---------" << std::endl;
	auto [first, second] = mismatch(std::cbegin(array1), std::cend(array1), std::cbegin(array2));
	std::cout << "first: " << *first << "  second: " << *second << std::endl;
}
