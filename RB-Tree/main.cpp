#include "Map.hpp"
#include <vector>
#include <stack>
#include <iostream>
#include <map>
#include <random>
#include <functional>

enum F
{
	root, left, right
};

struct T {
};

struct P : T {
};


void print(typename Map<int, int>::node_type *node, typename Map<int, int>::node_type *nil, F f = F::root, int n = 0)
{
	if (node == nil) return;
	//std::cout << (node->color == Color::red ? "RED" : "BLACK") << ' ';
	if (f == F::root)
		std::cout << n << " root: ";
	else if (f == F::left)
		std::cout << n << " left: ";
	else
		std::cout << n << " righ: ";
	std::cout << "value=" << node->value->first << " color=" << (node->color == Color::red ? "RED" : "BLACK") << '\n';
	print(node->left, nil, F::left, n+1);
	print(node->right, nil, F::right, n+1);
}

int main()
{
	auto rand = std::bind(std::uniform_int_distribution<>(1, 20000000), std::default_random_engine(std::random_device()()));
	Map<int,int> mm;
	std::vector<int> r;
	for (int i = 0; i < 2000000; ++i)
		r.push_back(rand());
	std::sort(r.begin(), r.end());
	r.erase(std::unique(r.begin(), r.end()), r.end());
	std::random_shuffle(r.begin(), r.end());
	for (int v : r)
		mm.insert({v, v});
	std::random_shuffle(r.begin(), r.end());
	for (int i = 0; i < r.size() - 1; ++i)
		mm.erase(mm.find(r[i]));
	std::cout << mm.test()->value->first << std::endl;
	std::cout << r.back() << std::endl;
	for (auto v : mm)
		std::cout << v.first << std::endl;
#if 0
	std::vector<int> vec {119, 531, 238, 398, 337, 227, 680, 95, 658, 741};
		int v;
	for (int i = 0; i < 10; ++i)
	//for (int v : vec)
	{
		v = rand();
		std::cout << v << ", ";
		m.insert({ v, v });
	}
	m[9999] = 1234;
	std::cout << m[9999] << std::endl;
	m.erase(m.find(v));
	print(m.test(), m.gnil());
	std::cout << std::boolalpha << isRBT(m.test(), m.gnil()) << std::endl;
	print(m.test(), m.gnil());
#endif
}
