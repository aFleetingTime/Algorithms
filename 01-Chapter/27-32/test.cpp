#include "MyAlgorithms.hpp"
#include <array>
#include <iostream>

int main()
{
	std::array<int, 10> ary { 2, 6, 9, 3, 5, 7, 6, 4, 8, 1 };
	std::cout << accumulate(ary.cbegin(), ary.cend(), 0) << " " << accumulate(ary.cbegin(), ary.cend(), 1, std::multiplies<int>()) << std::endl;
	{
		std::array<int, 10> a {};
		ccopy(ary.cbegin(), ary.cend(), a.begin());
		ccopy(a.cbegin(), a.cend(), std::ostream_iterator<int>(std::cout, " "));
		std::cout << std::endl;
	}
	permutation(ary.begin(), ary.begin() + 4, std::cout);
}
