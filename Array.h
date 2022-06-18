
#pragma once
#include "util.h"

template<typename Type>
class Array
{
private:

	// members

	Type* m_Data = nullptr;
	int m_Size = 0;
	int m_Capacity = 0;

public:

	// constructors

	Array() = default;

	explicit Array(int size)
		: m_Size(size), m_Capacity(size)
	{
		if(m_Capacity)
			m_Data = new Type[m_Capacity]{};
	}

	explicit Array(int size, const Type& data)
		: Array(size)
	{ fill(data); }

	// memory management

	Array(const Array& other)
	{ *this = other; }

	Array& operator=(const Array& other)
	{
		if(this != &other) // self-assignment check
		{
			// resize if source is larger than destination

			if(other.m_Size > m_Capacity)
			{
				delete[] m_Data;
				m_Capacity = other.m_Size;
				m_Data = new Type[m_Capacity]{};
			}

			// copy data and update size

			CopyArray(m_Data, other.m_Data, other.m_Size);
			m_Size = other.m_Size;
		}

		return *this;
	}

	~Array()
	{ delete[] m_Data; }

	// fill

	void fill(const Type& data)
	{ FillArray(m_Data, m_Size, data); }

	// empty state

	bool empty() const
	{ return m_Size == 0; }

	void clear()
	{ m_Size = 0; }

	// access

	Type* data()
	{ return m_Data; }

	const Type* data() const
	{ return m_Data; }

	int size() const
	{ return m_Size; }

	int capacity() const
	{ return m_Capacity; }

	Type& operator[](int index)
	{
		ErrorAbort(!InRange(index, 0, m_Size - 1), "Array::operator[]() : index out of bounds");
		return m_Data[index];
	}

	const Type& operator[](int index) const
	{
		ErrorAbort(!InRange(index, 0, m_Size - 1), "Array::operator[]() : index out of bounds");
		return m_Data[index];
	}

	Type& front()
	{
		ErrorAbort(empty(), "Array::front() : array is empty");
		return m_Data[0];
	}

	const Type& front() const
	{
		ErrorAbort(empty(), "Array::front() : array is empty");
		return m_Data[0];
	}

	Type& back()
	{
		ErrorAbort(empty(), "Array::back() : array is empty");
		return m_Data[m_Size - 1];
	}

	const Type& back() const
	{
		ErrorAbort(empty(), "Array::back() : array is empty");
		return m_Data[m_Size - 1];
	}

	// insert

	void insert(const Type& insertionData, int insertionIndex)
	{
		// after insertion, the data inserted will be positioned at the given index.

		ErrorAbort(!InRange(insertionIndex, 0, m_Size), "Array::insert() : index out of bounds");

		// resize if capacity is reached

		if(m_Size == m_Capacity)
		{
			Type* oldData = m_Data;
			m_Capacity = (m_Capacity == 0) ? 2 : (m_Capacity * 2);
			m_Data = new Type[m_Capacity]{};
			CopyArray(m_Data, oldData, m_Size);
		}

		// update size and insert by shifting right

		m_Size++;
		ShiftArrayRight(m_Data + insertionIndex, m_Size - insertionIndex);
		m_Data[insertionIndex] = insertionData;
	}

	void InsertFront(const Type& data)
	{ insert(data, 0); }

	void InsertBack(const Type& data)
	{ insert(data, m_Size); }

	// remove

	void remove(int removalIndex)
	{
		ErrorAbort(!InRange(removalIndex, 0, m_Size - 1), "Array::remove() : index out of bounds");
		ShiftArrayLeft(m_Data + removalIndex, m_Size - removalIndex);
		m_Size--;
	}

	void RemoveFront()
	{ remove(0); }

	void RemoveBack()
	{ remove(m_Size - 1); }

	// linear search

	int search(const Type& data) const
	{ return SearchArray(m_Data, m_Size, data); }

	// bubble sort

	void SortAscending()
	{ SortArrayAscending(m_Data, m_Size); }

	void SortDescending()
	{ SortArrayDescending(m_Data, m_Size); }
};

// compare

template<typename Type>
bool operator==(const Array<Type>& a1, const Array<Type>& a2)
{ return CompareArray(a1.data(), a2.data(), a1.size(), a2.size()) == 0; }

template<typename Type>
bool operator!=(const Array<Type>& a1, const Array<Type>& a2)
{ return !(a1 == a2); }

template<typename Type>
bool operator<(const Array<Type>& a1, const Array<Type>& a2)
{ return CompareArray(a1.data(), a2.data(), a1.size(), a2.size()) == -1; }

template<typename Type>
bool operator<=(const Array<Type>& a1, const Array<Type>& a2)
{ 
	int comparison = CompareArray(a1.data(), a2.data(), a1.size(), a2.size());
	return comparison == -1 || comparison == 0;
}

template<typename Type>
bool operator>(const Array<Type>& a1, const Array<Type>& a2)
{ return a2 < a1; }

template<typename Type>
bool operator>=(const Array<Type>& a1, const Array<Type>& a2)
{ return a2 <= a1; }
