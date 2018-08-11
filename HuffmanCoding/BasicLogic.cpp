#include "BasicLogic.h"

[[noreturn]] void cerrExit(std::string_view info) {
	std::cerr << info << '\n';
	std::exit(EXIT_FAILURE);
}

void checkPath(std::filesystem::path p) {
	if (!std::filesystem::is_regular_file(p))
		cerrExit("源文件不存在");
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

void preprocessingPath(char flag, std::filesystem::path &src, std::filesystem::path &dest)
{
	if (flag == Compress)
	{
		if (src.extension() == fileSuffix)
			cerrExit("无法压缩该文件");
		dest.replace_extension(fileSuffix);
	}
	else if (src.extension() != fileSuffix)
		cerrExit("仅支持压缩.hzl文件");
	if (std::filesystem::exists(dest))
		if (interaction("目标文件已经存在是否覆盖? (y/n)", 'n', 'y'))
			exit(EXIT_SUCCESS);
}
