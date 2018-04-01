#include "MyAlgorithms.hpp"
#include <iostream>

int main()
{
	std::cout << nonRecursiveFactorial(5) << " " 
			  << nonRecursiveFactorial(10) << " " 
			  << nonRecursiveFactorial<unsigned long long>(30) << std::endl;
	std::cout << fibonacci(4) << " "
			  << fibonacci(5) << " "
			  << fibonacci(7) << std::endl;
	std::cout << nonRecursiveFibonacci(4) << " "
			  << nonRecursiveFibonacci(5) << " "
			  << nonRecursiveFibonacci(7) << std::endl;
	std::cout << function(5) << " "
			  << function(7) << " " << std::endl;
	std::cout << nonRecursiveFunction(5) << " "
			  << nonRecursiveFunction(7) << std::endl;
	std::cout << ackermann(1, 2) << " "
			  << ackermann(2, 1) << " "
			  << ackermann(3, 3) << std::endl;
	std::cout << euclid(20, 30) << " "
			  << euclid(112, 42) << std::endl;
	int array[10] { 1, 98, 41, 15, 32, 56, 44, 32, 5, 9 };
	std::cout << std::boolalpha << includeVal(array, 10, 32) << " "
			  << includeVal(array, 10, 9) << " "
			  << includeVal(array, 10, 100) << std::noboolalpha << std::endl;
	char chars[5] { 'a', 'b', 'c', 'd', 'e' };
	bool flags[5] {};
	std::fill(flags, std::end(flags), false);
	subsetGeneration(chars, flags, 0, 3);
	std::cout << std::endl;
	subsetGeneration(chars, flags, 0, 4);
	std::cout << std::endl;
	subsetGeneration(chars, flags, 0, 5);
	std::cout << std::endl;
	grayCode(4);
	std::cout << std::endl;
	grayCode(3);
	std::cout << std::endl;
}
