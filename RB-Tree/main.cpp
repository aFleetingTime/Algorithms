#include "Map.hpp"
#include <iostream>
#include <map>
#include <random>
#include <functional>

int main()
{
	auto rand = std::bind(std::uniform_int_distribution<>(1, 100), std::default_random_engine(std::random_device()()));
	Map<int,int> m;
	for (int i = 0; i < 10; ++i)
		m.insert({ rand(), i });
	auto beg = m.begin(), end = m.end();
	beg = m.erase(beg);
	//beg = m.erase(beg);
	//beg = m.erase(beg);
	//beg = m.erase(beg);
	//for (auto &p : m)
		//std::cout << p.first << std::endl;
	std::cout << '\n';
	while (beg != end)
		std::cout << (*beg++).first << std::endl;
}
