#include <locale>
#include <fstream>
#include <unordered_map>
#include "BasicLogic.h"
#include "HuffmanCompress.hpp"
#include "../Algorithms.cpp"

namespace fs = std::filesystem;

using HuffmanCodingTree = HuffmanTree<CharType, uIntType, std::unordered_map>;
using Map = std::map<CharType, std::size_t>;
using UMap = std::unordered_map<CharType, std::size_t>;

int main(int argv, char *argc[])
{
	std::locale::global(std::locale(""));
	if (argv != 4 || (argc[3][0] != Compress && argc[3][0] != UnCompress))
		cerrExit("program -src -dest -z|d");

	fs::path src(argc[1]), dest(argc[2]);
	checkPath(src);

	char flag = argc[3][0];
	preprocessingPath(flag, src, dest);
	std::basic_ifstream<CharType> input(src, std::ios::binary);
	checkStream(input);
	std::basic_ofstream<CharType> output(dest, std::ios::binary);
	checkStream(output);

	test::Timer t;
	t.start();
	if (flag == Compress)
	{
		HuffmanCodingTree ht(convMap<Map>(countFreq<UMap>(input)));
		input.clear();
		input.seekg(0, std::ios::beg);
		try {
			output << ht;
		} catch (std::exception &e) {
			cerrExit(e.what());
		} catch (...) {
			cerrExit("未知错误");
		}
		compress(ht, input, output);
		std::cout << t.finish().count() << std::endl;
		std::cout << "压缩前: " << ht.freqSum() << "(bytes)\n"
				  << "压缩后: " << output.tellp() << "(bytes)\n";
	}
	else
	{
		try {
			HuffmanCodingTree ht(input);
			unCompress(ht, input, output);
		} catch (std::exception &e) {
			cerrExit(e.what());
		} catch (...) {
			cerrExit("未知错误");
		}
		std::cout << "解压后: " << output.tellp() << "(bytes)\n";
	}
}
