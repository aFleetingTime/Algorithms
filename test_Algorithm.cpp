#include <iterator>
#include <numeric>
#include <tuple>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/range.hpp>

namespace view = boost::range;

// 2.1-4 二进制数组相加
template<typename SrcType, typename DestType>
void binarySum(SrcType (&first), SrcType (&second), DestType (&dest))
{
	assert(std::size(first) == std::size(second) || std::size(first) < std::size(dest));
	std::fill(std::begin(dest), std::end(dest), 0);
	for(int i = std::size(first) - 1; i >= 0; --i)
	{
		auto [quot, rem] = std::div(first[i] + second[i] + dest[i + 1], 2);
		dest[i] = quot;
		dest[i + 1] = rem;
	}
}



int main()
{
	//std::vector<int> a1 { 100,113,110,85,105,102,86,63,81,101,94,106,101,79,94,90,97 };
	std::vector<int> a1 { 10, 6,7,7,2,11,8,10000, 11, 7, 10, 6, 999};
	//std::adjacent_difference(a1.cbegin(), a1.cend(), a1.begin());
	//a1[0] = 0;
	auto [i, j] = algo::findMaxSubArray(a1.begin(), a1.begin() + a1.size() / 2, a1.end());
	std::cout << *i << ' ' << *(j) << std::endl;
	std::cout << (i - a1.begin()) << ' ' << (j - a1.begin()) << std::endl;
}
