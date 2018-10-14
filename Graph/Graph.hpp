#include <list>
#include <unordered_map>

struct VertexDoesNotExist;

template<typename VertexDataT, typename EdgeDataT, typename VertexDataAlloc = std::allocator<VertexDataT>>
class Graph
{
public:

	using VertexData = VertexDataT;
	using VertexList = std::list<VertexData, VertexDataAlloc>;
	class Vertex;
	using EdgeData = EdgeDataT;
	using EdgeList = std::unordered_map<Vertex, std::unordered_multimap<Vertex, EdgeData, typename Vertex::Hash>, typename Vertex::Hash>;
	using Edge = typename EdgeList::mapped_type::iterator;

	Vertex insertVertex(const VertexData &data = VertexData{}) {
		vertices.push_front(data);
		return Vertex(vertices.begin());
	}
	Vertex insertVertex(VertexData &&data) {
		vertices.push_front(std::move(data));
		return Vertex(vertices.begin());
	}
	Edge insertEdge(Vertex from, Vertex to, const EdgeData &edge) {
		return edges[from].emplace(to, edge);
	}
	void eraseVertex(Vertex vertex) {
		edges.erase(vertex);
		for (auto &edgelist : edges)
		{
			auto [first, last] = edgelist.equal_range(vertex);
			edgelist.erase(first, last);
		}
		vertices.erase(vertex.base);
	}
	Edge eraseEdge(Vertex from, Vertex to) {
		auto result = edges.find(from);
		if (result == edges.end())
			throw VertexDoesNotExist("");
		return result->second.erase(to);
	}
	Vertex firstVertex() { return Vertex(vertices.begin()); }
	Vertex lastVertex() { return Vertex(std::prev(vertices.end())); }
	Vertex atVertex(size_t num) { return Vertex(std::next(vertices.begin(), num)); }

	std::pair<Edge, Edge> findEdges(Vertex vertex)
	{
		auto &tmp = edges[vertex];
		return { tmp.begin(), tmp.end() };
	}

private:
	VertexList vertices;
	EdgeList edges;

public:
	class Vertex
	{
		friend class Graph<VertexDataT, EdgeDataT, VertexDataAlloc>;
	public:
		Vertex() = delete;
		struct Hash {
			size_t operator()(const Vertex &vertex) const {
				return std::hash<VertexData*>()(&*(vertex.base));
			}
		};
		VertexData& operator*() const { return *base; }
		Vertex& operator++() {
			++base;
			return *this;
		}
		Vertex& operator--() {
			--base;
			return *this;
		}
		Vertex operator++(int) {
			return Vertex(base++);
		}
		Vertex operator--(int) {
			return Vertex(base--);
		}
		bool operator==(const Vertex &rhs) const { return base == rhs.base; }
		bool operator!=(const Vertex &rhs) const { return base != rhs.base; }
	private:
		explicit Vertex(typename VertexList::iterator it) : base(it) { }
		mutable typename VertexList::iterator base;
	};
};

struct VertexDoesNotExist : std::invalid_argument {
	VertexDoesNotExist(const std::string &s = "verrex does not exist!") : std::invalid_argument(s) { }
};
