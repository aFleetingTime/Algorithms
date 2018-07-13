#include "FibHeap.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <queue>

int main()
{
#if 0
	std::list<int> l1{10}, l2{20};
	auto it1 = l1.begin(), it2 = l2.begin();
	l1.splice(l1.end(), l2);
	std::cout << *it1 << std::endl;
	std::cout << *it2 << std::endl;
#endif
	FibHeap<int> fh;
	auto rand = std::bind(std::uniform_int_distribution<>(0, 10000000), std::default_random_engine(std::random_device()()));
	auto start = std::chrono::steady_clock::now();
	FibHeap<int>::iterator it, it2;
	for (std::size_t i = 0; i < 500000; ++i)
	{
		it = fh.insert(rand());
		it2 = fh.insert(rand());
	}
	std::cout << fh.pop() << std::endl;
	fh.decrease(it, -111);
	fh.decrease(it, -113);
	std::cout << fh.pop() << std::endl;
	fh.decrease(it2, -112);
	std::cout << fh.pop() << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>((std::chrono::steady_clock::now() - start)).count() << std::endl;
}
