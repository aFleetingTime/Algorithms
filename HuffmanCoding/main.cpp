#include "HuffmanCompress.h"
#include <array>
#include <locale>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;
using char_type = char;

constexpr char Compress = 'z', UnCompress = 'd';
constexpr std::size_t maxBitNum = 8 * 2;
constexpr std::string_view fileSuffix = ".hzl";

[[noreturn]] void cerrExit(std::string_view info) {
	std::cerr << info << '\n';
	std::exit(EXIT_FAILURE);
}

void checkPath(fs::path p) {
	if (!fs::is_regular_file(p))
		cerrExit("源文件不存在");
}

template<typename C>
void checkStream(const std::basic_ios<C> &stream) {
	if (!stream)
		cerrExit("流异常");
}

bool interaction(std::string_view info, char a, char b) 
{
	std::cout << "  " << info << '\n';
	while (std::cin)
	{
		std::cout << "-> ";
		if (char ch = std::cin.get(); ch == a)
			return true;
		else if (ch == b)
			return false;
	}
	cerrExit("流异常");
}

void initFreq(std::istream &input, std::map<char_type, std::size_t> &freq)
{
	std::array<char, 1024> tmpBuf;
	while (std::streamsize size = input.readsome(tmpBuf.data(), 1024))
		for (std::streamsize i = 0; i < size; ++i)
			++freq[tmpBuf[i]];
}

void preprocessingPath(char flag, fs::path &src, fs::path &dest)
{
	if (flag == Compress)
	{
		if (src.extension() == fileSuffix)
			cerrExit("无法压缩该文件");
		dest.replace_extension(fileSuffix);
	}
	else if (src.extension() != fileSuffix)
		cerrExit("仅支持压缩.hzl文件");
	if (fs::exists(dest))
		if (interaction("目标文件已经存在是否覆盖? (y/n)", 'n', 'y'))
			exit(EXIT_SUCCESS);
}

int main(int argv, char *argc[])
{
	std::locale::global(std::locale(""));

	if (argv != 4 || (argc[3][0] != Compress && argc[3][0] != UnCompress))
		cerrExit("program -src -dest -z|d");
	fs::path src(argc[1]), dest(argc[2]);
	checkPath(src);

	char flag = argc[3][0];

	preprocessingPath(flag, src, dest);

	std::basic_ifstream<char_type> input(src, std::ios::binary);
	checkStream(input);

	std::basic_ofstream<char_type> output(dest, std::ios::binary);
	checkStream(output);

	if (flag == Compress)
	{
		std::map<char_type, std::size_t> freq;
		initFreq(input, freq);
		input.clear();
		input.seekg(0, std::ios::beg);
		HuffmanTree<char_type, maxBitNum> ht(freq);
		try {
			output << ht;
		} catch (std::runtime_error &e) {
			cerrExit(e.what());
		} catch (std::exception &e) {
			cerrExit("未知错误");
		}
		compress(ht, input, output);
		std::cout << "压缩前: " << ht.freqSum() << "(bytes)\n"
				  << "压缩后: " << output.tellp() << "(bytes)\n";
	}
	else
	{
		HuffmanTree<char_type, maxBitNum> ht;
		try {
			input >> ht;
			unCompress(ht, input, output);
		} catch (std::exception &e) {
			cerrExit(e.what());
		}
		std::cout << "解压后: " << output.tellp() << "(bytes)\n";
	}
}
