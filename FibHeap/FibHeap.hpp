#include <list>
#include <iostream>
#include <cmath>
#include <vector>
#include <functional>
#include <type_traits>

template<typename T, typename Compare = std::less<T>, typename Alloc = std::allocator<T>>
class FibHeap
{
	struct FibHeapNode;
public:
	template<typename U>
	using container = std::list<U>;
	using allocator_type = Alloc;
	using value_type = T;
	using reference = std::add_lvalue_reference_t<T>;
	using const_reference = std::add_const_t<reference>;
	using compare_type = Compare;
	using pointer = std::add_pointer_t<value_type>;
	using size_type = std::size_t;
	using iterator = typename container<FibHeapNode*>::iterator;

	FibHeap(const Compare &c = Compare{}, const Alloc &a = Alloc{}) : comp(c), alloc(a), min(roots.end()), totalNode(0) { }
	~FibHeap() {
		destroyAllNode(roots);
	}

	void splice(FibHeap &other)
	{
		if (other.min == other.roots.end()) return;
		if (min != roots.end() && comp(*(*other.min)->value, *(*min)->value))
			min = other.min;
		roots.splice(roots.end(), other.roots);
		totalNode += other.totalNode;
		other.totalNode = 0;
		other.min = other.roots.end();
	}
	iterator insert(const value_type &v)
	{
		auto node = roots.insert(min, makeNode(v));
		if (min == roots.end() || comp(v, *(*min)->value))
			min = node;
		++totalNode;
		return node;
	}
	iterator insert(value_type &&v)
	{
		auto node = roots.insert(min, makeNode(std::move(v)));
		if (min == roots.end() || comp(v, *(*min)->value))
			min = node;
		++totalNode;
		return node;
	}
	value_type pop()
	{
		if (min == roots.end())
			throw std::runtime_error("");
		roots.splice(min, (*min)->roots);
		value_type result = *(*min)->value;
		destroyNode(*min);
		min = roots.erase(min);
		if (roots.empty()) return result;
		else if (min == roots.end()) min = roots.begin();
		consolidate();
		--totalNode;
		return result;
	}
	void decrease(iterator node, const value_type &newKey)
	{
		if (comp(*(*node)->value, newKey)) throw std::runtime_error("");
		*(*node)->value = newKey;
		if ((*node)->parent != iterator{} && comp(newKey, *(*(*node)->parent)->value))
		{
			cutNode(node);
			cascadingCutNode((*node)->parent);
			(*node)->parent = iterator{};
		}
		if (comp(newKey, *(*min)->value)) min = node;
	}

	size_type size() const noexcept { return totalNode; }
private:
	void cutNode(iterator node)
	{
		roots.splice(min, (*(*node)->parent)->roots, node);
		--(*(*node)->parent)->degree;
		(*node)->mark = false;
	}
	void cascadingCutNode(iterator node)
	{
		if ((*node)->parent != iterator{})
		{
			if (!(*(*node)->parent)->mark)
				(*(*node)->parent)->mark = true;
			else
			{
				cutNode(node);
				cascadingCutNode((*node)->parent);
				(*node)->parent = iterator{};
			}
		}
	}
	template<typename... Args>
	FibHeapNode* makeNode(Args&&... args)
	{
		auto temp = alloc.allocate(1);
		FibHeapNode *node;
		try {
			node = new FibHeapNode(temp);
			alloc.construct(temp, std::forward<Args>(args)...);
		} catch (...) {
			alloc.destroy(temp);
			throw;
		}
		return node;
	}
	void destroyNode(FibHeapNode *node)
	{
		alloc.destroy(node->value);
		alloc.deallocate(node->value, 1);
		delete node;
	}
	void destroyAllNode(container<FibHeapNode*> &r)
	{
		for (FibHeapNode *node : r)
		{
			destroyAllNode(node->roots);
			alloc.destroy(node->value);
			alloc.deallocate(node->value, 1);
			delete node;
		}
	}
	void linkNode(iterator y, iterator x)
	{
		(*x)->roots.splice((*x)->roots.begin(), roots, y);
		++(*x)->degree;
		(*y)->mark = false;
		(*y)->parent = x;
	}
	void consolidate()
	{
		std::vector<std::pair<bool, iterator>> temp(std::ceil(std::log2(totalNode)), std::make_pair(false, roots.end()));
		for (auto x = roots.begin(); x != roots.end(); ++x)
		{
			std::size_t d = (*x)->degree;
			while (temp[d].first && temp[d].second != x)
			{
				auto y = temp[d].second;
				if (comp(*(*y)->value, *(*x)->value)) std::swap(y, x);
				linkNode(y, x);
				temp[d++].first = false;
			}
			temp[d].second = x;
			temp[d].first = true;
		}
		min = roots.begin();
		for (auto beg = std::next(roots.begin()); beg != roots.end(); ++beg)
			if (comp(*(*beg)->value, *(*min)->value)) min = beg;
	}

	compare_type comp;
	allocator_type alloc;
	container<FibHeapNode*> roots;
	iterator min;
	size_type totalNode;
};

template<typename T, typename Compare, typename Alloc>
struct FibHeap<T, Compare, Alloc>::FibHeapNode {
	FibHeapNode(pointer v, iterator p = iterator{}, std::size_t d = 0, bool m = false) : value(v), parent(p), degree(d), mark(m) { }
	container<FibHeapNode*> roots;
	pointer value;
	iterator parent;
	std::size_t degree;
	bool mark;
};
