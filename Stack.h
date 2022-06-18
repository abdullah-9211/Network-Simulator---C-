
#pragma once
#include "util.h"
#include "List.h"

template<typename Type>
class Stack
{
private:

	// members

	List<Type> m_List;

public:

	// empty state

	bool empty() const
	{ return m_List.empty(); }

	void clear()
	{ m_List.clear(); }

	// access

	int size() const
	{ return m_List.size(); }

	Type& top()
	{
		ErrorAbort(empty(), "Stack::top() : stack is empty");
		return m_List.back();
	}

	const Type& top() const
	{
		ErrorAbort(empty(), "Stack::top() : stack is empty");
		return m_List.back();
	}

	// push and pop

	void push(const Type& data)
	{ m_List.InsertBack(data); }

	void pop()
	{
		ErrorAbort(empty(), "Stack::pop() : stack is empty");
		m_List.RemoveBack();
	}

	// compare

	friend bool operator==(const Stack<Type>& s1, const Stack<Type>& s2)
	{ return s1.m_List == s2.m_List; }

	friend bool operator!=(const Stack<Type>& s1, const Stack<Type>& s2)
	{ return !(s1 == s2); }
};
