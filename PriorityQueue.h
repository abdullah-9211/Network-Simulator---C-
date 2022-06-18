
#pragma once
#include "util.h"
#include "Array.h"

// TODO: add build-heap construction

template<typename ComparatorType>
class PriorityQueue
{
private:

	// types

	using Type = typename ComparatorType::ValueType;

	// members

	Array<Type> m_Data = Array<Type>(1);
	ComparatorType m_Comparator;

	// indexing

	static int root()
	{ return 1; }

	static int parent(int index)
	{ return index >> 1; }

	static int LeftChild(int index)
	{ return index << 1; }

	static int RightChild(int index)
	{ return (index << 1) | 1; }

public:

	// empty state

	bool empty() const
	{ return m_Data.size() == 1; }

	void clear()
	{ m_Data = Array<Type>(1); }

	// access

	int size() const
	{ return m_Data.size() - 1; }

	const Type& front() const
	{
		ErrorAbort(empty(), "PriorityQueue::front() : queue is empty");
		return m_Data[root()];
	}
	
	// enqueue and dequeue

	void enqueue(const Type& data)
	{
		m_Data.InsertBack(data);

		for(int index = m_Data.size() - 1; index > root();)
		{
			int parentIndex = parent(index);

			if(!m_Comparator.compare(m_Data[parentIndex], m_Data[index]))
			{
				Swap(m_Data[parentIndex], m_Data[index]);
				index = parentIndex;
			}
			else break;
		}
	}

	void dequeue()
	{
		ErrorAbort(empty(), "PriorityQueue::dequeue() : queue is empty");
		m_Data[root()] = m_Data.back();
		m_Data.RemoveBack();

		for(int index = root(); index < m_Data.size();)
		{
			int leftIndex = LeftChild(index);
			int rightIndex = RightChild(index);

			if(!(leftIndex < m_Data.size() && rightIndex < m_Data.size()))
				break;

			int nextIndex = m_Comparator.compare(m_Data[leftIndex], m_Data[rightIndex]) ? leftIndex : rightIndex;

			if(!m_Comparator.compare(m_Data[index], m_Data[nextIndex]))
			{
				Swap(m_Data[index], m_Data[nextIndex]);
				index = nextIndex;
			}
			else break;
		}
	}
};
