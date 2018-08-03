#include <random>
#include <vector>
#include <stack>
#include <functional>
#include <type_traits>

static auto getRandomNum = std::bind(std::uniform_int_distribution<unsigned>(1, std::numeric_limits<unsigned>::max()),
									 std::default_random_engine(std::random_device()()));

template<typename NumType>
struct Interval {
	NumType low, high;
};
template<typename NumType>
struct TreapNode {
	TreapNode(unsigned prio, TreapNode *null, Interval<NumType> i) :
		p(null), left(null), right(null), priority(prio), interval(i), max(i.high) { }
	TreapNode(unsigned prio, TreapNode *null) :
		p(null), left(null), right(null), priority(prio), interval{}, max{} { }
	TreapNode *p, *left, *right;
	Interval<NumType> interval;
	NumType max;
	const unsigned priority;
};
// treap树
template<typename NumType, typename Compare = std::less<NumType>, typename IntervalComp = std::less_equal<NumType>, typename Alloc = std::allocator<TreapNode<NumType>>>
class SimpleIntervalTree
{
	static_assert(std::is_integral_v<NumType> || std::is_floating_point_v<NumType>);
public:
	using node_type = TreapNode<NumType>;
	using interval_type = Interval<NumType>;

	SimpleIntervalTree() : null(alloc.allocate(1)), root(null) {
		alloc.construct(null, std::numeric_limits<unsigned>::max(), null);
	}

	~SimpleIntervalTree() {
		if (root != null)
		{
			std::stack<node_type*> s;
			s.push(root);
			node_type *temp;
			while (!s.empty())
			{
				temp = s.top(); s.pop();
				if (temp->right != null)
					s.push(temp->right);
				if (temp->left != null)
					s.push(temp->left);
				alloc.deallocate(temp, 1);
			}
		}
		alloc.deallocate(null, 1);
	}

	node_type* insert(interval_type i)
	{
		node_type *x = root, *newNode = alloc.allocate(1);
		try {
			alloc.construct(newNode, getRandomNum(), null, i);
		} catch (...) {
			alloc.deallocate(newNode, 1);
			throw;
		}
		if (x == null)
			return root = newNode;
		node_type* node_type::*dire;
		while (true)
		{
			if (comp(i.low, x->interval.low))
				dire = &node_type::left;
			else dire = &node_type::right;
			if (comp(x->max, newNode->max))
				x->max = newNode->max;
			if (x->*dire == null) break;
			x = x->*dire;
		}
		newNode->p = x;
		x->*dire = newNode;
		insertFixup(newNode);
		return newNode;
	}

	void erase(node_type *x)
	{
		if (x == null) return;//throw std::invalid_argument("node is null");
		if (null->left != null || null->right != null) throw;
		while (true)
		{
			if (x->left != null && (x->right == null || icomp(x->left->priority, x->right->priority)))
				rightRotate(x);
			else if (x->right != null)
				leftRotate(x);
			else break;
		}
		((x == root) ? root : (x == x->p->left) ? x->p->left : x->p->right) = null;
		node_type *y = x->p;
		while (y != null)
		{
			y->max = std::max(y->interval.high, std::max(y->left->max, y->right->max));
			y = y->p;
		}
		alloc.deallocate(x, 1);
	}

	node_type* find(interval_type i)
	{
		node_type *x = root;
		while (x != null && (comp(i.high, x->interval.low) || comp(x->interval.high, i.low)))
		{
			if (x->left != null && icomp(i.low, x->left->max))
				x = x->left;
			else x = x->right;
		}
		return x;
	}

	std::vector<node_type*> findAll(interval_type i)
	{
		std::vector<node_type*> res;
		return findAllBase(root, i, res), res;
	}

#if 0
	node_type* findExactly(interval_type i)
	{
		node_type *x = root;
		while (x != null)
		{
			if (x->interval.low == i.low && x->interval.high == i.high) return x;
			else if (icomp(i.low, x->interval.low))
			{
				if (x->right->interval.low == i.low && x->right->interval.high == i.high) return x;
				x = x->left;
			}
			else
			{
				if (x->left->interval.low == i.low && x->left->interval.high == i.high) return x;
				x = x->right;
			}
		}
		return x;
	}
#endif

	node_type* nullp() noexcept { return null; }
	node_type* rootp() noexcept { return root; }

private:
	Alloc alloc;
	Compare comp;
	IntervalComp icomp;
	node_type *null, *root = null;

	void findAllBase(node_type *x, interval_type i, std::vector<node_type*> &dest)
	{
		if (x->left != null && icomp(i.low, x->left->max))
			findAllBase(x->left, i, dest);
		if (icomp(i.low, x->interval.high) && icomp(x->interval.low, i.high))
			dest.push_back(x);
		if (x->right != null && icomp(x->interval.low, i.high) && icomp(i.low, x->right->max))
			findAllBase(x->right, i, dest);
	}

	node_type* rotate(node_type *x, node_type* node_type::*a, node_type* node_type::*b)
	{
		(x->*a)->p = x->p;
		if (x->p == null)
			root = x->*a;
		else (x == x->p->left ? x->p->left : x->p->right) = x->*a;
		x->p = x->*a;
		return (x->*a = std::exchange((x->*a)->*b, x))->p = x;
	}

	node_type* leftRotate(node_type *x) {
		return rotate(x, &node_type::right, &node_type::left);
	}
	node_type* rightRotate(node_type *x) {
		return rotate(x, &node_type::left, &node_type::right);
	}

	void insertFixup(node_type *x)
	{
		node_type *y;
		while (x != root && x->priority < x->p->priority)
		{
			if (x == x->p->left)
				y = rightRotate(x->p);
			else y = leftRotate(x->p);
			x->max = y->max;
			y->max = std::max(y->interval.high, std::max(y->left->max, y->right->max));
		}
	}
};
