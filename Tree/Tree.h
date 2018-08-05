#pragma once
#include <stack>
#include <memory>
#include "component/Iterator.h"
#include "component/TreeHelper.hpp"
#include "basic/BasicTreeOperation.h"


/* Tree: 
 * 1. 平衡策略由结点类型定义的BalanceStrategy实现。
 * 2. 要求BalanceStrategy定义insert、erase和find实现树的操作，树的平衡由BalanceStrategy维护。
 * 3. Node类型需定义null结点。
 * 4. BalanceStrategy继承链必须包含一个哨兵sentry，并维护哨兵的左孩子、右孩子和父结点总是指向正确的位置，Tree通过提供给迭代器sentry实现对树各结点的迭代。
 * 5. BalanceStrategy需要有一个模板参数，该参数通过Tree的模板参数Compare传递给BalanceStrategy，以提供树的比较函数对象。
 */


template<typename Key, template<typename> typename Node, typename Compare = std::less<Key>, typename Alloc = std::allocator<Node<Key>>>
class Tree : protected Node<Key>::template BalanceStrategy<toCompareAux<Key, Compare>>
{
public:
	using node_type = Node<Key>;
	using compare = toCompareAux<Key, Compare>;
	using balance = typename node_type::template BalanceStrategy<compare>;
	using value_type = typename compare::value_type;
	using key_type = typename compare::key_type;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = std::add_lvalue_reference_t<value_type>;
	using const_reference = std::add_const_t<reference>;
	using iterator = ConstIterator<node_type>;
	using const_iterator = ConstIterator<node_type>;
	using allocator_type = Alloc;
	using size_type = std::size_t;

	Tree() : root(&node_type::null), nodeCount(0) { }
	Tree(std::initializer_list<value_type> init) : root(&node_type::null), nodeCount(0) {
		for (const value_type &v : init)
			insert(v);
	}
	template<typename InputIt>
	Tree(InputIt first, InputIt last) : root(&node_type::null), nodeCount(0) {
		while (first != last)
		{
			emplace(*first);
			std::advance(first, 1);
		}
	}
	Tree(std::initializer_list<value_type> init, const Compare &c) : balance(c) {
		for (const value_type &v : init)
			insert(v);
	}
	template<typename InputIt>
	Tree(InputIt first, InputIt last, const Compare &c) : balance(c) {
		while (first != last)
		{
			emplace(*first);
			std::advance(first, 1);
		}
	}


	Tree(const Compare &c) : balance(c), root(&node_type::null), nodeCount(0) { }

	Tree(Tree &&rhs) : balance(std::move(rhs)), alloc(rhs.alloc), root(rhs.root), nodeCount(rhs.nodeCount)
	{
		rhs.root = &node_type::null;
		rhs.nodeCount = 0;
	}


	~Tree() {
		if (root == &node_type::null) return;
		std::stack<node_type*> s;
		s.push(root);
		node_type *p;
		while (!s.empty())
		{
			p = s.top(); s.pop();
			if (p->right != &node_type::null) s.push(p->right);
			if (p->left != &node_type::null) s.push(p->left);
			destroyNode(p);
		}
	}

	void swap(Tree &rhs) noexcept {
		std::swap(alloc, rhs.alloc);
		std::swap(root, rhs.root);
		std::swap(nodeCount, rhs.nodeCount);
		balance::swap(rhs);
	}


	const_iterator insert(const value_type &key) {
		node_type *newNode = createNode(key);
		balance::insert(root, newNode);
		++nodeCount;
		return iterator(newNode);
	}
	iterator insert(value_type &&key) {
		return emplace(std::move(key));
	}
	template<typename... Args>
	const_iterator emplace(Args&&... args)
	{
		node_type *newNode = createNode(std::forward<Args>(args)...);
		balance::insert(root, newNode);
		++nodeCount;
		return iterator(newNode);
	}

	const_iterator erase(const_iterator it) {
		iterator temp = it;
		balance::erase(root, const_cast<node_type*>(temp.node));
		destroyNode(const_cast<node_type*>(temp.node));
		--nodeCount;
		return it;
	}

	const_iterator find(const key_type &key) const {
		const node_type *target = balance::find(root, key);
		return (target == &node_type::null) ? cend() : const_iterator(target);
	}

	const_iterator begin() const noexcept { return const_iterator(this->sentry.sentry, const_iterator::createBegin); }
	const_iterator end() const noexcept { return const_iterator(this->sentry.sentry); }
	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cend() const noexcept { return end(); }
	
	size_type size() const noexcept { return nodeCount; }

	// Debug
	node_type* pubroot() { return root; }
	node_type* pubnull() { return &node_type::null; }

private:
	Alloc alloc;
	node_type *root;
	size_type nodeCount;

	template<typename... Args>
	node_type* createNode(Args&&... args)
	{
		node_type *node = alloc.allocate(1);
		try {
			alloc.construct(node, std::forward<Args>(args)...);
		} catch (...) {
			alloc.deallocate(node, 1);
			throw;
		}
		return node;
	}

	void destroyNode(node_type *node)
	{
		alloc.destroy(node);
		alloc.deallocate(node, 1);
	}
};

template<typename Key, template<typename> typename Node, typename Compare, typename Alloc>
void std::swap(Tree<Key, Node, Compare, Alloc> &lhs, Tree<Key, Node, Compare, Alloc> &rhs) { lhs.swap(rhs); }

template<typename Key, typename Value, template<typename> typename Node,
	     typename Compare = std::less<Key>, typename Alloc = std::allocator<Node<std::pair<const Key, Value>>>>
class TreeMap : public Tree<std::pair<const Key, Value>, Node, toMapCompareAux<Key, Value, Compare>, Alloc>
{
	using base = Tree<std::pair<const Key, Value>, Node, toMapCompareAux<Key, Value, Compare>, Alloc>;

public:
	using mapped_type = typename base::value_type::second_type;
	using iterator = Iterator<typename base::node_type>;

	using base::base;

	iterator insert(const typename base::value_type &key) {
		return iterator(const_cast<typename base::node_type*>(base::insert(key).node));
	}
	template<typename... Args>
	iterator emplace(Args&&... args) {
		return iterator(const_cast<typename base::node_type*>(base::emplace(std::forward<Args>(args)...).node));
	}
	iterator insert(typename base::value_type &&key) {
		return emplace(std::move(key));
	}

	iterator erase(typename base::const_iterator it) {
		return iterator(const_cast<typename base::node_type*>(base::erase(it).node));
	}

	iterator find(const typename base::key_type &key) {
		return iterator(const_cast<typename base::node_type*>(base::find(key).node));
	}
	typename base::const_iterator find(const typename base::key_type &key) const {
		return base::find(key);
	}

	mapped_type& operator[](const typename base::key_type &key)
	{
		iterator res = find(key);
		if (res.node != &base::node_type::null)
			return res->second;
		return insert(typename base::value_type{key, {}})->second;
	}
	mapped_type& operator[](typename base::key_type &&key)
	{
		iterator res = find(key);
		if (res.node != &base::node_type::null)
			return res->second;
		int i = 0;
		return insert(typename base::value_type{std::move(key), {}})->second;
	}

	mapped_type& at(const typename base::key_type &key) {
		return const_cast<mapped_type&>(const_cast<const TreeMap&>(*this).at(key));
	}
	const mapped_type& at(const typename base::key_type &key) const
	{
		typename base::const_iterator res = find(key);
		if (res.node == &base::node_type::null)
			throw std::out_of_range("");
		return res->second;
	}

	iterator begin() noexcept { return iterator(this->sentry.sentry, iterator::createBegin); }
	iterator end() noexcept { return iterator(this->sentry.sentry); }
};
