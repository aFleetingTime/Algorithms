#pragma once
#include <stack>
#include <memory>
#include "component/Iterator.h"
#include "basic/BasicTreeOperation.h"

/* Tree: 
 * 1. 平衡策略由结点类型定义的BalanceStrategy决定。
 * 2. 要求BalanceStrategy定义insert、erase和find实现树的操作，树的平衡由BalanceStrategy维护。
 * 3. Node类型需定义null结点。
 * 4. BalanceStrategy继承链必须包含一个哨兵sentry，并维护哨兵的左孩子、右孩子和父结点总是指向正确的位置，Tree通过提供给迭代器sentry实现对树各结点的迭代。
 * 5. Node定义iterator_result_type指定迭代器返回类型。
 *
 *
 */


template<typename Key, template<typename> typename Node, typename Compare = std::less<Key>, typename Alloc = std::allocator<Node<Key>>>
class Tree : private Node<Key>::template BalanceStrategy<Compare>
{
public:
	using node_type = Node<Key>;
	using value_type = typename node_type::value_type;
	using key_type = typename node_type::key_type;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = std::add_lvalue_reference_t<value_type>;
	using const_reference = std::add_const_t<reference>;
	using iterator = component::Iterator<node_type>;
	using const_iterator = component::ConstIterator<node_type>;
	using allocator_type = Alloc;
	using size_type = std::size_t;
	using balance = typename Node<Key>::template BalanceStrategy<Compare>;

	Tree() : root(&node_type::null), nodeCount(0) { }
	Tree(std::initializer_list<value_type> list) : root(&node_type::null), nodeCount(0) {
		for (const value_type &v : list)
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

	Tree(const Compare &c) : balance(c), root(&node_type::null), nodeCount(0) { }

	Tree(Tree &&rhs) : balance(std::move(rhs)), alloc(rhs.alloc), root(rhs.root), nodeCount(rhs.nodeCount)
	{
		rhs.root = &node_type::null;
		rhs.nodeCount = 0;
	}

	void swap(Tree &rhs) noexcept {
		std::swap(alloc, rhs.alloc);
		std::swap(root, rhs.root);
		std::swap(nodeCount, rhs.nodeCount);
		balance::swap(rhs);
	}

	template<typename InputIt>
	Tree(InputIt first, InputIt last, const Compare &c = Compare{}) : balance(c) {
		for (; first != last; ++first)
			insert(*first);
	}
	Tree(std::initializer_list<value_type> init, const Compare &c = Compare{}) : balance(c) {
		for (const value_type &v : init)
			insert(v);
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

	iterator insert(const value_type &key) {
		node_type *newNode = createNode(key);
		balance::insert(root, newNode);
		++nodeCount;
		return iterator(newNode);
	}
	template<typename... Args>
	iterator emplace(Args&&... args)
	{
		node_type *newNode = createNode(std::forward<Args>(args)...);
		balance::insert(root, newNode);
		++nodeCount;
		return iterator(newNode);
	}

	iterator erase(iterator it) {
		iterator temp = it;
		balance::erase(root, temp.node);
		destroyNode(temp.node);
		--nodeCount;
		return it;
	}

	iterator find(const key_type &key) {
		return iterator(const_cast<node_type*>(balance::find(root, key)));
	}
	const_iterator find(const key_type &key) const {
		return const_iterator(balance::find(root, key));
	}

	iterator begin() noexcept { return iterator(this->sentry.sentry, iterator::createBegin); }
	iterator end() noexcept { return iterator(this->sentry.sentry); }
	const_iterator begin() const noexcept { return const_iterator(this->sentry.sentry, const_iterator::createBegin); }
	const_iterator end() const noexcept { return const_iterator(this->sentry.sentry); }
	
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
