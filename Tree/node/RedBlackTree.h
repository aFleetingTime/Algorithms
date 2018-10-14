#pragma once
#include "../Tree.h"
#include "../basic/NodeBase.h"

namespace node {
	
template<typename Key>
class RedBlackNode : public basic::BasicNode<RedBlackNode<Key>> {
	template<typename Compare, template<typename> typename TreeOperation = basic::DefaultTreeOperation>
	struct RbTreeBalanceStrategy;
	struct RedBlackTreeSentry;

	enum class RBcolor : bool { red = true, black = false } color;
	Key val;
	
public:
	template<typename Comp, template<typename> typename TreeOperation = basic::DefaultTreeOperation>
	using BalanceStrategy = RbTreeBalanceStrategy<Comp, TreeOperation>;
	using IterativeStrategy = basic::DefaultIteratorOperation<RedBlackNode>;
	using Sentry = RedBlackTreeSentry;

	using value_type = Key;
	using basic_node = basic::BasicNode<RedBlackNode<Key>>;

	RedBlackNode(RBcolor c = RBcolor::red) : basic_node(&null, &null, &null), color(c), val{} { }
	RedBlackNode(value_type &&v) : basic_node(&null, &null, &null), color(RBcolor::red), val(std::move(v)) { }
	RedBlackNode(const value_type &v) : basic_node(&null, &null, &null), color(RBcolor::red), val(v) { }
	template<typename... Args>
	RedBlackNode(Args&&... args) : basic_node(&null, &null, &null), color(RBcolor::red), val(std::forward<Args>(args)...) { }

	RedBlackNode(const RedBlackNode&) = default;
	RedBlackNode& operator=(const RedBlackNode&) = default;
	RedBlackNode(RedBlackNode&&) = default;
	RedBlackNode& operator=(RedBlackNode&&) = default;

	bool empty() const noexcept { return this == &null; }
	explicit operator bool() const noexcept { return this == &null; }

	value_type& value() { return val; }
	const value_type& value() const { return val; }

	static RedBlackNode null;

private:
};

template<typename Key>
RedBlackNode<Key> RedBlackNode<Key>::null(RedBlackNode<Key>::RBcolor::black);

template<typename Key>
struct RedBlackNode<Key>::RedBlackTreeSentry
{
	RedBlackTreeSentry() : sentry(new RedBlackNode<Key>(RBcolor::black)) { }
	~RedBlackTreeSentry() { delete sentry; }

	RedBlackTreeSentry(const RedBlackTreeSentry&) = delete;
	RedBlackTreeSentry& operator=(const RedBlackTreeSentry&) = delete;
	RedBlackTreeSentry(RedBlackTreeSentry&&) = delete;
	RedBlackTreeSentry& operator=(RedBlackTreeSentry&&) = delete;

	void reset(RedBlackNode<Key> *root) {
		sentry->left = sentry->right = sentry->p = root;
	}

	RedBlackNode<Key> *sentry;
};

template<typename Key>
template<typename Compare, template<typename> typename TreeOp>
struct RedBlackNode<Key>::RbTreeBalanceStrategy : public TreeOp<RedBlackNode<Key>>
{
	using node_type = RedBlackNode<Key>;
	using TreeOperation = TreeOp<node_type>;

	RbTreeBalanceStrategy() = default;
	RbTreeBalanceStrategy(const RbTreeBalanceStrategy&) = default;
	RbTreeBalanceStrategy(RbTreeBalanceStrategy&&) = default;
	RbTreeBalanceStrategy& operator=(const RbTreeBalanceStrategy&) = default;
	RbTreeBalanceStrategy& operator=(RbTreeBalanceStrategy&&) = default;
	RbTreeBalanceStrategy(const Compare &c) : comp(c) { }

	void swap(RbTreeBalanceStrategy &rhs) noexcept {
		std::swap(comp, rhs.comp);
		std::swap(this->sentry.sentry, rhs.sentry.sentry);
	}

	void insert(node_type *&root, node_type *current, node_type *node)
	{
		if (root == &node_type::null)
		{
			root = node;
			node->p = TreeOperation::sentry.sentry;
			TreeOperation::resetSentry(root);
		}
		else
		{
			auto [target, dire] = TreeOp<node_type>::findInsertionPoint(current, node->value(), comp);
			node->p = const_cast<node_type*>(target);
			node->p->*dire = node;
		}
		insertFixup(root, node);
	}

	void erase(node_type *&root, node_type *node)
	{
		if (node->left == &node_type::null)
		{
			if (node->right == &node_type::null)
			{
				node_type* node_type::*d = node->filiation();
				TreeOperation::transplant(root, node, &node_type::null);
				if (root != &node_type::null && node->color == node_type::RBcolor::black)
					eraseFixupHelp(root, node->p, d);
				return;
			}
			TreeOperation::transplant(root, node, node->right);
			if (node->color == node_type::RBcolor::black)
				eraseFixup(root, node->right);
		}
		else if (node->right == &node_type::null)
		{
			TreeOperation::transplant(root, node, node->left);
			if (node->color == node_type::RBcolor::black)
				eraseFixup(root, node->left);
		}
		else
		{
			node_type *suc = const_cast<node_type*>(TreeOperation::min(node->right));
			node_type *x = suc->right, *p = suc;
			bool isRight = node == suc->p;

			if (!isRight)
			{
				if (x == &node_type::null)
				{
					suc->p->left = &node_type::null;
					p = suc->p;
				}
				else TreeOperation::transplant(root, suc, x);
				suc->right = node->right;
				suc->right->p = suc;
			}
			TreeOperation::transplant(root, node, suc);
			suc->left = node->left;
			suc->left->p = suc;
			typename node_type::RBcolor oldColor = suc->color;
			suc->color = node->color;
			if (oldColor != node_type::RBcolor::black)
				return;
			if (x == &node_type::null)
				eraseFixupHelp(root, p, isRight ? &node_type::right : &node_type::left);
			else eraseFixup(root, x);
		}
	}

	const node_type* find(const node_type *root, const typename Compare::key_type &key) const {
		return TreeOperation::findNode(root, key, comp);
	}

	std::pair<const node_type*, bool> findOrInsert(const node_type *root, const typename Compare::key_type &key) const {
		return TreeOperation::findInsertionPointOrNode(root, key, comp);
	}

protected:
	Compare comp;

private:
	void eraseFixupHelp(node_type *&root, node_type *node, node_type* node_type::*d)
	{
		node_type temp(node_type::RBcolor::black);
		temp.p = node;
		node->*d = &temp;
		eraseFixup(root, &temp);
		node->*d = &node_type::null;
	}

	void insertFixup(node_type *&root, node_type *node)
	{
		node_type* node_type::*a, *node_type::*b;
		while (node->p->color == node_type::RBcolor::red)
		{
			if (node->p == node->p->p->left)
				a = &node_type::right, b = &node_type::left;
			else a = &node_type::left, b = &node_type::right;
			if ((node->p->p->*a)->color == node_type::RBcolor::red)
			{
				node->p->color = (node->p->p->*a)->color = node_type::RBcolor::black;
				node->p->p->color = node_type::RBcolor::red;
				node = node->p->p;
			}
			else 
			{
				if (node == node->p->*a) {
					node = node->p;
					TreeOperation::rotate(root, node, a, b);
				}
				std::swap(node->p->color, node->p->p->color);
				TreeOperation::rotate(root, node->p->p, b, a);
			}
		}
		root->color = node_type::RBcolor::black;
	}

	void eraseFixup(node_type *& root, node_type *node)
	{
		node_type* node_type::*a, *node_type::*b, *w;
		while (node != root && node->color == node_type::RBcolor::black)
		{
			if (node == node->p->left)
				a = &node_type::right, b = &node_type::left;
			else a = &node_type::left, b = &node_type::right;
			w = node->p->*a;
			if (w->color == node_type::RBcolor::red)
			{
				std::swap(node->p->color, w->color);
				TreeOperation::rotate(root, node->p, a, b);
				w = node->p->*a;
			}
			if (w->left->color == node_type::RBcolor::black && w->right->color == node_type::RBcolor::black)
			{
				w->color = node_type::RBcolor::red;
				node = node->p;
			}
			else
			{
				if ((w->*a)->color == node_type::RBcolor::black)
				{
					std::swap((w->*b)->color, w->color);
					TreeOperation::rotate(root, w, b, a);
					w = node->p->*a;
				}
				w->color = node->p->color;
				node->p->color = (w->*a)->color = node_type::RBcolor::black;
				TreeOperation::rotate(root, node->p, a, b);
				node = root;
			}
		}
		node->color = node_type::RBcolor::black;
	}
};

}
