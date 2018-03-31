#pragma once
#include <cstddef>
#include <algorithm>

template <typename T>
T** make2dArray(std::size_t numberOfRows, std::size_t numberOfCols)
{
	T **p = new T*[numberOfRows] {};
	for(std::size_t i = 0; i < numberOfRows; ++i)
		p[i] = new T[numberOfCols] {};
	return p;
}

template <typename T, typename S = std::size_t>
T** make2dArray(S numberOfRows, const S *rowSize)
{
	T **p = new T*[numberOfRows] {};
	for(std::size_t i = 0; i < numberOfRows; ++i)
		p[i] = new T[rowSize[i]] {};
	return p;
}

template <typename T>
void delete2dArray(T ** &p, std::size_t numberOfRows)
{
	for(std::size_t i = 0; i < numberOfRows; ++i)
		delete p[i];
	delete p;
	p = nullptr;
}

template <typename T>
void changeLength2D(T ** &p, std::size_t oldRow, std::size_t oldCol, std::size_t newRow, std::size_t newCol)
{
	T **temp = make2dArray<T>(newRow, newCol);
	for(std::size_t i = 0; i < oldRow; ++i)
		std::copy_n(p[i], std::min(oldCol, newCol), temp[i]);
	delete2dArray(p, oldRow);
	p = temp;
}
