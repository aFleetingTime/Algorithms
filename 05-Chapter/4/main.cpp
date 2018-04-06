#include "MyAlgorithms.hpp"
#include <chrono>
#include <numeric>
#include <iomanip>

template <typename T>
void fast(T *a, T *b, T *c, size_t n)
{
	for(size_t i = 0; i < n; ++i)
		for(size_t j = 0; j < n; ++j)
			for(size_t k = 0; k < n; ++k)
				c[i][j] += a[i][k] * b[k][j];
}

template <typename T>
void fast2(T *a, T *b, T *c, size_t n)
{
	for(size_t i = 0; i < n; ++i)
		for(size_t k = 0; k < n; ++k)
			for(size_t j = 0; j < n; ++j)
				c[i][j] += a[i][k] * b[k][j];
}

template <typename T>
void init(T *a, std::size_t row, std::size_t col)
{
	for(std::size_t i = 0; i < row; ++i)
		std::fill_n(*(a + i), col, 0);
}

int main()
{
	constexpr std::size_t MaxSize = 800;
	int a[MaxSize][MaxSize]{}, b[MaxSize][MaxSize]{}, c[MaxSize][MaxSize]{};
	for(std::size_t i = 100; i <= MaxSize; i += 100)
	{
		//std::iota(reinterpret_cast<int*>(c), reinterpret_cast<int*>(c) + i, 0);
		init(a, MaxSize, MaxSize);
		init(b, MaxSize, MaxSize);
		init(c, MaxSize, MaxSize);
		auto start = std::chrono::system_clock::now();
		fast2(a, b, c, i);
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> seconds(end - start);
		std::cout << "Size: " << std::setw(4) << i << "  count: " << seconds.count() << std::endl;
	}
}
