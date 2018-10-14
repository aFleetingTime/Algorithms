#pragma once
#include "NodeBase.h"

namespace basic {

template<typename Node>
struct DefaultIteratorOperation
{
	const Node* precursor(const Node *node) const {
		return toNext(node, &Node::left, &Node::right);
	}
	const Node* successor(const Node *node) const {
		return toNext(node, &Node::right, &Node::left);
	}
	const Node* min(const Node *node) const { return minmax(node, &Node::left); }
	const Node* max(const Node *node) const { return minmax(node, &Node::right); }

private:
	const Node* minmax(const Node *node, Node* Node::*dire) const
	{
		while (node->*dire != &Node::null)
			node = node->*dire;
		return node;
	}
	const Node* toNext(const Node *node, Node* Node::*a, Node* Node::*b) const
	{
		if (node == &Node::null) return node;
		if (node->*a != &Node::null)
			return minmax(node->*a, b);
		while (node != &Node::null && node != node->p->*b)
			node = node->p;
		return node->p;
	}
};

template<typename Node>
struct DefaultTreeOperation : public DefaultIteratorOperation<Node> {
void transplant(Node *&root, Node *x, Node *y)
{
	if (x->p == sentry.sentry)
	{
		root = y;
		resetSentry(y);
	}
	else x->setFiliation(y);
	if (y != &Node::null)
		y->p = x->p;
}

void rotate(Node *&root, Node *x, Node* (Node::*a), Node* (Node::*b))
{
	(x->*a)->p = x->p;
	transplant(root, x, x->*a);
	x->p = x->*a;
	if ((x->*a)->*b != &Node::null)
		((x->*a)->*b)->p = x;
	x->*a = std::exchange((x->*a)->*b, x);
}

void leftRotate(Node *&root, Node *x) {
	rotate(root, x, &Node::right, &Node::left);
}
void rightRotate(Node *&root, Node *x) {
	rotate(root, x, &Node::left, &Node::right);
}

template<typename Compare>
auto findInsertionPoint(const Node *root, const typename Compare::value_type &key, const Compare &comp) const	// 假定root不为null
{
	Node* Node::*dire;
	while (true)
	{
		if (comp(key, root->value()))
			dire = &Node::left;
		else dire = &Node::right;
		if (root->*dire == &Node::null) break;
		root = root->*dire;
	}
	return std::pair{root, dire};
}

template<typename Compare>
std::pair<const Node*, bool> findInsertionPointOrNode(const Node *root, const typename Compare::key_type &key, const Compare &comp) const	// 假定root不为null
{
	Node* Node::*dire;
	bool exist;
	while (true)
	{
		if (comp(key, root->value()))
			dire = &Node::left;
		else if (comp(root->value(), key))
			dire = &Node::right;
		else
		{
			exist = true;
			break;
		}
		if (root->*dire == &Node::null)
		{
			exist = false;
			break;
		}
		root = root->*dire;
	}
	return {root, exist};
}

template<typename Compare>
const Node* findNode(const Node *root, const typename Compare::key_type &key, const Compare &comp) const
{
	while (root != &Node::null)
	{
		if (comp(key, root->value()))
			root = root->left;
		else if (comp(root->value(), key))
			root = root->right;
		else break;
	}
	return root;
}

void resetSentry(Node *root) {
	sentry.reset(root);
}

protected:
typename Node::Sentry sentry;
};

} // namespace basic
