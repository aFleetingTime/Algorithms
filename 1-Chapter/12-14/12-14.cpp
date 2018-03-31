#include <iostream>
#include "MyAlgorithm.hpp"

int main()
{
	int **dA = make2dArray<int>(3, 10);
	dA[2][9] = 100;
	std::cout << dA[2][9] << std::endl;
	changeLength2D(dA, 3, 10, 5, 20);
	std::cout << dA[2][9] << std::endl;
	dA[4][19] = 1000;
	std::cout << dA[4][19] << std::endl;
	delete2dArray(dA, 5);
}
