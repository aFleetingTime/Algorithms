#pragma once
#include <type_traits>

template<typename Key, typename Compare>
struct CompareAux {
	using key_type = Key;
	using value_type = Key;
	using compare = Compare;

	bool operator()(const value_type &lhs, const value_type &rhs) const {
		return comp(lhs, rhs);
	}
	Compare comp;
};

template<typename Key, typename Value, typename Compare>
struct MapCompareAux {
	using key_type = const Key;
	using value_type = std::pair<key_type, Value>;
	using compare = Compare;

	bool operator()(const value_type &lhs, const value_type &rhs) const {
		return comp(lhs.first, rhs.first);
	}
	bool operator()(const key_type &lhs, const value_type &rhs) const {
		return comp(lhs, rhs.first);
	}
	bool operator()(const value_type &lhs, const key_type &rhs) const {
		return comp(lhs.first, rhs);
	}
	Compare comp;
};

template<typename T>
struct IsCompareAux : std::false_type { };
template<typename K, typename C>
struct IsCompareAux<CompareAux<K, C>> : std::true_type { };
template<typename K, typename V, typename C>
struct IsCompareAux<MapCompareAux<K, V, C>> : std::true_type { };

template<typename K, typename T>
using toCompareAux = std::conditional_t<IsCompareAux<T>::value, T, CompareAux<K, T>>;
template<typename K, typename V, typename T>
using toMapCompareAux = std::conditional_t<IsCompareAux<T>::value, T, MapCompareAux<K, V, T>>;
