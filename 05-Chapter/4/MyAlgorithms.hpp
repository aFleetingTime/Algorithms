#include <algorithm>
#include <iostream>

void permutation(std::string s, std::string::size_type n = 0)
{
	if(n == s.size())
	{
		std::cout << s << std::endl;
		return;
	}
	for(std::string::size_type i = n; i < s.size(); ++i)
	{
		std::swap(s[i], s[n]);
		permutation(s, n + 1);
		std::swap(s[i], s[n]);
	}
}
