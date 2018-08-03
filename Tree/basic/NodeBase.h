#pragma once

namespace basic {

template<typename NodeType>
struct NodeBase {
	NodeBase(NodeType *l, NodeType *r) : left(l), right(r) { }
	NodeBase(const NodeBase&) = default;
	NodeBase& operator=(const NodeBase&) = default;
	NodeType *left, *right;
};

template<typename NodeType>
struct BasicNode : public NodeBase<NodeType> {
	BasicNode(NodeType *f, NodeType *l, NodeType *r) :NodeBase<NodeType>(l, r), p(f) { }
	BasicNode(const BasicNode&) = default;
	BasicNode& operator=(const BasicNode&) = default;
	inline NodeType* NodeType::* filiation() { return (this == p->left) ? &NodeType::left : &NodeType::right; }
	inline void setFiliation(NodeType *node) { (this == p->left ? p->left : p->right) = node; }
	NodeType *p;
};

}
