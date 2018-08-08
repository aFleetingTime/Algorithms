#pragma once
#include "HuffmanTree.h"

template<typename CharT, std::size_t MaxBit>
void compress(const HuffmanTree<CharT, MaxBit> &ht, std::basic_istream<CharT> &input, std::basic_ostream<CharT> &output)
{
	using tree_type = HuffmanTree<CharT, MaxBit>;
	using bit_size_type = typename tree_type::bit_size_type;
	constexpr auto bit_size = tree_type::bit_size;
	constexpr auto max_bit_size = tree_type::max_bit_size;
	constexpr auto eof = std::basic_istream<CharT>::traits_type::eof();

	typename tree_type::bits current;
	std::size_t count = 0;

	int ch;
	std::array<char, 4096> buf;
	//while ((ch = input.get()) != eof)
	std::streamsize readSize;
	while (readSize = input.readsome(buf.data(), 4096))
	{
		for (std::streamsize bufIndex = 0; bufIndex < readSize; ++bufIndex)
		{
			auto [bits, size] = ht.toBits(buf[bufIndex]);
			if (count + size <= max_bit_size)
			{
				current = (current << size) | bits;
				count += size;
			}
			else
			{
				std::size_t dis = max_bit_size - count;
				current <<= dis;
				count = size - dis;
				current |= (bits >> count);
				unsigned long bit = current.to_ulong();
				for (bit_size_type i = max_bit_size / bit_size; i;)
					output.put(bit >> bit_size * --i);//.to_ulong());
				current = bits;
				//for (bit_size_type i = count; i < size; ++i)
					//current.reset(i);
			}
		}
	}
	if (count != 0)
	{
		current <<= max_bit_size - count;
		for (bit_size_type i = max_bit_size / bit_size; i;)
			output.put((current >> bit_size * --i).to_ulong());
	}
}

template<typename CharT, std::size_t MaxBit>
void unCompress(const HuffmanTree<CharT, MaxBit> &ht, std::basic_istream<CharT> &input, std::basic_ostream<CharT> &output)
{
	using tree_type = HuffmanTree<CharT, MaxBit>;
	using bit_size_type = typename tree_type::bit_size_type;
	constexpr auto bit_size = tree_type::bit_size;
	constexpr auto max_bit_size = tree_type::max_bit_size;
	constexpr auto distance = max_bit_size - bit_size;

	std::intmax_t ch = input.get();
	if (!input) return;

	typename tree_type::bits bits(ch), next;
	bits <<= distance;
	std::size_t filesize = ht.freqSum(), bitsCount = bit_size, nextCount = 0;

	while (filesize != 0)
	{
		auto [ch, size] = ht.toChar(bits, bitsCount);
		if (size != 0)
		{
			bitsCount -= size;
			bits <<= size;
			output.put(ch);
			--filesize;
		}
		else
		{
			if (nextCount == 0)
			{
				if (!input)
					throw std::runtime_error("解压失败: 压缩文件损坏");
				next.reset() |= input.get() << distance;
				nextCount = bit_size;
			}
			bits |= next >> bitsCount;
			if (max_bit_size - bitsCount <= nextCount)
			{
				nextCount -= max_bit_size - bitsCount;
				next <<= max_bit_size - bitsCount;
				bitsCount = max_bit_size;
			}
			else
			{
				bitsCount += nextCount;
				nextCount = 0;
			}
		}
	}
}
