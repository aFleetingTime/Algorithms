#pragma once
#include <iterator>
#include <type_traits>

template<typename Node>
struct Iterator;
template<typename Node>
struct ConstIterator;

template<typename Node>
bool operator==(const Iterator<Node> &lhs, const Iterator<Node> &rhs);
template<typename Node>
bool operator==(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs);
template<typename Node>
bool operator!=(const Iterator<Node> &lhs, const Iterator<Node> &rhs);
template<typename Node>
bool operator!=(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs);
template<typename Node>
bool operator!=(const Iterator<Node> &lhs, const ConstIterator<Node> &rhs);
template<typename Node>
bool operator!=(const ConstIterator<Node> &lhs, const Iterator<Node> &rhs);
template<typename Node>
bool operator==(const Iterator<Node> &lhs, const ConstIterator<Node> &rhs);
template<typename Node>
bool operator==(const ConstIterator<Node> &lhs, const Iterator<Node> &rhs);

template<typename Node>
struct Iterator : private Node::IterativeStrategy {
	using value_type = typename Node::value_type;
	using pointer = std::add_pointer_t<value_type>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

	friend bool operator!=<Node>(const Iterator<Node> &lhs, const Iterator<Node> &rhs);
	friend bool operator==<Node>(const Iterator<Node> &lhs, const Iterator<Node> &rhs);

	friend bool operator!=<Node>(const Iterator<Node> &lhs, const ConstIterator<Node> &rhs);
	friend bool operator!=<Node>(const ConstIterator<Node> &lhs, const Iterator<Node> &rhs);
	friend bool operator==<Node>(const Iterator<Node> &lhs, const ConstIterator<Node> &rhs);
	friend bool operator==<Node>(const ConstIterator<Node> &lhs, const Iterator<Node> &rhs);

	template<typename Key, template<typename> typename N, typename Compare, typename Alloc>
	friend class Tree;
	template<typename Key, typename Value, template<typename> typename N, typename Compare, typename Alloc>
	friend class TreeMap;
	template<typename N>
	friend class ConstIterator;

	struct CreateBegin { constexpr CreateBegin() = default; };
	static CreateBegin createBegin;

	Iterator() : node(&Node::null) { }
	Iterator(Node *root, CreateBegin) : node(const_cast<Node*>(Node::IterativeStrategy::min(root))) { }

	Iterator(Node *x) : node(x) { }

	Iterator& operator++() {
		node = const_cast<Node*>(Node::IterativeStrategy::successor(node));
		return *this;
	}
	Iterator operator++(int) {
		Iterator res(*this);
		return ++*this, res;
	}
	Iterator& operator--() {
		node = const_cast<Node*>(Node::IterativeStrategy::precursor(node));
		return *this;
	}
	Iterator& operator--(int) {
		Iterator res(*this);
		return --*this, res;
	}
	value_type& operator*() { return node->value(); }
	const value_type& operator*() const { return node->value(); }
	value_type* operator->() { return &node->value(); }
	const value_type* operator->() const { return &node->value(); }

private:
	Node *node;
};
template<typename Node>
typename Iterator<Node>::CreateBegin Iterator<Node>::createBegin;
template<typename Node>
bool operator==(const Iterator<Node> &lhs, const Iterator<Node> &rhs) {
	return lhs.node == rhs.node;
}
template<typename Node>
bool operator!=(const Iterator<Node> &lhs, const Iterator<Node> &rhs) {
	return !(lhs == rhs);
}

template<typename Node>
struct ConstIterator : private Node::IterativeStrategy {
	using value_type = typename Node::value_type;
	using pointer = std::add_pointer_t<const value_type>;
	using reference = std::add_lvalue_reference_t<const value_type>;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

	friend bool operator!=<Node>(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs);
	friend bool operator==<Node>(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs);

	friend bool operator!=<Node>(const Iterator<Node> &lhs, const ConstIterator<Node> &rhs);
	friend bool operator!=<Node>(const ConstIterator<Node> &lhs, const Iterator<Node> &rhs);
	friend bool operator==<Node>(const Iterator<Node> &lhs, const ConstIterator<Node> &rhs);
	friend bool operator==<Node>(const ConstIterator<Node> &lhs, const Iterator<Node> &rhs);

	template<typename Key, template<typename> typename N, typename Compare, typename Alloc>
	friend class Tree;
	template<typename Key, typename Value, template<typename> typename N, typename Compare, typename Alloc>
	friend class TreeMap;

	struct CreateBegin { constexpr CreateBegin() = default; };
	static CreateBegin createBegin;

	ConstIterator() : node(&Node::null) { }
	ConstIterator(const Node *root, CreateBegin) : node(Node::IterativeStrategy::min(root)) { }
	ConstIterator(const Node *x) : node(x) { }
	template<typename N, typename = std::enable_if_t<std::is_base_of_v<Node, N>, N>>
	ConstIterator(const ConstIterator<N> &it) : node(it.node) { }
	template<typename N, typename = std::enable_if_t<std::is_base_of_v<Node, N>, N>>
	ConstIterator(const Iterator<N> &it) : node(it.node) { }

	template<typename N, typename = std::enable_if_t<std::is_base_of_v<Node, N>, N>>
	ConstIterator& operator=(const ConstIterator<N> &it) { node = it.node; }
	template<typename N, typename = std::enable_if_t<std::is_base_of_v<Node, N>, N>>
	ConstIterator& operator=(const Iterator<N> &it) { node = it.node; }

	ConstIterator& operator++() {
		node = Node::IterativeStrategy::successor(node);
		return *this;
	}
	ConstIterator operator++(int) {
		ConstIterator res(*this);
		return ++*this, res;
	}
	ConstIterator& operator--() {
		node = Node::IterativeStrategy::precursor(node);
		return *this;
	}
	ConstIterator& operator--(int) {
		ConstIterator res(*this);
		return --*this, res;
	}
	const value_type& operator*() const { return node->value(); }
	const value_type* operator->() const { return &node->value(); }

private:
	const Node *node;
};
template<typename Node>
typename ConstIterator<Node>::CreateBegin ConstIterator<Node>::createBegin;

template<typename Node>
bool operator==(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs) {
	return lhs.node == rhs.node;
}
template<typename Node>
bool operator!=(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs) {
	return lhs.node != rhs.node;
}

template<typename Node>
bool operator!=(const Iterator<Node> &lhs, const ConstIterator<Node> &rhs) {
	return lhs.node != rhs.node;
}
template<typename Node>
bool operator!=(const ConstIterator<Node> &lhs, const Iterator<Node> &rhs) {
	return lhs.node != rhs.node;
}
template<typename Node>
bool operator==(const Iterator<Node> &lhs, const ConstIterator<Node> &rhs) {
	return lhs.node == rhs.node;
}
template<typename Node>
bool operator==(const ConstIterator<Node> &lhs, const Iterator<Node> &rhs) {
	return lhs.node == rhs.node;
}
