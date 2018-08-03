#include "columnSortBase.hpp"
#include <iomanip>
#include <fstream>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <cassert>
#include <functional>
#include <valarray>
#include <forward_list>
#include <numeric>
#include <thread>
#include <list>
#include <cmath>
#include <complex>
#include <chrono>
#include <random>
#include <type_traits>
#include <iterator>
#include <iostream>

namespace test {
// 编译阶段查看类型
template<typename> int type();

// 打印
template<typename Func, typename T, typename... Argv>
void print(const T& first, Func func = [](auto &v) { std::cout << v << ' '; }, const Argv&... argv)
{
	for (const auto &value : first)
		func(value);
	std::cout << '\n';
	if constexpr (sizeof...(Argv))	// 如果没有额外参数，结束递归。
		print(func, argv...);
}
template<typename T, typename... Argv>
void print(const T& first, const Argv&... argv)
{
	for (const auto &value : first)
		std::cout << value << ' ';
	std::cout << '\n';
	if constexpr (sizeof...(Argv))
		print(argv...);
}

// 以n行m列打印线性数组
template<typename T>
void printmatrix(const std::valarray<T> &va, std::size_t n, std::size_t m)
{
	for (std::size_t i = 0; i < n; ++i)
		test::print(std::valarray<T>(va[std::slice(i * m, m, 1)]));
	std::cout << '\n';
}

template<typename Func>
struct GetMemFuncArgs;
template<typename R, typename C, typename A>
struct GetMemFuncArgs<R(C::*)(A)> {
	using result_type = R;
	using parameter = std::remove_reference_t<std::remove_const_t<A>>;
};
// 产生随机数
template<typename Container, typename RandomEngine,
		 typename Return = void, typename Value = typename Container::const_reference>
void loadRandomNum(Container &cont, RandomEngine rand, std::size_t count, Return(Container::*op)(const Value&) = &Container::push_back) {
	while (count--) (cont.*op)(rand());
}

// 计时
template<typename Clock = std::chrono::steady_clock>
class Timer
{
	typename Clock::time_point tp;

public:
	void start() { tp = Clock::now(); }
	template<typename Duration = std::chrono::nanoseconds>
	Duration finish() { return std::chrono::duration_cast<Duration>(Clock::now() - tp); }
};

}

namespace algoBase {
template<typename value_type = unsigned int>
struct ValueIndex {
	value_type value; std::size_t index;
	ValueIndex() : value{}, index{} { }
	ValueIndex(value_type v, std::size_t i = 0) : value(v), index(i) { }
	operator value_type() { return value; }
	ValueIndex& operator++() { ++value; return *this; }
	ValueIndex& operator--() { --value; return *this; }
	ValueIndex operator++(int) { ValueIndex tmp = *this; ++value; return tmp; }
	ValueIndex operator--(int) { ValueIndex tmp = *this; --value; return tmp; }
	ValueIndex operator+=(const ValueIndex &rhs) { value += rhs.value; return *this; }
	ValueIndex operator+(const ValueIndex &rhs) { return ValueIndex(*this) += rhs; }
};
template<typename value_type>
bool operator<(const ValueIndex<value_type> &lhs, const ValueIndex<value_type> &rhs) { return lhs.value < rhs.value; }
template<typename value_type>
bool operator<(const ValueIndex<value_type> &lhs, value_type rhs) { return lhs.value < rhs; }
template<typename value_type>
bool operator<(value_type lhs, const ValueIndex<value_type> &rhs) { return lhs < rhs.value; }

inline unsigned int getBit(std::uintmax_t num, std::size_t i) {
	if (i == 0)
		return num % 10;
	return num % static_cast<std::uintmax_t>(std::pow(10, i + 1)) / static_cast<std::uintmax_t>(std::pow(10, i));
}

inline constexpr unsigned int numDigit(std::uintmax_t num) {
	return std::log10(num) + 1;
}

template<typename InputIt>
std::tuple<InputIt, InputIt, typename std::iterator_traits<InputIt>::value_type> maxSubArray(InputIt low, InputIt mid, InputIt high)
{
	using value_type = typename std::iterator_traits<InputIt>::value_type;

	InputIt leftMax(std::prev(mid));
	value_type leftSum = std::numeric_limits<value_type>::lowest(), sum{};
	for (std::reverse_iterator beg(mid), end(low); beg != end; std::advance(beg, 1))
	{
		sum = sum + *beg;
		if (leftSum < sum)
		{
			leftMax = std::prev(beg.base());
			leftSum = sum;
		}
	}

	InputIt rightMax(mid);
	value_type rightSum = std::numeric_limits<value_type>::lowest();
	sum = value_type{};
	for (InputIt beg = mid; beg != high; std::advance(beg, 1))
	{
		sum = sum + *beg;
		if (rightSum < sum)
		{
			rightMax = beg;
			rightSum = sum;
		}
	}

	return { leftMax, rightMax, leftSum + rightSum };
}
}

namespace traits {
	template<typename, typename, class = void>
	struct can_divide : std::false_type { };
	template<typename Type1, typename Type2>
	struct can_divide<Type1, Type2, std::void_t<decltype(std::declval<Type2&>() / std::declval<Type2&>())>> : std::true_type { };
}

namespace algo {

// 活动选择问题(贪心方法) - O(n)
template<typename InputIt>
auto activitySelector(InputIt s_first, InputIt s_last, InputIt f_first, InputIt f_last)
{
	std::vector<std::pair<InputIt, InputIt>> res;
	if (s_first == s_last) return res;
	std::size_t count = 0;
	res.push_back({ s_first, f_first });
	InputIt max = f_first;
	while (++f_first != f_last)
	{
		if (*max < *++s_first)
		{
			max = f_first;
			res.push_back({ s_first, f_first });
		}
	}
	return res;
}

// 找零问题 - O(n lg n)
template<typename InputIt, typename T = typename std::iterator_traits<InputIt>::value_type>
auto change(T val, InputIt first, InputIt last)
{
	static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);
	std::vector<typename std::iterator_traits<InputIt>::value_type> deno(first, last), dest;
	std::sort(deno.begin(), deno.end(), std::greater<typename decltype(deno)::value_type>());
	for (auto beg = deno.cbegin(); beg != deno.cend(); ++beg)
		if (*beg <= val)
		{
			if (*beg == 0) throw std::invalid_argument("");
			dest.resize(dest.size() + val / *beg, *beg);
			val = val % *beg;
		}
	return std::pair(dest, val);
}

// 赫夫曼编码 - O(n lg n)
template<typename InputIt>
auto huffman(InputIt first, InputIt last)
{
	using value_type = typename std::iterator_traits<InputIt>::value_type::first_type;
	using freq_type = typename std::iterator_traits<InputIt>::value_type::second_type;
	static_assert(std::is_integral_v<freq_type>);
	struct Node {
		Node &left, &right;
		freq_type freq;
		value_type value;
		explicit Node(Node *l, Node *r, freq_type f, const value_type &v = value_type{}) : left(*l), right(*r), freq(f), value(v) { }
		static bool comp(const Node *lhs, const Node *rhs) { return lhs->freq > rhs->freq; };
		static void free(const Node &root)
		{
			if (&root.left) freeBase(root.left);
			if (&root.right) freeBase(root.right);
		}
	private:
		static void freeBase(const Node &root)
		{
			if (&root.left) freeBase(root.left);
			if (&root.right) freeBase(root.right);
			delete &root;
		}
	};
	auto size = std::distance(first, last);
	if (!size) return Node(nullptr, nullptr, 0);
	std::priority_queue<Node*, std::vector<Node*>, decltype(Node::comp)*> que(Node::comp);
	for (InputIt cur = first; cur != last; ++cur)
		que.push(new Node(nullptr, nullptr, cur->second, cur->second));
	while (que.size() > 1)
	{
		Node *x = que.top();
		que.pop();
		Node *y = que.top();
		que.pop();
		que.push(new Node(x, y, x->freq + y->freq));
	}
	Node root = *que.top();
	delete que.top();
	return root;
}

// 分数背包问题 - O(n)
template <typename InputIt1, typename InputIt2>
std::size_t fractionalKnapsack(InputIt1 pb, InputIt1 pe, InputIt2 wb, InputIt2 we, std::size_t n)
{
	using type1 = typename std::iterator_traits<InputIt1>::value_type;
	using type2 = typename std::iterator_traits<InputIt2>::value_type;
	static_assert(traits::can_divide<type1, type2>::value);
	auto priceNum = std::distance(pb, pe);
	if (priceNum != std::distance(wb, we)) throw std::invalid_argument("");
	std::vector<std::pair<type1, type2>> prices;
	for (auto pbeg = pb, wbeg = wb; pbeg != pe; ++pbeg, ++wbeg)
		prices.emplace_back(*pbeg / *wbeg, *wbeg);
	std::sort(prices.begin(), prices.end(), [](auto &lhs, auto &rhs) { return lhs.first > rhs.first; });
	std::size_t result = 0;
	for (auto &val : prices)
		if (n <= val.second)
		{
			result += val.first * n;
			break;
		}
		else
		{
			result += val.first * val.second;
			n -= val.second;
		}
	return result;
}

// 01背包问题
template<typename InputIt>
auto zeroOneKnapsack(InputIt price, InputIt pend, InputIt weight, const typename std::iterator_traits<InputIt>::value_type &capacity)
{
	std::vector<std::vector<std::size_t>> dp(std::distance(price, pend) + 1), sz(dp.size());
	for (auto &v : dp)
		v.resize(capacity + 1);
	for (std::size_t i = 0; i < dp.size() - 1; ++i)
		for (std::size_t j = 1; j <= capacity; ++j)
			if (j >= weight[i])
				dp[i + 1][j] = std::max(dp[i][j - weight[i]] + price[i], dp[i][j]);
			else dp[i + 1][j] = dp[i][j];
	return dp;
}

// 最优二叉搜索树 - O(n^3)
template<typename InputIt>
auto optimalBSTBase(InputIt pb, InputIt pe, InputIt qb, InputIt qe)
{
	struct MyHash {
		std::size_t operator()(const std::pair<std::size_t, std::size_t> &val) const noexcept {
			return std::hash<std::size_t>()(val.first) ^ std::hash<std::size_t>()(val.second);
		}
	};
	auto psize = std::distance(pb, pe), qsize = std::distance(qb, qe);
	if (psize != qsize - 1) throw std::invalid_argument("");
	std::unordered_map<std::pair<std::size_t, std::size_t>, double, MyHash> e, w;
	std::unordered_map<std::pair<std::size_t, std::size_t>, std::size_t, MyHash> root;
	for (int i = 0; i <= psize; ++i)
		root[{i + 1, i}] = i + 1;
	for (std::size_t i = 1; i <= qsize; ++i)
		e[{i, i - 1}] = w[{i, i - 1}] = qb[i - 1];
	for (std::size_t l = 0; l < psize; ++l)
	{
		for (std::size_t i = 1; i <= psize - l; ++i)
		{
			std::size_t j = i + l;
			e[{i, j}] = std::numeric_limits<double>::max();
			w[{i, j}] = w[{i, j - 1}] + *std::next(pb, j - 1) + *std::next(qb, j);
			for (std::size_t r = root[{i, j - 1}], len = root[{i + 1, j}]; r <= len; ++r)
			{
				double temp = e[{i, r - 1}] + e[{r + 1, j}] + w[{i, j}];
				if (temp < e[{i, j}])
				{
					e[{i, j}] = temp;
					root[{i, j}] = r;
				}
			}
		}
	}
	return root;
}
template<typename T>
void printOptimalBST(T &root, std::size_t low, std::size_t high, short flag)
{
	if (low <= high)
		std::cout << "(k" << root[{low, high}] << ')';
	else std::cout << "(d" << high << ')';
	if (flag == 0)
		std::cout << "是root" << std::endl;
	else if (flag == -1)
		std::cout << "是(k" << (high + 1) << ")的左孩子" << std::endl;
	else
		std::cout << "是(k" << (low - 1) << ")的右孩子" << std::endl;
	if (low > high)
		return;
	printOptimalBST(root, low, root[{low, high}] - 1, -1);
	printOptimalBST(root, root[{low, high}] + 1, high, 1);
}
template<typename InputIt>
void optimalBST(InputIt pb, InputIt pe, InputIt qb, InputIt qe)
{
	auto root = optimalBSTBase(pb, pe, qb, qe);
	printOptimalBST(root, 1, std::distance(pb, pe), 0);
}

// 最长公共子序列 - O(mn)
template<typename InputIt, typename Compare = std::equal_to<typename std::iterator_traits<InputIt>::value_type>>
std::vector<std::vector<std::size_t>> LCSlengthSolution(InputIt xb, InputIt xe, InputIt yb, InputIt ye, Compare comp = {})
{
	auto xsize = std::distance(xb, xe), ysize = std::distance(yb, ye);

	std::vector<std::vector<std::size_t>> lengths(xsize + 1);
	for (auto &c : lengths)
		c.resize(ysize + 1);

	for (InputIt i = xb; i != xe; ++i)
	{
		auto idis = std::distance(xb, i) + 1;
		for (InputIt j = yb; j != ye; ++j)
		{
			auto jdis = std::distance(yb, j) + 1;
			if (comp(*i, *j))
				lengths[idis][jdis] = lengths[idis - 1][jdis - 1] + 1;
			else if (lengths[idis - 1][jdis] < lengths[idis][jdis - 1])
				lengths[idis][jdis] = lengths[idis][jdis - 1];
			else lengths[idis][jdis] = lengths[idis - 1][jdis];
		}
	}
	return lengths;
}
template<typename InputIt, typename OutputIt, typename Compare>
void createLCS(const std::vector<std::vector<std::size_t>> &s, InputIt xb, InputIt xe, InputIt yb, InputIt ye, OutputIt dest, const Compare &comp)
{
	auto xlen = std::distance(xb, xe) + 1, ylen = std::distance(yb, ye) + 1;
	if (xlen == 0 || ylen == 0)
		return;
	if (comp(*xe, *ye))
	{
		createLCS(s, xb, std::prev(xe), yb, std::prev(ye), std::prev(dest), comp);
		*dest = *xe;
	}
	else if (s[xlen - 1][ylen] < s[xlen][ylen - 1])
		createLCS(s, xb, xe, yb, std::prev(ye), dest, comp);
	else createLCS(s, xb, std::prev(xe), yb, ye, dest, comp);
}
template<typename InputIt, typename Compare = std::equal_to<typename std::iterator_traits<InputIt>::value_type>>
auto LCSlength(InputIt xb, InputIt xe, InputIt yb, InputIt ye, Compare comp = {})
{
	using value_type = typename std::iterator_traits<InputIt>::value_type;
	auto lengths = LCSlengthSolution(xb, xe, yb, ye, comp);
	std::vector<value_type> res(lengths.back().back());
	createLCS(lengths, xb, std::prev(xe), yb, std::prev(ye), --res.end(), comp);
	return res;
}
// 最长公共子序列长度
template<typename InputIt, typename Compare = std::equal_to<typename std::iterator_traits<InputIt>::value_type>>
std::size_t LCS(InputIt xb, InputIt xe, InputIt yb, InputIt ye, Compare comp = {})
{
	auto minlen = std::distance(yb, ye), maxlen = std::distance(xb, xe);
	if (!minlen || !maxlen)
		return std::max(minlen, maxlen);
	if (maxlen < minlen)
	{
		std::swap(maxlen, minlen);
		std::swap(xb, yb);
		std::swap(xe, ye);
	}
	std::vector<std::size_t> dp(minlen + 1);

	for (InputIt ibeg = xb; ibeg != xe; ++ibeg)
	{
		std::size_t prev = 0;
		for (InputIt jbeg = yb; jbeg != ye; ++jbeg)
		{
			auto len = std::distance(yb, jbeg) + 1;
			if (comp(*ibeg, *jbeg))
				dp[len] = prev + 1;
			else if (dp[len] < dp[len - 1])
				prev = std::exchange(dp[len], dp[len - 1]);
			else prev = dp[len];
		}
	}

	return dp.back();
}

// 矩阵链乘法问题(打印最优括号化方案)
template<typename Map>
std::ostream& printMatrixChainMultiplySolution(std::ostream &os, Map &s, std::size_t i, std::size_t j)
{
	if (i + 1 >= j)
		os << ("A" + std::to_string(i));
	else
	{
		os << '(';
		printMatrixMultiplySolution(os, s, i, s[{i, j - 1}]);
		printMatrixMultiplySolution(os, s, s[{i, j - 1}], j);
		os << ')';
	}
	return os;
}
template<typename Map>
std::ostream& printMatrixChainMultiply(std::ostream &os, Map &s, std::size_t size) {
	return printMatrixMultiplySolution(os, s, 0, size);
}
// 矩阵链乘法问题(自底向上)
template<typename InputIt>
auto matrixChainMultiply(InputIt chain, InputIt last)
{
	struct MyHash {
		std::size_t operator()(const std::pair<std::size_t, std::size_t> &val) const noexcept {
			return std::hash<std::size_t>()(val.first) ^ std::hash<std::size_t>()(val.second);
		}
	};
	auto size = std::distance(chain, last) - 1;
	std::unordered_map<std::pair<std::size_t, std::size_t>, std::size_t, MyHash> results, indexes;
	for (std::size_t i = 1; i < size; ++i)
	{
		for (std::size_t j = 0; j < size - i; ++j)
		{
			std::size_t l = j + i;
			results[{j, l}] = std::numeric_limits<std::size_t>::max();
			for (std::size_t k = j; k < l; ++k)
			{
				auto current = results[{j, k}] + results[{k + 1, l}] + chain[j] * chain[k + 1] * chain[l + 1];
				if (current < results[{j, l}])
				{
					indexes[{j, l}] = k + 1;
					results[{j, l}] = current;
				}
			}
		}
	}
	return std::pair<decltype(results), decltype(indexes)>{results, indexes};
}

// 矩阵链乘法问题(带备忘的自顶向下)
template<typename InputIt, typename Map>
std::size_t recMatrixChainMultiplySolution(Map &results, Map &indexes, InputIt p, std::size_t i, std::size_t j)
{
	if (i + 1 >= j)
		return results[{i, i}] = 0;
	if (auto temp = results.find({i, j - 1}); temp != results.end())
		return temp->second;
	results[{i, j - 1}] = std::numeric_limits<std::size_t>::max();
	for (std::size_t k = i + 1; k < j; ++k)
	{
		std::size_t currentValue = recMatrixChainMultiplySolution(results, indexes, p, i, k) + recMatrixChainMultiplySolution(results, indexes, p, k, j) + p[i] * p[k] * p[j];
		if (currentValue < results[{i, j - 1}])
		{
			results[{i, j - 1}] = currentValue;
			indexes[{i, j - 1}] = k;
		}
	}
	return results[{i, j - 1}];
}
template<typename InputIt>
auto recMatrixChainMultiply(InputIt beg, InputIt end)
{
	struct MyHash {
		std::size_t operator()(const std::pair<std::size_t, std::size_t> &val) const noexcept {
			return std::hash<std::size_t>()(val.first) ^ std::hash<std::size_t>()(val.second);
		}
	};
	std::unordered_map<std::pair<std::size_t, std::size_t>, std::size_t, MyHash> results, indexes;
	std::size_t sz = std::distance(beg, end) - 1;
	recMatrixChainMultiplySolution(results, indexes, beg, 0, sz);
	return std::pair(results, indexes);
}

// 钢条切割问题 - O(n^2)
template<typename InputIt>
std::vector<typename std::iterator_traits<InputIt>::value_type> baseCutRod(InputIt prices, std::intmax_t n, std::intmax_t c)
{
	using integer = typename std::iterator_traits<InputIt>::value_type;
	std::vector<integer> results(n), lengths(n), counts(n);
	for (std::size_t i = 0; i < n; ++i)
	{
		results[i] = *std::next(prices, i);
		lengths[i] = i + 1;
		for (std::size_t j = 1; j <= i; ++j)
			if (integer price = *std::next(prices, j - 1) + results[i - j] - c - counts[i - j] * c; results[i] < price)
			{
				lengths[i] = j;
				results[i] = price;
				counts[i] = counts[i - j] + 1;
			}
	}
	return lengths;
}
template<typename InputIt, typename OutputIt>
void cutRod(InputIt prices, std::intmax_t n, std::intmax_t c, OutputIt dest)
{
	static_assert(std::is_integral_v<typename std::iterator_traits<InputIt>::value_type>);
	auto result = baseCutRod(prices, n, c);
	while (n > 0)
	{
		*dest++ = result[n - 1];
		n -= result[n - 1];
	}
}

// 约瑟夫问题(链表实现) - O(n)
template<typename LinkedList>
void josephus(LinkedList &list, LinkedList &dest, std::size_t m)
{
	if (m == 0) throw std::logic_error("logic error");
	std::size_t current = (m - 1) % list.size();
	while (true)
	{
		dest.splice(dest.end(), list, std::next(list.begin(), current));
		if (list.empty()) return;
		current = (current + m - 1) % list.size();
	}
}

// 搜索第二小元素
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
InputIt findSecond(InputIt first, InputIt last, Compare comp = {})
{
	if (std::distance(first, last) <= 1)
		return first;
	InputIt old(first), min(std::next(first));
	if (comp(*old, *min))
		std::swap(old, min);
	std::advance(first, 2);
	while (first != last)
	{
		if (comp(*first, *min))
		{
			old = min;
			min = first;
		}
		else if (comp(*first, *old) && comp(*min, *first))
			old = first;
		++first;
	}
	return old;
}

// 随机划分
template<typename InputIt, typename RandomEngine = std::default_random_engine, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
InputIt randomPartition(InputIt first, InputIt last, RandomEngine engine = RandomEngine{std::random_device()()}, Compare comp = {})
{
	if (first == last)
		return first;
	std::iter_swap(std::prev(last), std::next(first, std::uniform_int_distribution<>(0, std::distance(first, last) - 1)(engine)));
	InputIt lastPrev(std::prev(last));
	const typename std::iterator_traits<InputIt>::value_type &key = *lastPrev;
	std::size_t count = 0;
	for (InputIt current = first; current != lastPrev; ++current)
		if (*current == key)
			++count;
		else if (comp(*current, key))
			std::iter_swap(first++, current);
	std::iter_swap(first, lastPrev);
	return (count == std::distance(first, lastPrev)) ? std::prev(lastPrev, (count + 1) / 2) : first;
}

// 划分
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
InputIt partition(InputIt first, InputIt last, Compare comp = {})
{
	InputIt lastPrev(std::prev(last));
	const typename std::iterator_traits<InputIt>::value_type &key = *lastPrev;
	std::size_t count = 0, size = std::distance(first, lastPrev);
	for (InputIt current = first; current != lastPrev; ++current)
	{
		if (comp(key, *current)) continue;
		if (!comp(*current, key))
			++count;
		std::iter_swap(first++, current);
	}
	std::iter_swap(first, lastPrev);
	return (count == size) ? std::prev(lastPrev, count / 2) : first;
}

// 搜索第i小元素
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
InputIt randomSelect(InputIt first, InputIt last, std::size_t i, Compare comp = {})
{
	if (std::distance(first, last) < i + 1) return last;
	while (std::distance(first, last) > 1)
	{
		InputIt mid = algo::randomPartition(first, last, std::default_random_engine(std::random_device()()), comp);
		if (auto k = std::distance(first, mid); k == i)
			return mid;
		else if (i < k)
			last = mid;
		else
		{
			first = std::next(mid);
			i -= k + 1;
		}
	}
	return first;
}

// 搜索第i小元素(递归)
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
InputIt recRandomSelect(InputIt first, InputIt last, std::size_t i, Compare comp = {})
{
	if (std::distance(first, last) <= 1)
		return first;
	InputIt mid = algo::randomPartition(first, last, std::default_random_engine(std::random_device()()), comp);
	if (auto k = std::distance(first, mid); k == i)
		return mid;
	else if (i < k)
		return recRandomSelect(first, mid, i, comp);
	else return recRandomSelect(std::next(mid), last, i - k - 1, comp);
}

// 搜索第i小元素(递归)
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
InputIt selectBase(std::vector<InputIt> &&vec, Compare comp = {})
{
	if (vec.empty()) return {};
	std::vector<InputIt> tmp;
	typename std::vector<InputIt>::iterator first = vec.begin(), last = vec.end(), old;
	for (std::size_t i = 0, c = vec.size() / 5; i < c; ++i)
	{
		old = first;
		std::sort(old, first += 5, [comp](auto a, auto b) { return comp(*a, *b); });
		tmp.push_back(*std::next(old, 2));
	}
	if (vec.size() % 5)
	{
		std::sort(first, last, [comp](auto a, auto b) { return comp(*a, *b); });
		tmp.push_back(*(first + (std::distance(first, last) - 1) / 2));
	}
	return tmp.size() == 1 ? vec.front() : selectBase(std::move(tmp), comp);
}

template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
InputIt select(InputIt first, InputIt last, std::size_t i, Compare comp = {})
{
	using value_type = typename std::iterator_traits<InputIt>::value_type;
	std::size_t size = std::distance(first, last);
	if (size <= 1)
		return first;
	std::vector<InputIt> vec(std::distance(first, last));
	std::iota(vec.begin(), vec.end(), first);
	std::iter_swap(std::prev(last), selectBase(std::move(vec), comp));
	InputIt mid = algo::partition(first, last, comp);
	if (auto k = std::distance(first, mid); k == i)
		return mid;
	else if (i < k)
		return select(first, mid, i, comp);
	else return select(mid + 1, last, i - k - 1, comp);
}

#if 0
// K分位数
template<typename InputIt, typename OutputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
InputIt findQuantile(InputIt first, InputIt last, OutputIt dest, std::size_t k, Compare comp = Compare{});
#endif

// 划分
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>, typename Equal = std::equal_to<typename std::iterator_traits<InputIt>::value_type>>
std::pair<InputIt, InputIt> equalPartition(InputIt first, InputIt last, Compare comp = {}, Equal equal= {})
{
	InputIt end = last, beg = first;
	const typename std::iterator_traits<InputIt>::value_type &key = *std::prev(last);
	while (first < last)
	{
		if (comp(key, *first))
			std::iter_swap(first, --end);
		else if (comp(*first, key))
			std::iter_swap(beg++, first++);
		else ++first;
	}
	return { beg, first };
}

// Hoare划分
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
InputIt hoarePartition(InputIt first, InputIt last, Compare comp = {})
{
	const typename std::iterator_traits<InputIt>::value_type key = *first;
	--last;
	while (true)
	{
		while (key < *last) --last;//comp(key, *last)) --last;
		while (*first < key) ++first;//comp(*first, key)) ++first;
		if (first < last)
			std::iter_swap(first, last);
		else return last;
	}
}

// 划分(一元谓词版本)
template<typename InputIt, typename UnaryPredicate>
InputIt partitionPred(InputIt first, InputIt last, UnaryPredicate pred)
{
	first = std::find_if_not(first, last, pred);
	if (first == last)
		return first;
	for (InputIt current = std::next(first); current != last; ++current)
		if (pred(*current))
			std::iter_swap(first++, current);
	return first;
}

// 8.2-4
template<typename InputIt>
std::size_t rangeCount(InputIt first, InputIt last, std::size_t min, std::size_t max)
{
	using integral_t = typename std::iterator_traits<InputIt>::value_type;
	std::vector<integral_t> counts(max - min + 1);
	while (first != last)
	{
		if (*first <= max && min <= *first)
			++counts[*first - min];
		++first;
	}
	return std::accumulate(counts.cbegin(), counts.cend(), std::size_t{});
}

template<typename InputIt>
std::size_t rangeCount2(InputIt first, InputIt last, std::size_t min, std::size_t max)
{
	using integral_t = typename std::iterator_traits<InputIt>::value_type;
	std::vector<integral_t> counts(*std::max_element(first, last) + 1);
	while (first != last)
		++counts[*first++];
	std::partial_sum(counts.cbegin(), counts.cend(), counts.begin());
	return min ? counts[max] - counts[min - 1] : counts[max];
}

}

namespace sort {
/* 
 * Sort Algorihm: 直接插入排序
 * 渐近时间复杂度: O(n^2)
 */
template<typename T, typename CompareType = std::less<typename std::iterator_traits<T>::value_type>>
void insertSort(T first, T last, CompareType comp = CompareType{})
{

	T tmp = first;
	while (std::distance(tmp, last) > 1 && comp(*tmp, *std::next(tmp)))
		++tmp;
	for (T current = tmp; current != last; ++current)	// 循环不变式: 每次循环first到current的区间都是有序的。
	{
		auto key = *current;
		T prev = std::prev(current);
		while (std::prev(first) != prev && comp(key, *prev))	// 若用prev与key调用的comp为真，将prev位置的元素后移一个位置。
		{
			*std::next(prev) = *prev;
			--prev;
		}
		*std::next(prev) = key;		// 循环结束后，prev必定在可正确插入的位置-1处。
	}
}

/* 
 * Sort Algorithm: 选择排序
 * 渐近时间复杂度: O(n^2)
 */
template<typename T, typename CompareType = std::less<typename std::iterator_traits<T>::value_type>>
void selectSort(T first, T last, CompareType comp = CompareType{})
{
	T index;
	while(first != std::prev(last))	// 循环不变式: 每次循环执行swap后，first当前位置及之前的元素都是有序的。
	{
		index = first;
		for(auto beg = std::next(first); beg != last; ++beg)
			if(comp(*beg, *index))
				index = beg;
		std::swap(*(first++), *index);
	}
}

/*
 * Merge: 归并
 * 渐进时间复杂度: O(n)
 */
template<typename InputIt1, typename InputIt2, typename OutputIt, typename Compare = std::less<typename std::iterator_traits<InputIt1>::value_type>>
OutputIt merge(InputIt1 fst1, InputIt1 lst1, InputIt2 fst2, InputIt2 lst2, OutputIt dest, Compare comp = {})
{
	for (;fst1 != lst1; ++dest)
	{
		if (fst2 == lst2)
			return std::copy(fst1, lst1, dest);
		if (comp(*fst2, *fst1))
			*dest = *fst2++;
		else
			*dest = *fst1++;
	}
	return std::copy(fst2, lst2, dest);
}

/* 
 * Sort Algorithm: 递归归并排序
 * 渐近时间复杂度: O(n lg n)
 */
template<typename T, typename Compare = std::less<typename std::iterator_traits<T>::value_type>>
void recMergeSort(T first, T last, Compare comp = {})
{
	if (auto dis = std::distance(first, last); dis > 1)
	{
		T mid = first;
		std::advance(mid, dis / 2);
		recMergeSort(first, mid, comp);
		recMergeSort(mid, last, comp);
		std::vector<typename std::iterator_traits<T>::value_type> tmp(dis);
		sort::merge(first, mid, mid, last, tmp.begin(), comp);
		std::copy(tmp.cbegin(), tmp.cend(), first);
	}
}

// 快速排序随机化版本
// 相比与普通版本，划分更为均衡，即使输入数据处于基本有序，其运行时间依然为O(n lg n)（普通版本运行时间为O(n^2)）
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
void recQuickSort(InputIt first, InputIt last, Compare comp = {})
{
	static std::uniform_int_distribution<typename std::iterator_traits<InputIt>::difference_type> dist;
	static std::default_random_engine eng;
	if (first == last)
		return;
	if (std::distance(first, last) <= 16)
		insertSort(first, last, comp);
	else
	{
		dist.param(typename decltype(dist)::param_type{ 0, std::distance(first, last) - 1 });
		std::iter_swap(std::prev(last), std::next(first, dist(eng)));
		InputIt mid;
		do {
			mid = algo::partition(first, last, comp);
			recQuickSort(first, mid);
			first = mid;
		} while (++first != last);
	}
}

// Hoare划分快速排序
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
void recHoareQuickSort(InputIt first, InputIt last, Compare comp = {})
{
	if (first >= last)
		return;
	InputIt mid = algo::hoarePartition(first, last, comp);
	recHoareQuickSort(first, mid);
	recHoareQuickSort(std::next(mid), last);
}

// 计数排序 - O(n)
template<typename InputIt>
std::vector<typename std::iterator_traits<InputIt>::value_type> countingSort(InputIt first, InputIt last, std::size_t maxRange)
{
	using integral_t = typename InputIt::value_type;

	std::vector<integral_t> counts(maxRange + 1);
	for (InputIt beg = first; beg != last; ++beg)
		++counts[*beg];

	std::partial_sum(counts.cbegin(), counts.cend(), counts.begin());

	std::vector<integral_t> result(std::distance(first, last));

	for (InputIt beg(std::prev(last)), end(std::prev(first)); beg != end; --beg)	// 相等元素是按逆序存入输出数组的，因此为保持稳定性，以逆序遍历输入数组元素，这样可以使输入数组中靠后的相同元素也在输出数组中位于靠后位置。
		result[--counts[*beg]] = *beg;
	return result;
}
template<typename InputIt>
std::vector<typename std::iterator_traits<InputIt>::value_type> autoCountingSort(InputIt first, InputIt last) {
	return countingSort(first, last, *std::max_element(first, last));
}

// 指定闭区间最小值计数排序
template<typename InputIt, typename OutputIt>
OutputIt rangeCountingSort(InputIt first, InputIt last, typename InputIt::value_type min, typename InputIt::value_type max, OutputIt dest)
{
	using integral_t = typename InputIt::value_type;
	static_assert(std::is_unsigned_v<integral_t>);

	std::vector<integral_t> counts(max - min + 1);
	for (InputIt beg = first; beg != last; ++beg)
		++counts[*beg - min];
	std::partial_sum(counts.cbegin(), counts.cend(), counts.begin());

	for (InputIt beg(std::prev(last)), end(std::prev(first)); beg != end; --beg)
		*std::next(dest, --counts[*beg - min]) = *beg;
	return std::next(dest, std::distance(first, last));
}

// 计数排序(原址排序) - O(n)
template<typename InputIt>
void inplaceCountingSort(InputIt first, InputIt last, typename std::iterator_traits<InputIt>::value_type maxRange)
{
	using integral_t = typename std::iterator_traits<InputIt>::value_type;

	std::vector<integral_t> counts(maxRange + 1);
	for (InputIt beg = first; beg != last; ++beg)
		++counts[*beg];
	std::partial_sum(counts.cbegin(), counts.cend(), counts.begin());
	std::vector<integral_t> indexes(counts.size());

	InputIt current = first;
	for (std::size_t i = 0, sz = last - first; i < sz; ++i, ++current)
	{
		std::size_t curIndex = std::distance(first, current);
		while (curIndex >= counts[*current] || (*current ? curIndex < counts[*current - 1] : false))
			std::iter_swap(current, std::next(first, counts[*current] - ++indexes[*current]));
	}
}

// 桶排序 - O(n)
template<typename InputIt, typename OutputIt>
void bucketSort(InputIt first, InputIt last, typename InputIt::value_type bucketSize, OutputIt dest)
{
	using value_type = typename InputIt::value_type;
	std::vector<std::vector<value_type>> buckets(bucketSize + 1);

	for (;first != last; ++first)
		buckets[*first / 10].emplace_back(*first);

	for (std::size_t i = 0; i < buckets.size(); ++i)
		dest = rangeCountingSort(buckets[i].cbegin(), buckets[i].cend(), i * 10, i * 10 + 9, dest);
}

// 桶排序 - O(n)
template<typename InputIt, typename OutputIt>
void bucketInsertSort(InputIt first, InputIt last, typename InputIt::value_type bucketSize, OutputIt dest)
{
	using value_type = typename InputIt::value_type;
	std::vector<std::vector<value_type>> buckets(bucketSize + 1);

	for (;first != last; ++first)
		buckets[*first / 10].emplace_back(*first);

	for (auto &bucket : buckets)
	{
		insertSort(bucket.begin(), bucket.end());
		dest = std::copy(bucket.cbegin(), bucket.cend(), dest);
	}
}

// 整型基数排序 - O(n)
template<typename InputIt>
void integerRadixSort(InputIt first, InputIt last)
{
	using value_type = typename std::iterator_traits<InputIt>::value_type;
	static_assert(std::is_unsigned_v<value_type>);

	std::size_t digit = 0;
	for (auto beg = first; beg != last; ++beg)
		if (auto ret = algoBase::numDigit(*beg); digit < ret)
			digit = ret;
	for (std::size_t i = 0; i < digit; ++i)
	{
#if 0
		std::vector<algoBase::ValueIndex<value_type>> vec;
		std::size_t index = 0;
		for (InputIt beg = first; beg != last; ++beg, ++index)
			vec.emplace_back(algoBase::getBit(*beg, i), index);
		vec = countingSort(vec.begin(), vec.end(), std::size_t(9));
		index = 0;
		std::vector<value_type> temp(first, last);
		for (InputIt beg = first; beg != last; ++beg, ++index)
			*beg = temp[vec[index].index];
#endif
		std::vector<std::size_t> counts(10);
		for (InputIt beg = first; beg != last; ++beg)
			++counts[algoBase::getBit(*beg, i)];
		std::partial_sum(counts.cbegin(), counts.cend(), counts.begin());
		std::vector<value_type> temp(std::distance(first, last));
		for (InputIt beg = first; beg != last; ++beg)
			temp[--counts[algoBase::getBit(*beg, i)]] = *beg;
		std::copy(temp.cbegin(), temp.cend(), first);
	}
}
// 整型数组基数排序 - O(n)
template<template<typename> typename Container, typename Array>
void arrayRadixSort(Container<Array> &cont, std::size_t min, std::size_t max, std::size_t base)
{
	for (std::size_t i = 0; i < base; ++i)
	{
		std::vector<std::size_t> counts(max - min + 1);
		for (auto beg = cont.cbegin(); beg != cont.cend(); ++beg)
			++counts[std::size(*beg) <= i ? 0 : (*beg)[std::size(*beg) - i - 1] - min];
		std::partial_sum(counts.cbegin(), counts.cend(), counts.begin());
		std::size_t index = 0;
		Container<Array> temp(cont.size());
		for (auto beg(std::prev(cont.end())), end(std::prev(cont.begin())); beg != end; --beg, ++index)
			temp[--counts[std::size(*beg) <= i ? 0 : (*beg)[std::size(*beg) - i - 1] - min]] = std::move(*beg);
		cont = std::move(temp);
	}
}


// 列排序
template<typename value_type>
void columnSort(std::valarray<value_type> &ary, std::size_t r, std::size_t s)
{
	// s必须是r的因子
	assert(r % s == 0);

	// 第1步
	columnSortBase::subColumnSort(ary, r, s);
	// 第2步
	columnSortBase::transpose(ary, r, s);
	// 第3步
	columnSortBase::subColumnSort(ary, r, s);
	// 第4步
	columnSortBase::reverseTranspose(ary, r, s);
	// 第5步
	columnSortBase::subColumnSort(ary, r, s);
	// 第6步
	ary = columnSortBase::matrixPermutation(ary, r, s);
	// 第7步
	{ // 排序两侧列
		std::valarray<value_type> left = ary[std::slice(r / 2 * s, r / 2, s + 1)], right = ary[std::slice(s + 1, r / 2, s + 1)];
		std::sort(std::begin(left), std::end(left)), std::sort(std::begin(right), std::end(right));
		ary[std::slice(r / 2 * s, r / 2, s + 1)] = left, ary[std::slice(s + 1, r / 2, s + 1)] = right;
	}
	{ // 排序剩余的列
		std::valarray<value_type> tmp(ary[std::gslice(1, { r, s - 1 }, { s + 1, 1 })]);
		columnSortBase::subColumnSort(tmp, r, s - 1);
		ary[std::gslice(1, { r, s - 1 }, { s + 1, 1 })] = tmp;
	}
	// 第8步
	ary = columnSortBase::reserveMatrixPermutation(ary, r, s);
	columnSortBase::subColumnSort(ary, r, s);
}

}

namespace algo {

// 最小值最大值
template<typename InputIt, typename Compare = std::less<typename std::iterator_traits<InputIt>::value_type>>
std::pair<InputIt, InputIt> minmax(InputIt beg, InputIt end, Compare comp = Compare{})
{
	InputIt min, max;
	if (auto sz = std::distance(beg, end); sz == 0)
		return { beg, end };
	else if (sz & 1)
		min = max = beg;
	else
	{
		min = beg;
		max = ++beg;
		if (comp(*max, *min)) std::swap(min, max);
	}
	InputIt first;
	while (++beg != end)
	{
		first = beg++;
		if (comp(*beg, *first))
		{
			if (comp(*beg, *min))
				min = beg;
			if (!comp(*first, *max))
				max = first;
		}
		else
		{
			if (comp(*first, *min))
				min = first;
			if (!comp(*beg, *max))
				max = beg;
		}
	}
	return { min, max };
}

// 斐波那契数列
constexpr std::uintmax_t binaryFibonacci(std::size_t n) {
	return (n >= 2) ? binaryFibonacci(n - 1) + binaryFibonacci(n - 2) : n;
}
// 直接求值n为90以下的斐波那契数
std::uintmax_t fibonacci(std::uintmax_t n)
{
	constexpr long double a = (1 + std::sqrt(5.0l)) / 2.0l;
	return std::pow(a, n) / std::sqrt(5.0l) + 0.5l;
}
// 自底向下 - O(n)
std::uintmax_t baseRecFibo(std::vector<std::uintmax_t> &results, std::size_t n)
{
	if (results[n] > 0 || n == 0)
		return results[n];
	results[n] = baseRecFibo(results, n - 2) + baseRecFibo(results, n - 1);
	if (results[n] < results[n - 1])
		throw std::underflow_error(std::to_string(n) + "的斐波那契数发生上溢");
	return results[n];
}
std::uintmax_t recFibo(std::size_t n)
{
	if (n <= 2)
		return n ? 1 : 0;
	std::vector<std::uintmax_t> results(n + 1);
	results[1] = results[2] = 1;
	return baseRecFibo(results, n);
}
// 自底向上 - O(n)
std::uintmax_t fibo(std::size_t n)
{
	std::uintmax_t sum = 1, next = 1;
	for (std::size_t i = 2; i < n; ++i)
		sum += std::exchange(next, sum);
	return sum;
}

// T(n) = 2T(n/2) + 2n
// 查找差值最大的两个元素(改自CLRS 4.1)
template<typename InputIt>
std::pair<InputIt, InputIt> findMaxDifference(InputIt first, InputIt mid, InputIt last)
{
	if (std::distance(first, last) <= 2)
		return { first, mid };

	auto left = findMaxDifference(first, std::next(first, std::distance(first, mid) / 2), mid);
	auto right = findMaxDifference(mid, std::next(mid, std::distance(mid, last) / 2), last);

	if (auto tmp = std::max_element(mid, last); *left.second < *tmp)
		left.second = tmp;
	if (auto tmp = std::min_element(first, mid); *tmp < *right.first)
		right.first = tmp;

	if (*left.second - *left.first >= *right.second - *right.first)
		return left;
	return right;
}

// 查找最大子数组 - O(n lg n)
template<typename InputIt>
std::tuple<InputIt, InputIt, typename std::iterator_traits<InputIt>::value_type> recFindMaxSubArray(InputIt first, InputIt last)
{
	if (std::distance(first, last) <= 1)
		return { first, last, (first == last) ? 0 : *first };

	auto mid = std::next(first, std::distance(first, last) / 2);

	auto left = recFindMaxSubArray(first, mid);
	auto right = recFindMaxSubArray(mid, last);
	auto current = algoBase::maxSubArray(first, mid, last);

	if (std::get<2>(left) >= std::get<2>(right) && std::get<2>(left) >= std::get<2>(current))
		return left;
	else if (std::get<2>(right) > std::get<2>(left) && std::get<2>(right) > std::get<2>(current))
		return right;
	return current;
}

template<typename InputIt>
std::tuple<InputIt, InputIt, typename std::iterator_traits<InputIt>::value_type> myFindMaxSubArray(InputIt first, InputIt last)
{
	using value_t = typename std::iterator_traits<InputIt>::value_type;
	value_t maxVal{std::numeric_limits<value_t>::lowest()}, sum{};
	InputIt maxBeg = first, maxEnd = maxBeg, tmpBeg = first;
	while (first != last)
	{
		sum = sum + *first;	
		if (maxVal < sum)
		{
			maxVal = sum;
			maxBeg = tmpBeg;
			maxEnd = std::next(first);
		}
		else if (sum < value_t{})
		{
			sum = value_t{};
			tmpBeg = std::next(first);
		}
		++first;
	}
	return { maxBeg, maxEnd, maxVal };
}

// 查找最大子数组 - O(n)
template<typename InputIt>
std::tuple<InputIt, InputIt, typename std::iterator_traits<InputIt>::value_type> findMaxSubArray(InputIt first, InputIt last)
{
	using value_type = typename std::iterator_traits<InputIt>::value_type;
	constexpr value_type lowest = std::numeric_limits<value_type>::lowest();

	InputIt maxStart = first, maxEnd = first, temp = first;
	value_type maxSum = lowest, sum{}, zero{};
	for (InputIt beg = first; beg != last; ++beg)
	{
		sum = sum + *beg;
		if (maxSum < sum)
		{
			maxSum = sum;
			maxEnd = std::next(beg);
			maxStart = temp;
		}
		else if (sum < zero)
		{
			sum = zero;
			temp = std::next(beg);
		}
	}
	return { maxStart, maxEnd, maxSum };
}

// 快速幂取模
std::intmax_t mod(std::intmax_t a, std::intmax_t b, std::intmax_t m)
{
	std::uintmax_t res = 1;
	a %= m;
	while (b != 0)
	{
		if (b & 1) res = (res * a) % m;
		b >>= 1;
		a = (a * a) % m;
	}
	return res;
}

}

namespace heap {

// 维护堆性质
template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void heapify(RandomIt first, RandomIt last, RandomIt current, Compare comp = {})
{
	RandomIt left, right, oldtemp;
	while (true)
	{
		oldtemp = current;
		left = std::next(first, ((current - first) * 2 + 1)), right = std::next(left);
		if (left < last && comp(*current, *left))
			current = left;
		if (right < last && comp(*current, *right))
			current = right;
		if (current == oldtemp)
			break;
		std::swap(*oldtemp, *current);
	}
}

// 建堆
template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void makeHeap(RandomIt first, RandomIt last, Compare comp = {})
{
	if (first < last)
		for (RandomIt beg = std::next(first, (last - first) / 2); first <= beg; --beg)
			heapify(first, last, beg, comp);
}

// 是否满足堆性质
template<typename RandomIt, typename Compare = std::less_equal<typename std::iterator_traits<RandomIt>::value_type>>
bool isHeap(RandomIt first, RandomIt last, RandomIt current, Compare comp = {})
{
	RandomIt left = first + ((current - first) * 2 + 1), right = left + 1;
	if ((left < last && !comp(*left, *current)) || (right < last && !comp(*right, *current)))
		return false;
	return ((left < last) ? isHeap(first, last, left, comp) : true) && ((right < last) ? isHeap(first, last, right, comp) : true);
}
template<typename RandomIt, typename Compare = std::less_equal<typename std::iterator_traits<RandomIt>::value_type>>
bool isHeap(RandomIt first, RandomIt last, Compare comp = {}) {
	return isHeap(first, last, first, comp);
}

// "弹出"最大/最小元素
template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void popHeap(RandomIt first, RandomIt last, Compare comp = {})
{
	std::iter_swap(first, std::prev(last));
	heapify(first, std::prev(last), first, comp);
}

// 将prev(last)处的元素插入栈
template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void pushHeap(RandomIt first, RandomIt last, Compare comp = {})
{
	if (std::distance(first, last) <= 0) return;
	RandomIt beg = std::prev(last);
	std::size_t size = last - first;
	typename std::iterator_traits<RandomIt>::value_type key(std::move(*beg));
	while (first < beg)
	{
		RandomIt p = first + ((beg - first + 1) / 2 - 1);
		if (comp(*p, key))
			*beg = std::move(*p);
		else break;
		beg = p;
	}
	*beg = std::move(key);
}

// 删除指定元素
template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void eraseHeap(RandomIt first, RandomIt last, RandomIt target, Compare comp = {})
{
	bool b = comp(*target, *std::prev(last));
	*target = *std::prev(last);
	if (b)
		pushHeap(first, std::next(target), comp);
	else heapify(first, last, target, comp);
}

// 堆排序
template<typename RandomIt, typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
void sortHeap(RandomIt first, RandomIt last, Compare comp = {})
{
	for (auto fnext = std::next(first); fnext < last; --last)
		popHeap(first, last, comp);
}

template<template<typename> typename Vec, typename Link, typename Compare = std::less<typename Link::value_type>>
Link mergeLink(Vec<Link> &list, Compare comp = {})
{
	if (list.empty())
		return {};

	std::vector<std::pair<typename Vec<Link>::size_type, typename Link::size_type>> minHeap;
	auto heapCompare = [](const auto &rhs, const auto &lhs) {
		return rhs.second > lhs.second;
	};

	for (std::size_t i = 0; i < list.size(); ++i)
		minHeap.emplace_back(i, list[i].size());
	makeHeap(minHeap.begin(), minHeap.end(), heapCompare);

	using value_type = typename Link::value_type;
	Link result;
	for (typename Vec<Link>::size_type i = 0; i < list.size(); ++i)
	{
		result.merge(list[minHeap.front().first], comp);
		popHeap(minHeap.begin(), minHeap.end() - i, heapCompare);
	}

	return result;
}

// K路归并(堆实现)
template<template<typename> typename Vec, typename Link, typename Compare = std::less<typename Link::value_type>>
Link KwayMerge(Vec<Link> &list, Compare comp = {})
{
	std::vector<Link> minHeap;
	for (auto &link : list)
		minHeap.push_back(std::move(link));

	auto heapComp = [comp](const auto &rhs, const auto &lhs) {
		return comp(lhs.front(), rhs.front());
	};
	makeHeap(minHeap.begin(), minHeap.end(), heapComp);

	Link result;
	while (!minHeap.empty())	//凭借最小堆性质，每次迭代将K个链表中的一个最小元素结点移入result
	{
		if (!minHeap[0].empty())
		{
			result.splice(result.end(), minHeap[0], minHeap[0].begin());		//将最小堆中的最小元素结点移到result
			if (minHeap[0].empty())		//最小堆首元素链表为空，将其与“第二大链表”交换并将空链表从堆中释放
			{
				std::swap(minHeap.front(), minHeap.back());
				minHeap.pop_back();
			}
		}
		else
		{
			std::swap(minHeap.front(), minHeap.back());
			minHeap.pop_back();
		}
		heapify(minHeap.begin(), minHeap.end(), minHeap.begin(), heapComp);
	}		//每次迭代后，最小堆首元素链表保有当前最小元素
	return result;
}

}

template<typename T>
bool check(const std::valarray<T> &array, std::size_t r, std::size_t s)
{
	for (std::size_t i = 0; i < s; ++i)
	{
		std::valarray<T> tmp(array[std::slice(i, r, s)]);
		if (!std::is_sorted(std::begin(tmp), std::end(tmp)))
			return false;
	}
	for (std::size_t i = 0; i < r; ++i)
	{
		std::valarray<T> tmp(array[std::slice(i * s, s, 1)]);
		if (!std::is_sorted(std::begin(tmp), std::end(tmp)))
			return false;
	}
	return true;
}

int hoare_partition(int arr[],int p,int q){
    int i,j,x,t;
    x=arr[p];
    i=p;
    j=q;
    while(1){
		for(;i<q && arr[i] < x;i++);
		for(;j>p && arr[j]>=x;j--);
        if(i<j){
            t = arr[i];
            arr[i] = arr[j];
            arr[j] = t;
        }else{
            return j;
	}
    }
}

std::list<int> func(std::vector<int> &v, int low, int high)
{
	std::cout << low << ' ' << high << std::endl;
	if (low == high)
		return std::list<int>{v[low]};
	std::vector<std::vector<int>> dp;
	int mid = (low + high) / 2;
	auto a = func(v, low, mid);
	auto b = func(v, mid + 1, high);
	if (a.back() < b.front())
	{
		std::cout << a.back() << ' ' << b.front() << std::endl;
		a.splice(a.end(), b);
		return a;
	}
	else if (b.back() < a.front())
	{
		b.splice(b.end(), a);
		return b;
	}
	return (a.size() < b.size()) ? b : a;
}

// test
int main(int argv, char **)
{
	// timer
	test::Timer<> timer;

	// random
	auto rand = std::bind(std::uniform_int_distribution<int>(0, 100), std::default_random_engine(std::random_device()()));
	std::vector<int> v;

	// init

	timer.start();
	std::cout << "用时: " << timer.finish().count() << std::endl;
}
