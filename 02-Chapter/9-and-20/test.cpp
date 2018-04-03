#include "MyAlgorithms.hpp"
#include <array>
#include <iterator>
#include <iostream>

int main()
{
	std::array<int, 9> ary { 9,1,8,2,7,3,6,4,5 };
	std::array<int, 9> dest {};
	rank(ary.cbegin(), ary.cend(), dest.begin());
	std::copy(dest.cbegin(), dest.cend(), std::ostream_iterator<int>(std::cout, " "));
	rankSort(ary.begin(), ary.end(), dest.begin());
	std::copy(dest.cbegin(), dest.cend(), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;
}
