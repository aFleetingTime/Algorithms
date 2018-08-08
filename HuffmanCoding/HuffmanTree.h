#pragma once
#include <map>
#include <stack>
#include <queue>
#include <bitset>

struct HuffmanTreeNode {
	struct before_type {
		bool operator()(const std::pair<std::size_t, HuffmanTreeNode*> &lhs, const std::pair<std::size_t, HuffmanTreeNode*> rhs) {
			return lhs.first > rhs.first;
		}
	};

	HuffmanTreeNode(HuffmanTreeNode *l = nullptr, HuffmanTreeNode *r = nullptr,
			HuffmanTreeNode *p = nullptr) : left(l), right(r), parent(p) { }

	HuffmanTreeNode *left, *right, *parent;

	bool currentBit() const noexcept {
		return this == parent->right;
	}
};

template<typename CharT>
struct HuffmanTreeLeafNode : public HuffmanTreeNode {
	HuffmanTreeLeafNode(CharT v = CharT{}, HuffmanTreeNode *l = nullptr,
			HuffmanTreeNode *r = nullptr, HuffmanTreeNode *p = nullptr) : HuffmanTreeNode(l, r, p), value(v) { }
	CharT value;
};

template<typename CharT, std::size_t MaxBit>
class HuffmanTree;
template<typename CharT, std::size_t MaxBit>
std::basic_istream<CharT>& operator>>(std::basic_istream<CharT>&, HuffmanTree<CharT, MaxBit>&);
template<typename CharT, std::size_t MaxBit>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>&, const HuffmanTree<CharT, MaxBit>&);

template<typename CharT, std::size_t MaxBit>
class HuffmanTree
{
	static_assert(sizeof(CharT) <= MaxBit);

public:
	using freq_type = std::size_t;
	using size_type = std::size_t;
	using node_type = HuffmanTreeNode;
	using leaf_node_type = HuffmanTreeLeafNode<CharT>;
	using bits = std::bitset<MaxBit>;
	using char_type = CharT;
	using map_type = std::map<CharT, leaf_node_type*>;
	using bit_size_type = unsigned short;

	constexpr static bit_size_type bit_size = sizeof(CharT) * 8;
	constexpr static bit_size_type max_bit_size = MaxBit;

	friend std::basic_ostream<CharT>& operator<<<CharT, MaxBit>(std::basic_ostream<CharT>&, const HuffmanTree&);
	friend std::basic_istream<CharT>& operator>><CharT, MaxBit>(std::basic_istream<CharT>&, HuffmanTree&);

	HuffmanTree() : root(new node_type(0)) { }

	HuffmanTree(const std::map<CharT, freq_type> &freqMap)
	{
		if (freqMap.empty())
		{
			root = new node_type(0);
			freq = 0;
			return;
		}
		std::priority_queue<std::pair<std::size_t, node_type*>,
				std::vector<std::pair<std::size_t, node_type*>>, node_type::before_type> queue;
		for (auto [ch, freq] : freqMap)
		{
			leaf_node_type *leaf = new leaf_node_type(ch);
			queue.emplace(freq, leaf);
			leaves.emplace(ch, leaf);
		}
		if (queue.size() == 1)
		{
			root = new node_type(queue.top().second, queue.top().second);
			root->left->parent = root;
		}
		else
		{
			std::pair<std::size_t, node_type*> x, y;
			while (queue.size() > 1)
			{
				x = queue.top(); queue.pop();
				y = queue.top(); queue.pop();
				node_type *p = new node_type(x.second, y.second);
				x.second->parent = y.second->parent = p;
				queue.emplace(x.first + y.first, p);
			}
			root = queue.top().second;
		}
		freq = queue.top().first;
	}

	~HuffmanTree()
	{
		for (auto [c, leaf] : leaves)
		{
			(leaf == leaf->parent->left ? leaf->parent->left : leaf->parent->right) = nullptr;
			delete leaf;
		}
		std::stack<node_type*> s;
		s.push(root);
		while (!s.empty())
		{
			node_type *tmp = s.top(); s.pop();

			if (tmp->right)
				s.push(tmp->right);
			if (tmp->left)
				s.push(tmp->left);

			delete tmp;
		}
	}

	bool contains(CharT c) { return leaves.find(c) != leaves.end(); }

	std::pair<bits, unsigned short> toBits(CharT c) const
	{
		bits res{};
		unsigned short count = 0;
		for (node_type *n = leaves.at(c); n != root; ++count, n = n->parent)
			res[count] = n->currentBit();
		return { res, count };
	}

	std::pair<char_type, unsigned short> toChar(const bits &bs, unsigned short size) const
	{
		if (!root)
			throw std::logic_error("");

		const node_type *n = root;
		unsigned short count = 0;

		while ((n->left && n->right) && size)
		{
			n = (bs.test(max_bit_size - count - 1) ? n->right : n->left);
			++count, --size;
		}

		if (n->left && n->right)
			return { CharT{}, 0 };
		return { static_cast<const leaf_node_type*>(n)->value, count };
	}
	
	size_type leafSize() const noexcept { return leaves.size(); }
	size_type size() const noexcept { return leaves.size() * 2 - 1; }
	size_type freqSum() const noexcept { return freq; }

private:
	freq_type freq;
	node_type *root;
	map_type leaves;
};

template<typename CharT, std::size_t MaxBit>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> &output, const HuffmanTree<CharT, MaxBit> &tree)
{
	using tree_type = HuffmanTree<CharT, MaxBit>;
	constexpr typename tree_type::bit_size_type while_count = tree_type::max_bit_size / tree_type::bit_size;

	output << tree.leafSize() << '-';
	if (tree.freq == 0)
		return output;

	for (const auto &leaf : tree.leaves)
	{
		auto [bits, size] = tree.toBits(leaf.second->value);
		output.put(leaf.second->value);
		output.put(size);
		bits <<= tree_type::max_bit_size - size;
		typename tree_type::bit_size_type i = while_count;
		unsigned long bit = bits.to_ulong();
		while (i != 0)
			output.put(bit >> --i * tree_type::bit_size);
	}

	return output << '-' << tree.freq << '-';
}
template<typename CharT, std::size_t MaxBit>
std::basic_istream<CharT>& operator>>(std::basic_istream<CharT> &input, HuffmanTree<CharT, MaxBit> &tree)
{
	using tree_type = HuffmanTree<CharT, MaxBit>;
	using node_type = typename tree_type::node_type;
	using leaf_node_type = typename tree_type::leaf_node_type;
	constexpr auto eof = std::basic_istream<CharT>::traits_type::eof();

	node_type *root = new node_type;
	typename tree_type::map_type leaves;
	typename tree_type::bit_size_type size;

	std::size_t leafSize;
	input >> leafSize;
	input.get();

	char value;
	while (leafSize != 0)
	{
		value = input.get();
		typename tree_type::bits bits;
		size = input.get();
		for (typename tree_type::bit_size_type i = tree_type::max_bit_size / tree_type::bit_size; i;)
			bits |= std::uintmax_t(input.get()) << (tree_type::bit_size * --i);
		node_type *current = root;

		for (typename tree_type::bit_size_type i = 1; i < size; ++i)
		{
			node_type* node_type::*d = bits.test(MaxBit - i) ? &node_type::right : &node_type::left;
			if (!(current->*d))
			{
				current->*d = new node_type;
				(current->*d)->parent = current;
			}
			current = current->*d;
		}

		leaf_node_type *leaf = new leaf_node_type(value);
		leaf->parent = current;
		(bits.test(tree_type::max_bit_size - size) ? current->right : current->left) = leaf;

		--leafSize;
		if (leaves.emplace(value, leaf).second == false)
		{
			tree_type destroy;
			destroy.leaves = std::move(leaves);
			destroy.root = root;
			throw std::runtime_error("压缩文件格式错误");
		}
	}

	tree.~HuffmanTree();
	tree.root = root;
	tree.leaves = std::move(leaves);

	input.get();
	input >> tree.freq;
	input.get();

	return input;
}
