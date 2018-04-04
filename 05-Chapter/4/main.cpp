#include "MyAlgorithms.hpp"

template <typename T>
void fast(T **a, T **b, T **c, size_t n)
{
	for(size_t i = 0; i < n; ++i)
		for(size_t j = 0; j < n; ++i)
			for(size_t k = 0; k < n; ++i)
				c[i][j] += a[i][k] * b[k][j];
}

int main()
{
	permutation("ijk");
}
