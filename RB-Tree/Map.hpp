#include <algorithm>
#include <iterator>
#include <iostream>
#include <type_traits>
#include <memory>

template<typename Key, typename Value, typename Compare>
class Map;

enum class color_type : bool { red = true, blank = false };

template<typename Value>
class node_type {
public:
	using value_type = Value;
	node_type() : p(&nil), left(&nil), right(&nil), color(color_type::blank) { }
	node_type(const value_type &val) : p(&nil), left(&nil), right(&nil), color(color_type::red), value(new value_type(val)) { }

	void leftRotate()
	{
		right->p = p;
		if (p->left == this)
			p->left = right;
		else p->right = right;
		p = right;
		right = std::exchange(right->left, this);
	}

	void rightRotate()
	{
		left->p = p;
		if (p->left == this)
			p->left = left;
		else p->right = left;
		p = left;
		left = std::exchange(left->right, this);
	}

	node_type* min() noexcept {
		node_type *current = this;
		while (current->left != &nil)
			current = current->left;
		return current;
	}

	node_type* max() noexcept {
		node_type *current = this;
		while (current->right != &nil)
			current = current->right;
		return current;
	}

	const node_type* min() const noexcept {
		const node_type *current = this;
		while (current->left != &nil)
			current = current->left;
		return current;
	}

	const node_type* max() const noexcept {
		const node_type *current = this;
		while (current->right != &nil)
			current = current->right;
		return current;
	}

	node_type *p;
	node_type *left;
	node_type *right;
	color_type color;
	Value *value;
	inline static node_type nil;
};

template<typename Node>
class iterator;
template<typename Node>
bool operator==(const iterator<Node> &lhs, const iterator<Node> &rhs);
template<typename Node>
bool operator!=(const iterator<Node> &lhs, const iterator<Node> &rhs);

template<typename Node>
class iterator {
public:
	template<typename Key, typename Value, typename Compare>
	friend class Map;
	friend bool operator==<Node>(const iterator<Node> &lhs, const iterator<Node> &rhs);
	explicit iterator(Node *n) : node(n) { }

	const iterator& operator++() const noexcept {
		if (node->right != &Node::nil)
			node = node->right->min();
		else
		{
			Node *current = node;
			while (current->p != &Node::nil && current == current->p->right)
				current = current->p;
			node = current->p;
		}
		return *this;
	}
	iterator& operator++() noexcept {
		return const_cast<iterator&>(++(*this));
	}

	iterator operator++(int) const noexcept {
		iterator temp = *this;
		++*this;
		return temp;
	}

	const iterator& operator--() const noexcept {
		if (node->left != &Node::nil)
			node = node->left->max();
		else
		{
			Node *current = node;
			while (current->p != &Node::nil && current == current->p->left)
				current = current->p;
			node = current->p;
		}
		return *this;
	}
	iterator& operator--() noexcept {
		return const_cast<iterator&>(--(*this));
	}

	iterator operator--(int) const noexcept {
		iterator temp = *this;
		--*this;
		return temp;
	}

	typename Node::value_type& operator*() noexcept { return *node->value; }
	const typename Node::value_type& operator*() const noexcept { return node->value; }

private:
	mutable Node *node;
};

template<typename Node>
bool operator==(const iterator<Node> &lhs, const iterator<Node> &rhs) {
	return lhs.node == rhs.node;
}
template<typename Node>
bool operator!=(const iterator<Node> &lhs, const iterator<Node> &rhs) {
	return !(lhs == rhs);
}

template<typename Key, typename Value, typename Compare = std::less<Key>>
class Map
{
public:
	using value_type = std::pair<const Key, Value>;
	using size_type = std::size_t;
	using node_type = ::node_type<value_type>;
	using iterator = ::iterator<node_type>;
	using const_iterator = const iterator;
	using insert_result_type = std::pair<iterator, bool>;

	Map(const Compare &c = Compare()) : root(&node_type::nil), comp(c), size(0) { }

	insert_result_type insert(const value_type &value)
	{
		node_type *old = &node_type::nil, *current = root;
		while (current != &node_type::nil)
		{
			old = current;
			if (comp(value.first, current->value->first))
				current = current->left;
			else if (comp(current->value->first, value.first))
				current = current->right;
			else return { iterator(current), false };
		}
		node_type *temp = new node_type(value);
		temp->p = old;
		if (old == &node_type::nil)
			root = temp;
		else if (comp(temp->value->first, old->value->first))
			old->left = temp;
		else old->right = temp;
		++size;
		return { iterator(temp), true };
	}

	iterator erase(const iterator &value)
	{
		node_type *result;
		if (value.node->left == &node_type::nil)
			result = transplant(value.node, value.node->right);
		else if (value.node->right == &node_type::nil)
			result = transplant(value.node, value.node->left);
		else
		{
			node_type *temp = value.node->right->min();
			if (temp->p != value.node)
			{
				transplant(temp, temp->right);
				temp->right = value.node->right;
				temp->right->p = temp;
			}
			result = transplant(value.node, temp);
			temp->left = value.node->left;
			temp->left->p = temp;
		}
		delete value.node;
		--size;
		return iterator(result);
	}

	node_type* test() { return root; }

private:
	node_type *root;
	Compare comp;
	size_type size;

	node_type* transplant(node_type *lhs, node_type *rhs)
	{
		if (lhs->p == &node_type::nil)
			root = rhs;
		else if (lhs == lhs->p->left)
			lhs->p->left = rhs;
		else lhs->p->right = rhs;
		rhs->p = lhs->p;
		return rhs;
	}


public:
	iterator begin() const noexcept { return iterator(root->min()); }
	iterator end() const noexcept { return iterator(&node_type::nil); }
};

#if 0
template<typename Value>
node_type<Value> node_type<Value>::nil(nullptr, nullptr, nullptr, color_type::blank);
#endif
