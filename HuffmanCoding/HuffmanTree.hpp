#pragma once
#include <cmath>
#include <map>
#include <stack>
#include <queue>
#include <bitset>
#include <string>
#include "HuffmanTreeNode.hpp"

template<typename T>
inline void toUnsigned(T&);
template<typename T, typename U>
inline void bitCopy(T&, U);

template<typename CharT, typename BitsT, template<typename...> typename Map>
class HuffmanTree;
template<typename CharT, typename BitsT, template<typename...> typename Map>
std::basic_istream<CharT>& operator>>(std::basic_istream<CharT>&, HuffmanTree<CharT, BitsT, Map>&);
template<typename CharT, typename BitsT, template<typename...> typename Map>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>&, const HuffmanTree<CharT, BitsT, Map>&);

template<typename CharT = char, typename BitsT = unsigned, template<typename...> typename Map = std::map>
class HuffmanTree
{
	static_assert(sizeof(CharT) <= sizeof(BitsT) && std::is_unsigned_v<BitsT>);

public:
	using CharType = CharT;
	using FreqType = std::size_t;
	using SizeType = std::size_t;
	using BitsetSizeType = unsigned short;
	using NodeType = HuffmanTreeNode;
	using LeafNodeType = HuffmanTreeLeafNode<CharType>;
	using MapType = Map<CharType, LeafNodeType*>;
	using FreqMapType = std::map<CharType, FreqType>;
	using Bitset = BitsT;
	using String = std::basic_string<CharType>;
	using PriorityQueue = std::priority_queue<std::pair<std::size_t, NodeType*>,
			std::vector<std::pair<std::size_t, NodeType*>>, NodeType::BeforeType>;

	constexpr static CharType fileSeparator = '-';
	constexpr static BitsetSizeType bitsetSize = sizeof(BitsT) * 8;
	constexpr static BitsetSizeType charBitSize = sizeof(CharT) * 8;
	constexpr static Bitset highBit = Bitset(0x1) << (bitsetSize - 1);
	constexpr static Bitset lowBit = 0x1;

	friend std::basic_ostream<CharT>& operator<<<CharT, BitsT>(std::basic_ostream<CharT>&, const HuffmanTree&);
	friend std::basic_istream<CharT>& operator>><CharT, BitsT>(std::basic_istream<CharT>&, HuffmanTree&);

	HuffmanTree() : root(new NodeType) { }
	HuffmanTree(std::istream &input) : root(nullptr) { input >> *this; }
	HuffmanTree(const FreqMapType&);
	HuffmanTree(const HuffmanTree&);
	HuffmanTree(HuffmanTree &&rhs) noexcept : freq(std::exchange(rhs.freq, 0)),
									root(std::exchange(rhs.root, nullptr)), leaves(std::move(rhs.leaves)) { }

	HuffmanTree& operator=(HuffmanTree rhs) noexcept {
		if (root == rhs.root)
			rhs.root = nullptr;
		else swap(rhs);
		return *this;
	}
	~HuffmanTree();

	void swap(HuffmanTree &rhs) noexcept {
		using std::swap;
		swap(freq, rhs.freq);
		swap(root, rhs.root);
		swap(leaves, rhs.leaves);
	}

	String encode(const String &text, SizeType size, Bitset &bitBuf, BitsetSizeType &count) const;
	String decode(const String &bits, SizeType size, SizeType decodeSize, Bitset &bitBuf, BitsetSizeType &count) const;

	bool contains(CharType c) { return leaves.find(c) != leaves.end(); }

	auto fromBitset(Bitset, BitsetSizeType) const;
	auto toBitset(CharType) const;
	
	SizeType leafCount() const noexcept { return leaves.size(); }
	SizeType nodeSize() const noexcept { return leaves.size() * 2 - 1; }
	SizeType freqSum() const noexcept { return freq; }

private:
	void constructTree(PriorityQueue&);
	NodeType* copyAllNode(const NodeType*, NodeType* = nullptr);

	FreqType freq;
	NodeType *root;
	MapType leaves;
};

template<typename CharT, typename BitsT, template<typename...> typename Map>
void HuffmanTree<CharT, BitsT, Map>::constructTree(PriorityQueue &queue)
{
	if (queue.empty())
	{
		root = new NodeType;
		freq = 0;
		return;
	}
	if (queue.size() == 1)
	{
		root = new NodeType(queue.top().second, queue.top().second);
		root->left->parent = root;
	}
	else
	{
		std::pair<std::size_t, NodeType*> x, y;
		while (queue.size() > 1)
		{
			x = queue.top(); queue.pop();
			y = queue.top(); queue.pop();
			NodeType *p = new NodeType(x.second, y.second);
			x.second->parent = y.second->parent = p;
			queue.emplace(x.first + y.first, p);
		}
		root = queue.top().second;
	}
	freq = queue.top().first;
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
HuffmanTree<CharT, BitsT, Map>::HuffmanTree(const FreqMapType &freqMap)
{
	PriorityQueue queue;
	for (auto [ch, freq] : freqMap)
	{
		LeafNodeType *leaf = new LeafNodeType(ch);
		queue.emplace(freq, leaf);
		leaves.emplace(ch, leaf);
	}
	constructTree(queue);
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
HuffmanTree<CharT, BitsT, Map>::HuffmanTree(const HuffmanTree &rhs) : freq(rhs.freq) {
	root = copyAllNode(rhs.root);
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
typename HuffmanTree<CharT, BitsT, Map>::NodeType* HuffmanTree<CharT, BitsT, Map>::copyAllNode(const NodeType *node, NodeType *parent)
{
	if (!node)
		return nullptr;
	else if (node->left || node->right)
	{
		NodeType *tmp = new NodeType;
		tmp->parent = parent;
		tmp->left = copyAllNode(node->left, tmp);
		tmp->right = copyAllNode(node->right, tmp);
		return tmp;
	}
	else
	{
		LeafNodeType *tmp = new LeafNodeType(static_cast<const LeafNodeType*>(node)->value);
		tmp->parent = parent;
		leaves.emplace(tmp->value, tmp);
		return tmp;
	}
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
HuffmanTree<CharT, BitsT, Map>::~HuffmanTree()
{
	if (root == nullptr)
		return;
	for (auto [c, leaf] : leaves)
	{
		(leaf == leaf->parent->left ? leaf->parent->left : leaf->parent->right) = nullptr;
		delete leaf;
	}
	std::stack<NodeType*> s;
	s.push(root);
	while (!s.empty())
	{
		NodeType *tmp = s.top(); s.pop();
		if (tmp->right)
			s.push(tmp->right);
		if (tmp->left)
			s.push(tmp->left);
		delete tmp;
	}
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
typename HuffmanTree<CharT, BitsT, Map>::String HuffmanTree<CharT, BitsT, Map>::encode
	(const String &text, SizeType size, Bitset &bitBuf, BitsetSizeType &count) const
{
	String result;
	for (SizeType i = 0; i < size; ++i)
	{
		auto [bits, size] = toBitset(text[i]);
		if (count + size <= bitsetSize)
		{
			(bitBuf <<= size) |= bits;
			count += size;
		}
		else
		{
			BitsetSizeType diff = bitsetSize - count;
			count = size - diff;
			(bitBuf <<= diff) |= bits >> count;
			for (BitsetSizeType j = bitsetSize / charBitSize; j;)
				result.push_back(bitBuf >> charBitSize * --j);
			(bitBuf = 0) |= bits;
		}
	}
	if (count != 0)
	{
		Bitset bit = bitBuf << bitsetSize - count;
		for (BitsetSizeType i = 1, j = count / charBitSize; i <= j; ++i)
		{
			result.push_back(bit >> (bitsetSize - i * charBitSize));
			count -= charBitSize;
		}
	}
	return result;
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
typename HuffmanTree<CharT, BitsT, Map>::String HuffmanTree<CharT, BitsT, Map>::decode
	(const String &bits, SizeType size, SizeType decodeSize, Bitset &bitBuf, BitsetSizeType &count) const
{
	if (decodeSize == 0)
		return {};

	String result;
	for (SizeType i = 0; i < size; ++i)
	{
		bitCopy(bitBuf, bits[i]);
		count += charBitSize;
		while (true)
		{
			auto [ch, decodeNum] = fromBitset(bitBuf, count);
			if (decodeNum == 0)
				break;
			result.push_back(ch);
			count -= decodeNum;
			if (result.size() == decodeSize)
				return result;
		}
	}
	while (result.size() < decodeSize)
	{
		auto [ch, decodeNum] = fromBitset(bitBuf, count);
		if (decodeNum == 0)
			break;
		result.push_back(ch);
		count -= decodeNum;
	}
	return result;
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
auto HuffmanTree<CharT, BitsT, Map>::toBitset(CharType c) const
{
	using ResultType = std::pair<Bitset, BitsetSizeType>;
	BitsetSizeType count = 0;
	std::bitset<bitsetSize> tmp;
	for (NodeType *n = leaves.at(c); n != root; ++count, n = n->parent)
		tmp[count] = n->bit();
	return ResultType{tmp.to_ullong(), count};
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
auto HuffmanTree<CharT, BitsT, Map>::fromBitset(Bitset bs, BitsetSizeType size) const
{
	using ResultType = std::pair<CharType, BitsetSizeType>;

	const NodeType *n = root;
	BitsetSizeType count = 0;

	bs <<= bitsetSize - size;
	while ((n->left || n->right) && size)
	{
		n = (bs & highBit ? n->right : n->left);
		bs <<= 1;
		++count, --size;
	}

	if (n->left || n->right)
		return ResultType{CharType{}, 0};
	return ResultType{static_cast<const LeafNodeType*>(n)->value, count};
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT> &output, const HuffmanTree<CharT, BitsT, Map> &tree)
{
	using Tree = HuffmanTree<CharT, BitsT, Map>;
	using BitsetSizeType = typename Tree::BitsetSizeType;
	constexpr BitsetSizeType whileCount = Tree::bitsetSize / Tree::charBitSize;

	output << tree.leafCount() << Tree::fileSeparator;
	for (const auto &leaf : tree.leaves)
	{
		auto [bits, size] = tree.toBitset(leaf.second->value);
		output.put(leaf.second->value).put(size);
		BitsetSizeType i = whileCount;
		while (i != 0)
			output.put(bits >> --i * Tree::charBitSize);
	}
	return output << Tree::fileSeparator << tree.freq << Tree::fileSeparator;
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
std::basic_istream<CharT>& operator>>(std::basic_istream<CharT> &input, HuffmanTree<CharT, BitsT, Map> &tree)
{
	using Tree = HuffmanTree<CharT, BitsT, Map>;
	using Bitset = typename Tree::Bitset;
	using String = typename Tree::String;
	using NodeType = typename Tree::NodeType;
	using LeafNodeType = typename Tree::LeafNodeType;
	using BitsetSizeType = typename Tree::BitsetSizeType;
	constexpr BitsetSizeType whileCount = Tree::bitsetSize / Tree::charBitSize;

	typename Tree::FreqType leavesInfoSize;
	input >> leavesInfoSize;
	if (input.get() != Tree::fileSeparator)
		throw FileFormatError("压缩文件内容格式错误");

	leavesInfoSize *= whileCount + 2;
	String leavesInfo(leavesInfoSize, '\0');
	input.read(leavesInfo.data(), leavesInfoSize);
	if (input.gcount() != leavesInfoSize)
		throw FileDataLost("压缩信息缺失", leavesInfoSize, input.gcount());

	typename Tree::MapType leaves;
	NodeType *root = new NodeType;
	for (std::size_t index = 0; index < leavesInfoSize; index += whileCount + 2)
	{
		Bitset bits;
		for (BitsetSizeType i = whileCount - ((leavesInfo[index + 1] - 1) / Tree::charBitSize + 1); i < whileCount; ++i)
			bitCopy(bits, leavesInfo[index + i + 2]);
		bits <<= Tree::bitsetSize - leavesInfo[index + 1];

		NodeType *current = root;
		for (BitsetSizeType i = 1; i < leavesInfo[index + 1]; ++i)
		{
			NodeType* NodeType::*d = (bits & Tree::highBit) ? &NodeType::right : &NodeType::left;
			if (!(current->*d))
			{
				current->*d = new NodeType;
				(current->*d)->parent = current;
			}
			current = current->*d;
			bits <<= 1;
		}

		LeafNodeType *leaf = new LeafNodeType(leavesInfo[index]);
		leaf->parent = current;
		((bits & Tree::highBit) ? current->right : current->left) = leaf;
		leaves.emplace(leaf->value, leaf);
	}

	tree.~HuffmanTree();
	tree.root = root;
	tree.leaves = std::move(leaves);

	auto a = input.get();
	input >> tree.freq;
	auto b = input.get();
	if (a != Tree::fileSeparator || b != Tree::fileSeparator)
		throw FileFormatError("压缩文件内容格式错误");
	return input;
}

template<typename T>
inline void toUnsigned(T &b) {
	if (b < 0)
		b -= std::pow(2, sizeof(T) - 1);
}

template<typename T, typename U>
inline void bitCopy(T &a, U b) {
	constexpr std::size_t bit = sizeof(U) * 8;
	if (b < 0)
	{
		(a <<= bit) |= std::make_unsigned_t<U>(b) - std::make_unsigned_t<U>(std::pow(2, bit - 1));
		a |= T(0x1) << bit - 1;
	}
	else (a <<= bit) |= std::make_unsigned_t<U>(b);

	/*
	if (b < 0)
	{
		(a <<= sizeof(U) * 8) |= std::make_unsigned_t<U>(b) - std::make_unsigned_t<U>(std::pow(2, sizeof(U) * 8 - 1));
		a |= T(0x1) << sizeof(U) * 8 - 1;
	}
	else (a <<= sizeof(U) * 8) |= std::make_unsigned_t<U>(b);
	*/

	//std::make_unsigned_t<U> c = b;
	//if (b < 0)
	//{
	//	c -= std::pow(2, sizeof(U) * 8 - 1);
	//	(a <<= sizeof(U) * 8) |= c;
	//	a |= T(0x1) << sizeof(U) * 8 - 1;
	//}
	//else (a <<= sizeof(U) * 8) |= c;
}
