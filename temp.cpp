#include <vector>
#include <string>
#include <algorithm>
#include <type_traits>
#include <chrono>
#include <iostream>
using namespace std;
    string longestCommonPrefix(vector<string>& strs) {

        int j = 0, min = min_element(strs.cbegin(), strs.cend(), [](const auto &lhs, const auto &rhs) {
            return lhs.size() < rhs.size();
        })->size();
        for (;j < min; ++j)
        {
            char cur = strs[0][j];
            for (int i = 1; i < strs.size(); ++i)
                if (strs[i][j] != cur) return strs.front().substr(0, j);
        }
        return strs.front().substr(0, j);
    }
int main()
{
	vector<string> vs { "dog", "racecar", "car" };
	cout << longestCommonPrefix(vs) << '\n';
}
