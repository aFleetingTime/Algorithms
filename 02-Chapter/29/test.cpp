#include "MyAlgorithms.hpp"
#include <iostream>

int main()
{
	int ary[10] {};
	std::cout << "count: " << d(ary, 3) << std::endl;
	std::cout << "count: " << d(ary, 5) << std::endl;
	std::cout << "count: " << d(ary, 6) << std::endl;
	std::cout << "count: " << d(ary, 8) << std::endl;
	std::cout << "count: " << d(ary, 9) << std::endl;
	std::cout << "count: " << d(ary, 10) << std::endl;
}
