#include "heap.hpp"
#include <vector>

int main()
{
	std::vector<int> v { 10,31,53,5461,1,43,5,4,62,13 };
	heap::makeHeap(v.begin(), v.end());
	std::cout << std::boolalpha << heap::isHeap(v.begin(), v.end()) << std::endl;
	heap::sortHeap(v.begin(), v.end());
	for (auto vl : v)
		std::cout << vl << ' ';
	std::cout << '\n';
}
