
#pragma once
#include "util.h"
#include "List.h"

template<typename Type>
class Queue
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

	Type& front()
	{
		ErrorAbort(empty(), "Queue::front() : queue is empty");
		return m_List.front();
	}

	const Type& front() const
	{
		ErrorAbort(empty(), "Queue::front() : queue is empty");
		return m_List.front();
	}

	Type& back()
	{
		ErrorAbort(empty(), "Queue::back() : queue is empty");
		return m_List.back();
	}

	const Type& back() const
	{
		ErrorAbort(empty(), "Queue::back() : queue is empty");
		return m_List.back();
	}

	// enqueue and dequeue

	void enqueue(const Type& data)
	{ m_List.InsertBack(data); }

	void dequeue()
	{
		ErrorAbort(empty(), "Queue::dequeue() : queue is empty");
		m_List.RemoveFront();
	}

	// compare

	friend bool operator==(const Queue<Type>& q1, const Queue<Type>& q2)
	{ return q1.m_List == q2.m_List; }

	friend bool operator!=(const Queue<Type>& q1, const Queue<Type>& q2)
	{ return !(q1 == q2); }
};
