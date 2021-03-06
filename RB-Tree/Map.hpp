#pragma once
#include <iostream>
#include <stack>
#include <vector>
#include <memory>
#include <iterator>
#include <algorithm>
#include <type_traits>

template<typename Key, typename Value, typename Compare>
class Map;

enum class Color : bool { red = true, black = false };

template<typename Value>
class Node {
public:
	using value_type = Value;
	Node(Node *nl, Color c = Color::red) : p(nl), left(nl), right(nl), color(c), value(nullptr) { }
	template<typename T>
	Node(Node *nl, T&& val, Color c = Color::red)
	try : p(nl), left(nl), right(nl), color(c), value(new value_type(std::forward<T>(val)))
	{ } catch (...) { delete value; throw; }

	~Node() { delete value; }

	Node*& child() const {
		return this == p->left ? p->left : p->right;
	}

	Node *p, *left, *right;
	Color color;
	Value *value;
};

template<typename Node>
bool isRBT(Node *root, Node *nil)
{
	if (root->color != Color::black) return false;
	std::stack<std::pair<Node*, int>> s;
	s.push({root, 0});
	std::vector<int> vec;
	while (!s.empty())
	{
		auto p = s.top(); s.pop();
		bool b = p.first->color == Color::black;
		if (p.first->left == nil && p.first->right == nil)
		{
			vec.push_back(p.second + b);
			continue;
		}
		if (p.first->right != nil)
		{
			if (p.first->color == Color::red && p.first->right->color != Color::black)
				return false;
			s.push({p.first->right, p.second + b});
		}
		if (p.first->left != nil)
		{
			if (p.first->color == Color::red && p.first->left->color != Color::black)
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


template<typename Node>
class Iterator;
template<typename Node>
bool operator==(const Iterator<Node> &lhs, const Iterator<Node> &rhs);
template<typename Node>
bool operator!=(const Iterator<Node> &lhs, const Iterator<Node> &rhs);

template<typename Node>
class Iterator {
public:
	using difference_type = std::ptrdiff_t;
	using value_type = typename Node::value_type;
	using reference = value_type&;
	using pointer = value_type*;
	using iterator_category = std::bidirectional_iterator_tag;

	template<typename Key, typename Value, typename Compare>
	friend class Map;
	friend bool operator==<Node>(const Iterator<Node> &lhs, const Iterator<Node> &rhs);
	Iterator() : node(nullptr), nil(nullptr) { }
	explicit Iterator(Node *n, const Node *nl) : node(n), nil(nl) { }

	Iterator& operator++() noexcept {
		next();
		return *this;
	}

	Iterator operator++(int) noexcept {
		Iterator temp = *this;
		++*this;
		return temp;
	}

	Iterator& operator--() noexcept {
		prev();
		return *this;
	}

	Iterator operator--(int) noexcept {
		Iterator temp = *this;
		--*this;
		return temp;
	}

	typename Node::value_type& operator*() noexcept { return *node->value; }
	const typename Node::value_type& operator*() const noexcept { return *node->value; }

	typename Node::value_type* operator->() noexcept { return node->value; }
	const typename Node::value_type* operator->() const noexcept { return node->value; }

private:
	void toNextNode(Node* (Node::*a), Node* (Node::*b)) noexcept
	{
		Node *current = node;
		if (node->*b != nil)
		{
			current = current->*b;
			while (current->*a != nil)
				current = current->*a;
			node = current;
		}
		else
		{
			while (current->p != nil && current == current->p->*b)
				current = current->p;
			node = current->p;
		}
	}
	void next() noexcept { toNextNode(&Node::left, &Node::right); }
	void prev() noexcept { toNextNode(&Node::right, &Node::left); }

	Node *node;
	const Node *nil;
};

template<typename Node>
bool operator==(const Iterator<Node> &lhs, const Iterator<Node> &rhs) {
	return lhs.node == rhs.node;
}
template<typename Node>
bool operator!=(const Iterator<Node> &lhs, const Iterator<Node> &rhs) {
	return !(lhs == rhs);
}

template<typename Key, typename Value, typename Compare = std::less<Key>>
class Map
{
public:
	using value_type = std::pair<const Key, Value>;
	using size_type = std::size_t;
	using node_type = ::Node<value_type>;
	using iterator = ::Iterator<node_type>;
	using const_iterator = const iterator;
	using insert_result_type = std::pair<iterator, bool>;

	Map(const Compare &c = Compare{}) : root(&nil), comp(c), nodeCount(0) { nil.left = root, nil.right = root; }
	template<typename InputIt>
	Map(InputIt first, InputIt last, const Compare &c = Compare{}) : Map(c) {
		while (first != last)
		{
			insert(*first);
			++first;
		}
	}
	Map(std::initializer_list<value_type> init, const Compare &c = Compare{}) : Map(c) {
		for (const value_type &v : init)
			insert(v);
	}

	~Map() {
		if (root == &nil) return;
		std::stack<node_type*> s;
		s.push(root);
		node_type *p;
		while (!s.empty())
		{
			p = s.top(); s.pop();
			if (p->right != &nil) s.push(p->right);
			if (p->left != &nil) s.push(p->left);
			delete p;
		}
	}

	insert_result_type insert(const value_type &value)
	{
		node_type *old = &nil, *current = root, *temp = new node_type(&nil, value);
		if (current == &nil)
			root = temp;
		else {
			iterator current = find(value.first);
			if (current.node != &nil)
				return { current, false };
			temp->p = current.node->p;
			if (comp(value.first, current.node->p->value->first))
				current.node->p->left = temp;
			else current.node->p->right = temp;
		}
		++nodeCount;
		insertFixup(temp);
		nil.left = nil.right = root;
		return { iterator(temp, &nil), true };
	}

	iterator erase(const iterator &value)
	{
		if (value.node == &nil) return value;
		node_type *result, *x, *temp;
		Color oldColor = value.node->color;
		if (value.node->left == &nil)
		{
			result = (++iterator(value.node, &nil)).node;
			x = transplant(value.node, value.node->right);
		}
		else if (value.node->right == &nil)
		{
			result = (++iterator(value.node, &nil)).node;
			x = transplant(value.node, value.node->left);
		}
		else
		{
			temp = value.node->right;
			while (temp->left != &nil) temp = temp->left;
			std::swap(temp->color, oldColor);
			x = temp->right;
			if (temp->p != value.node)
			{
				transplant(temp, temp->right);
				temp->right = value.node->right;
				temp->right->p = temp;
			}
			else x->p = temp;
			result = transplant(value.node, temp);
			temp->left = value.node->left;
			temp->left->p = temp;
		}
		--nodeCount;
		delete value.node;
		if (oldColor == Color::black)
			eraseFixup(x);
		return iterator(result, &nil);
	}

	iterator find(const Key &key)
	{
		if (root == &nil) {
			nil.p = &nil;
			return iterator(&nil, &nil);
		}
		node_type *current = root, *(node_type::*flag);
		while (true)
		{
			if (comp(key, current->value->first))
				flag = &node_type::left;
			else if (comp(current->value->first, key))
				flag = &node_type::right;
			else return iterator(current, &nil);
			if (current->*flag == &nil)
				break;
			current = current->*flag;
		}
		nil.p = current;
		return iterator(&nil, &nil);
	}

	Value& operator[](const Key &key) {
		iterator temp = find(key);
		if (temp.node->p == &nil)
			return insert(value_type(key, Value{})).first->second;
		else if (temp.node != &nil)
			return temp->second;
		node_type *newNode = new node_type(&nil, value_type(key, Value{}));
		newNode->p = temp.node->p;
		if (comp(key, temp.node->p->value->first))
			temp.node->p->left = newNode;
		else temp.node->p->right = newNode;
		++nodeCount;
		insertFixup(newNode);
		nil.left = nil.right = root;
		return newNode->value->second;
	}

	size_type size() const noexcept { return nodeCount; }

	iterator begin() noexcept {
		nil.left = nil.right = root;
		return ++iterator(&nil, &nil);
	}
	iterator end() noexcept { return iterator(&nil, &nil); }

	node_type* test() { return root; }
	node_type* gnil() { return &nil; }

private:
	node_type *root, nil = node_type(&nil, Color::black);
	Compare comp;
	size_type nodeCount;

	void insertFixup(node_type *node) noexcept
	{
		while (node->p->color == Color::red)
		{
			node_type* (node_type::*a), *(node_type::*b);
			if (node->p == node->p->p->left)
				a = &node_type::right, b = &node_type::left;
			else a = &node_type::left, b = &node_type::right;
			if ((node->p->p->*a)->color == Color::red)
			{
				(node->p->p->*a)->color = node->p->color = Color::black;
				node->p->p->color = Color::red;
				node = node->p->p;
			}
			else
			{
				if (node == node->p->*a)
				{
					node = node->p;
					rotate(node, a, b);
				}
				std::swap(node->p->color, node->p->p->color);
				rotate(node->p->p, b, a);
			}
		}
		root->color = Color::black;
	}

	void eraseFixup(node_type *node)
	{
		while (node != root && node->color == Color::black)
		{
			node_type* (node_type::*a), *(node_type::*b);
			if (node == node->p->left)
				a = &node_type::right, b = &node_type::left;
			else a = &node_type::left, b = &node_type::right;
			node_type *w = node->p->*a;
			if (w->color == Color::red)
			{
				std::swap(w->color, node->p->color);
				rotate(node->p, a, b);
				w = node->p->*a;
			}
			if ((w->*a)->color == Color::black && (w->*b)->color == Color::black)
			{
				if (w == &nil) throw;
				w->color = Color::red;
				node = node->p;
			}
			else
			{
				if ((w->*a)->color == Color::black)
				{
					std::swap((w->*b)->color, w->color);
					specialRotation(w, b, a);
					w = node->p->*a;
				}
				w->color = node->p->color;
				(w->*a)->color = node->p->color = Color::black;
				rotate(node->p, a, b);
				node = root;
			}
		}
		node->color = Color::black;
	}

	node_type* transplant(node_type *lhs, node_type *rhs)
	{
		if (lhs->p == &nil)
			root = rhs;
		else lhs->child() = rhs;
		rhs->p = lhs->p;
		return rhs;
	}

	void rotate(node_type *node, node_type* (node_type::*a), node_type* (node_type::*b)) noexcept
	{
		(node->*a)->p = node->p;
		if (node->p == &nil)
			root = node->*a;
		else node->child() = node->*a;
		node->p = node->*a;
		((node->*a)->*b)->p = node;
		node->*a = std::exchange((node->*a)->*b, node);
	}

	void specialRotation(node_type *node, node_type* (node_type::*a), node_type* (node_type::*b)) noexcept
	{
		(node->*a)->p = node->p;
		if (node->p == &nil)
			root = node->*a;
		else node->child() = node->*a;
		node->p = node->*a;
		if ((node->*a)->*b != &nil) // 判断node->*a->*b是否为nil再对node->*a->*b->p进行赋值。
			((node->*a)->*b)->p = node;
		/*
		 * 如果不加((node->*a)->*b != &nil)的判断，则eraseFixup在特定情况下无法正确执行。
		 * 原因如下:
		 * 假设RB-DELETE-FIXUP的参数x为nil(在待删除结点没有左孩子或右孩子，或者后继没有右孩子时为nil)
		 * 但凡进入情况3，RB-DELETE-FIXUP第15行(情况3)会对x的兄弟结点w进行左旋/右旋，
		 * 如果w相应参与旋转的孙子结点为nil (node->*a->*b == &nil)，而旋转过程没有判断直接赋值给参数为nil的父结点，
		 * 则相当于将同为nil的x的父结点p赋值为w，随后从情况3执行情况4，而情况4的第17、20行将引用错误的结点w。
		 */
		node->*a = std::exchange((node->*a)->*b, node);
	}

	void leftRotate(node_type *node) noexcept {
		rotate(node, &node_type::right, &node_type::left);
	}

	void rightRotate(node_type *node) noexcept {
		rotate(node, &node_type::left, &node_type::right);
	}
};
