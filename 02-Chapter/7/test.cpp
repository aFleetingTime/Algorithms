#include "MyAlgorithms.hpp"
#include <iostream>

int main()
{
	std::cout << factorial(-10) << std::endl;
	std::cout << factorial(0) << std::endl;
	std::cout << factorial(10) << std::endl;
	std::cout << factorial<unsigned long long>(50) << std::endl;
}
