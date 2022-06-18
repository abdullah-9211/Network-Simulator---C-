
#pragma once
#include "util.h"

template<typename T1, typename T2>
struct Pair
{
	// members

	T1 first = T1();
	T2 second = T2();

	// constructors

	Pair() = default;

	Pair(const T1& first, const T2& second)
		: first(first), second(second)
	{}
};

// compare

template<typename T1, typename T2>
bool operator==(const Pair<T1, T2>& p1, const Pair<T1, T2>& p2)
{ return (p1.first == p2.first) && (p1.second == p2.second); }

template<typename T1, typename T2>
bool operator!=(const Pair<T1, T2>& p1, const Pair<T1, T2>& p2)
{ return !(p1 == p2); }

template<typename T1, typename T2>
bool operator<(const Pair<T1, T2>& p1, const Pair<T1, T2>& p2)
{
	if(p1.first < p2.first)
		return true;

	else if(p1.first == p2.first)
		return p1.second < p2.second;

	else
		return false;
}

template<typename T1, typename T2>
bool operator<=(const Pair<T1, T2>& p1, const Pair<T1, T2>& p2)
{ return (p1 < p2) || (p1 == p2); }

template<typename T1, typename T2>
bool operator>(const Pair<T1, T2>& p1, const Pair<T1, T2>& p2)
{ return p2 < p1; }

template<typename T1, typename T2>
bool operator>=(const Pair<T1, T2>& p1, const Pair<T1, T2>& p2)
{ return p2 <= p1; }
