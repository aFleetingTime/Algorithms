#pragma once
#include <utility>

struct HuffmanTreeNode {
	struct BeforeType {
		bool operator()(const std::pair<std::size_t, HuffmanTreeNode*> &lhs, const std::pair<std::size_t, HuffmanTreeNode*> rhs) {
			return lhs.first > rhs.first;
		}
	};

	HuffmanTreeNode(HuffmanTreeNode *l = nullptr, HuffmanTreeNode *r = nullptr,
			HuffmanTreeNode *p = nullptr) : left(l), right(r), parent(p) { }
	HuffmanTreeNode *left, *right, *parent;

	bool bit() const noexcept {
		return this == parent->right;
	}
};

template<typename CharT>
struct HuffmanTreeLeafNode : public HuffmanTreeNode {
	HuffmanTreeLeafNode(CharT v = CharT{}, HuffmanTreeNode *l = nullptr, HuffmanTreeNode *r = nullptr,
			HuffmanTreeNode *p = nullptr) : HuffmanTreeNode(l, r, p), value(v) { }
	CharT value;
};
