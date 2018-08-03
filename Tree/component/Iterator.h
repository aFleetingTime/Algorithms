#pragma once
#include <iterator>
#include <type_traits>

namespace component {

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
struct Iterator : private Node::IterativeStrategy {
	using value_type = typename Node::value_type;
	using pointer = std::add_pointer_t<value_type>;
	using reference = std::add_lvalue_reference_t<value_type>;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

	friend bool operator==<Node>(const Iterator<Node> &lhs, const Iterator<Node> &rhs);
	friend bool operator!=<Node>(const Iterator<Node> &lhs, const Iterator<Node> &rhs);
	template<typename Key, template<typename> typename N, typename Compare, typename Alloc>
	friend class Tree;

	struct CreateBegin { constexpr CreateBegin() = default; };
	struct CreateEnd { constexpr CreateEnd() = default; };
	static CreateBegin createBegin;
	static CreateEnd createEnd;

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
	typename Node::iterator_result_type& operator*() { return node->value(); }
	const typename Node::iterator_result_type& operator*() const { return node->value(); }
	typename Node::iterator_result_type* operator->() { return &node->value(); }
	const typename Node::iterator_result_type* operator->() const { return &node->value(); }

private:
	Node *node;
};
template<typename Node>
typename Iterator<Node>::CreateBegin Iterator<Node>::createBegin;
template<typename Node>
typename Iterator<Node>::CreateEnd Iterator<Node>::createEnd;
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
	using value_type = std::remove_cv_t<typename Node::value_type>;
	using pointer = std::add_pointer_t<const value_type>;
	using reference = std::add_lvalue_reference_t<const value_type>;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

	friend bool operator==<Node>(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs);
	friend bool operator!=<Node>(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs);
	template<typename Key, template<typename> typename N, typename Compare, typename Alloc>
	friend class Tree;

	struct CreateBegin { constexpr CreateBegin() = default; };
	struct CreateEnd { constexpr CreateEnd() = default; };
	static CreateBegin createBegin;
	static CreateEnd createEnd;

	ConstIterator(const Node *root, CreateBegin) : node(Node::IterativeStrategy::min(root)) { }
	//ConstIterator(CreateEnd) : node(&Node::null) { }

	ConstIterator(const Node *x) : node(x) { }

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
typename ConstIterator<Node>::CreateEnd ConstIterator<Node>::createEnd;
template<typename Node>
bool operator==(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs) {
	return lhs.node == rhs.node;
}
template<typename Node>
bool operator!=(const ConstIterator<Node> &lhs, const ConstIterator<Node> &rhs) {
	return !(lhs == rhs);
}

}
