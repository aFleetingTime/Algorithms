#include <iostream>
#include <SimpleIntervalTree.hpp>

int count = 0;
template<typename Node>
void print(const Node *root, const Node *null, int i = 1)
{
	if (root == null) return;
	std::cout << i << ": " << root->priority << "  interval: " << root->interval.low << ' ' << root->interval.high << 
		" max: " << root->max << std::endl;
	//if (i > count) count = i;
	print(root->left, null, i + 1);
	print(root->right, null, i + 1);
}

int main()
{
	auto rand = std::bind(std::uniform_int_distribution<int>(0, 10000000), std::default_random_engine(std::random_device()()));
	SimpleIntervalTree<int> m;
	std::vector<Interval<int>> v;//{{16,21},{8,9},{25,30},{5,8},{15,23},{17,19},{26,26},{0,3},{6,10},{19,20}};
	for (std::size_t s = 0; s < 1235512; ++s)
	{
		int a = rand(), b = rand();
		while (a > b) b = rand();
		v.push_back({a, b});
		m.insert({a, b});
	}
	std::random_shuffle(v.begin(), v.end());
	for (auto a : v)
		if (m.findExactly(a) == m.nullp()) ++count;
	std::cout << count << std::endl;
#if 0
	for (int i = 0; i < v.size() / 3; ++i)
		m.erase(m.find(v[i]));
	print(m.rootp(), m.nullp());
	auto res = m.findAll(target);
	for (auto *p : res)
		std::cout << p->interval.low << ' ' << p->interval.high << std::endl;
#endif
}
