#pragma once
#include "Exceptions.hpp"
#include "HuffmanTree.hpp"

template<typename CharT, typename BitsT, template<typename...> typename Map>
void compress(const HuffmanTree<CharT, BitsT, Map> &ht, std::basic_istream<CharT> &input, std::basic_ostream<CharT> &output, std::size_t bufSize = 4096)
{
	using Tree = HuffmanTree<CharT, BitsT, Map>;
	using String = typename Tree::String;
	using Bitset = typename Tree::Bitset;
	using BitsetSizeType = typename Tree::BitsetSizeType;

	BitsetSizeType count = 0;
	Bitset bitBuf{};
	String tmpBuf(bufSize, '\0');
	while (input)
	{
		input.read(tmpBuf.data(), bufSize);
		String huffmanCoding = ht.encode(tmpBuf, input.gcount(), bitBuf, count);
		output.write(huffmanCoding.data(), huffmanCoding.size());
	}
	if (count != 0)
	{
		bitBuf <<= Tree::bitsetSize - count;
		for (BitsetSizeType i = (count - 1) / Tree::charBitSize + 1; i; --i)
			output.put(bitBuf >> Tree::bitsetSize - i * Tree::charBitSize);
	}
}

template<typename CharT, typename BitsT, template<typename...> typename Map>
void unCompress(const HuffmanTree<CharT, BitsT, Map> &ht, std::basic_istream<CharT> &input, std::basic_ostream<CharT> &output, std::size_t bufSize = 4096)
{
	using Tree = HuffmanTree<CharT, BitsT, Map>;
	using String = typename Tree::String;
	using FreqType = typename Tree::FreqType;
	using Bitset = typename Tree::Bitset;
	using BitsetSizeType = typename Tree::BitsetSizeType;

	FreqType srcFileSize = ht.freqSum();
	BitsetSizeType count = 0;
	Bitset bitBuf{};
	String tmpBuf(bufSize, '\0');
	while (input)
	{
		input.read(tmpBuf.data(), bufSize);
		String text = ht.decode(tmpBuf, input.gcount(), srcFileSize, bitBuf, count);
		output.write(text.data(), text.size());
		srcFileSize -= text.size();
	}
	if (srcFileSize > 0)
		throw UnCompressFailed("数据缺失，未能得到完整文件", srcFileSize);
}
