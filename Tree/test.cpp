#include <iostream>
#include <string>
#include <map>
#include <random>
#include "node/RedBlackTree.h"
#include <functional>

template<typename Node>
bool isRBT(Node *root, Node *nil)
{
	if (root->color != Node::RBcolor::black) return false;
	std::stack<std::pair<Node*, int>> s;
	s.push({root, 0});
	std::vector<int> vec;
	while (!s.empty())
	{
		auto p = s.top(); s.pop();
		bool b = p.first->color == Node::RBcolor::black;
		if (p.first->left == nil && p.first->right == nil)
		{
			vec.push_back(p.second + b);
			continue;
		}
		if (p.first->right != nil)
		{
			if (p.first->color == Node::RBcolor::red && p.first->right->color != Node::RBcolor::black)
				return false;
			s.push({p.first->right, p.second + b});
		}
		if (p.first->left != nil)
		{
			if (p.first->color == Node::RBcolor::red && p.first->left->color != Node::RBcolor::black)
				return false;
			s.push({p.first->left, p.second + b});
		}
	}
	int h = vec.back();
	vec.pop_back();
	for (int v : vec)
		if (v != h) return false;
	return true;
}

int main()
{
	auto rand = std::bind(std::uniform_int_distribution<>(1, 150000000), std::default_random_engine(std::random_device()()));
	using Map = Tree<int, node::RedBlackNode, std::greater<int>>;
	TreeMap<int, int, node::RedBlackNode> t;

	std::vector<int> v;
	v.resize(4561213);
	t[888888] = 12;
	for (int i = 0; i < 4561213; ++i)
	{
		v[i] = rand();
		t.insert({v[i], i});
	}
	std::sort(v.begin(), v.end());
	//v.erase(std::unique(v.begin(), v.end()), v.end());
	std::random_shuffle(v.begin(), v.end());
	int s = v.size();
	t[13134] = 12;
	for (int i = 0; i < v.size() - 1; ++i)
	{
		t.erase(t.find(v[i]));
		--s;
	}
	std::cout << (--t.end())->first << std::endl;
	std::cout << v.back() << std::endl;
	std::cout << (t.begin())->first << std::endl;
	std::cout << t.size() << ' ' << v.size() << std::endl;
}
