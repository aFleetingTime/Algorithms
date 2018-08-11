#pragma once
#include <array>
#include <iostream>
#include <filesystem>
#include <string_view>

using CharType = char;
using uIntType = unsigned;

constexpr char Compress = 'z', UnCompress = 'd';
constexpr std::string_view fileSuffix = ".hzl";

[[noreturn]] void cerrExit(std::string_view);

void checkPath(std::filesystem::path);

bool interaction(std::string_view info, char a, char b);

void preprocessingPath(char flag, std::filesystem::path &src, std::filesystem::path &dest);

template<typename C>
inline void checkStream(const std::basic_ios<C> &stream) {
	if (!stream) cerrExit("流异常");
}

template<typename Map, typename M>
inline auto convMap(const M &m) {
	return Map(m.cbegin(), m.cend());
}

template<typename Map>
auto countFreq(std::istream &input)
{
	std::array<char, 4096> tmpBuf;
	Map freq;
	while (std::streamsize size = input.readsome(tmpBuf.data(), 4096))
		for (std::streamsize i = 0; i < size; ++i)
			++freq[tmpBuf[i]];
	return freq;
}
