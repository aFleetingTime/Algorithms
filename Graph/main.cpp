#include "Graph.hpp"
#include <iostream>
#include <queue>

template<typename T1, typename T2, typename V>
void insert(Graph<T1, T2> &g, size_t n1, size_t n2, V v)
{
	g.insertEdge(g.atVertex(n1), g.atVertex(n2), v);
	g.insertEdge(g.atVertex(n2), g.atVertex(n1), v);
}

void DFS(Graph<std::pair<bool, int>, bool> &g)
{
	std::queue<typename Graph<std::pair<bool, int>, bool>::Vertex> q;
	q.push(g.firstVertex());
	(*q.front()).first = true;
	while (!q.empty())
	{
		auto [beg, end] = g.findEdges(q.front());
		while (beg != end)
		{
			if (!(*(beg->first)).first)
			{
				q.push(beg->first);
				(*(beg->first)).first = true;
				(*(beg->first)).second = (*q.front()).second + 1;
			}
			++beg;
		}
		q.pop();
	}
}

int main()
{
	Graph<std::pair<bool, int>, bool> g;
	for (int i = 0; i < 8; ++i)
		g.insertVertex({ false, 0 });
	insert(g, 0, 4, 0);
	insert(g, 0, 1, 0);
	insert(g, 1, 5, 0);
	insert(g, 5, 2, 0);
	insert(g, 5, 6, 0);
	insert(g, 2, 3, 0);
	insert(g, 2, 6, 0);
	insert(g, 6, 3, 0);
	insert(g, 6, 7, 0);
	insert(g, 7, 3, 0);
	DFS(g);
	auto beg = g.firstVertex(), end = g.lastVertex();
	int i = 0;
	while (beg != end)
	{
		std::cout << ++i << ": " << (*beg).second << std::endl;
		++beg;
	}
	std::cout << ++i << ": " << (*beg).second << std::endl;
}
