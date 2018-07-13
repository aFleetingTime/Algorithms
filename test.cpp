#include <valarray>
#include <iostream>

struct T
{
	T() { }
	T(T&&) { std::cout << "111\n"; }
	T& operator=(T&&) { std::cout << "222\n"; return *this; }
};
struct C
{
	C() {} 
	C(int a) { }
	C(char  a) { }
	//C(C&&) { }
	//C& operator=(C&&) { }
	T v;
};

int main()
{
	C c;
	C b(std::move(c));
	b = std::move(c);
}
