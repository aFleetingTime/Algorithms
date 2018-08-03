#include <iostream>
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
	Tree<int, node::RedBlackNode> t;
	Tree<int, node::RedBlackNode> tt;
	std::swap(tt, t);
	std::cout << "1" << std::endl;
	t.begin();
	std::cout << "2" << std::endl;
	tt.begin();
#if 0
	Tree<int, RedBlackNode> t;
	std::vector<int> v;
	v.resize(4561213);
	for (int i = 0; i < 4561213; ++i)
	{
		v[i] = rand();
		t.insert(v[i]);
	}
	std::random_shuffle(v.begin(), v.end());
	for (int i = 0; i < 4000000; ++i)
		t.erase(t.find(v[i]));
	Tree<int, RedBlackNode> tt;
	tt.swap(t);
	std::cout << std::boolalpha << isRBT(tt.pubroot(), tt.pubnull()) << std::endl;

	Tree<int, RedBlackNode> a;
	a.begin();
	std::cout << *(--tt.end()) << std::endl;
	std::cout << *std::max_element(v.begin() + 4000000, v.end()) << std::endl;
	std::cout << *(tt.begin()) << std::endl;
	std::cout << *std::min_element(v.begin() + 4000000, v.end()) << std::endl;
	std::cout << *(t.begin()) << std::endl;
	std::cout << *std::min_element(v.begin() + 4000000, v.end()) << std::endl;
	std::cout << t.size() << std::endl;
#endif
}
