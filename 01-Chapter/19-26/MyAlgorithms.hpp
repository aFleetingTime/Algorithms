#pragma once
#include <algorithm>
#include <functional>
#include <exception>
#include <stdexcept>
#include <iterator>
#include <iostream>
#include <type_traits>

template <typename T>
T nonRecursiveFactorial(T number)	//斐波那契序列
{
	for(int i = number; i > 2; number *= --i);
	return number;
}

template <typename T>
T fibonacci(T number)
{
	if(number < 2) return number;
	return fibonacci(number - 1) + fibonacci(number - 2);
}

template <typename T>
T nonRecursiveFibonacci(T number)
{
	T result = 1, next = 1, prev = 1;
	while(number-- > 2)
	{
		next = prev;
		prev = result;
		result = next + prev;
	}
	return result;
}

template <typename T>
T function(T n)
{
	if(n % 2) return function(3 * n + 1);
	return n / 2;
}

template <typename T>
T nonRecursiveFunction(T n)
{
	if(n % 2) return (3 * n + 1) / 2;
	return n / 2;
}

template <typename T>
T ackermann(T i, T j)
{
	if(!i) return j + 1;
	if(!j) return ackermann(i - 1, 1);
	return ackermann(i - 1, ackermann(i, j - 1));
}

template <typename T>
T euclid(T x, T y)
{
	if(!y) return x;
	return euclid(y, x % y);
}

template <typename T>
bool includeVal(T *a, std::size_t i, T val)
{
	if(!i) return false;
	if(a[--i] == val) return true;
	return includeVal(a, i, val);
}

template <typename T>
void subsetGeneration(T *set, bool *flags, std::size_t k, std::size_t m)
{
	if(k == m - 1)
	{
		flags[k] = false;
		if(std::all_of(flags, flags + m, [](bool b){ return !b; }))
			std::cout << "null";
		else
		{
			for(std::size_t i = 0; i != m; ++i)
				if(flags[i]) std::cout << set[i];
		}
		std::cout << " ";
		flags[k] = true;
		for(std::size_t i = 0; i != m; ++i)
			if(flags[i]) std::cout << set[i];
		std::cout << " ";
		return;
	}
	flags[k] = false;
	subsetGeneration(set, flags, k + 1, m);
	flags[k] = true;
	subsetGeneration(set, flags, k + 1, m);
}

template <typename T>
void grayCode(T n)
{
	if(n == 1)
	{
		std::cout << n;
		return;
	}
	grayCode(n - 1);
	std::cout << n;
	grayCode(n - 1);
}
