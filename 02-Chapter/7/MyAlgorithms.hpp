#pragma once

template <typename T>
T factorial(T n)
{
	for(T ret = n; 1 < ret; n *= --ret);
	return n;
}
