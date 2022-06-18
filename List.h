
#pragma once
#include "util.h"

template<typename Type>
class List
{
private:

	// node

	struct Node
	{
		Type data = Type();
		Node* prev = nullptr;
		Node* next = nullptr;
	};

	// members

	Node* m_First = nullptr;
	Node* m_Last = nullptr;
	int m_Size = 0;

public:

	// iterator

	class Iterator;

	class ConstIterator
	{
	protected:

		// friends

		friend class List;

		// members

		Node* m_Node = nullptr;

		// constructors

		explicit ConstIterator(Node* node) :
			m_Node(node)
		{}

	public:

		// constructors

		ConstIterator() = default;

		ConstIterator(Iterator it)
			: m_Node(it.m_Node)
		{}

		// access

		bool valid() const
		{ return m_Node != nullptr; }

		const Type& operator*() const
		{
			ErrorAbort(!valid(), "List::ConstIterator::operator*() : invalid iterator");
			return m_Node->data;
		}

		const Type* operator->() const
		{
			ErrorAbort(!valid(), "List::ConstIterator::operator->() : invalid iterator");
			return &m_Node->data;
		}

		// increment

		ConstIterator next() const
		{ return valid() ? ConstIterator(m_Node->next) : ConstIterator(nullptr); }

		ConstIterator& operator++()
		{
			*this = next();
			return *this;
		}

		ConstIterator operator++(int)
		{
			ConstIterator temp = *this;
			++(*this);
			return temp;
		}

		// decrement

		ConstIterator prev() const
		{ return valid() ? ConstIterator(m_Node->prev) : ConstIterator(nullptr); }

		ConstIterator& operator--()
		{
			*this = prev();
			return *this;
		}

		ConstIterator operator--(int)
		{
			ConstIterator temp = *this;
			--(*this);
			return temp;
		}

		// compare

		friend bool operator==(ConstIterator i1, ConstIterator i2)
		{ return i1.m_Node == i2.m_Node; }

		friend bool operator!=(ConstIterator i1, ConstIterator i2)
		{ return !(i1 == i2); }
	};

	class Iterator : public ConstIterator
	{
	private:

		// scope resolution

		using ConstIterator::m_Node;

		// friends

		friend class List;
		friend class ConstIterator;

		// constructors

		explicit Iterator(Node* node) :
			ConstIterator(node)
		{}

	public:

		// scope resolution
		
		using ConstIterator::valid;

		// constructors

		Iterator() = default;

		// access

		Type& operator*() const
		{
			ErrorAbort(!valid(), "List::Iterator::operator*() : invalid iterator");
			return m_Node->data;
		}

		Type* operator->() const
		{
			ErrorAbort(!valid(), "List::Iterator::operator->() : invalid iterator");
			return &m_Node->data;
		}
	};

public:

	// constructors and memory management

	List() = default;

	List(const List& other)
	{ *this = other; }

	List& operator=(const List& other)
	{
		if(this != &other)
		{
			clear();

			if(!other.empty())
			{
				m_First = new Node{other.m_First->data};
				Node* thisNode = m_First;
				Node* otherNode = other.m_First;

				while(otherNode->next)
				{
					thisNode->next = new Node{otherNode->next->data, thisNode};
					thisNode = thisNode->next;
					otherNode = otherNode->next;
				}

				m_Last = thisNode;
				m_Size = other.m_Size;
			}
		}

		return *this;
	}

	~List()
	{ clear(); }

	// empty state

	bool empty() const
	{ return !(m_First || m_Last || m_Size); }

	void clear()
	{
		Node* currentNode = m_First;

		while(currentNode)
		{
			Node* tempNode = currentNode;
			currentNode = currentNode->next;
			delete tempNode;
		}

		m_First = nullptr;
		m_Last = nullptr;
		m_Size = 0;
	}

	// access

	Iterator first()
	{ return Iterator(m_First); }

	ConstIterator first() const
	{ return ConstIterator(m_First); }

	Iterator last()
	{ return Iterator(m_Last); }

	ConstIterator last() const
	{ return ConstIterator(m_Last); }

	int size() const
	{ return m_Size; }

	Type& front()
	{
		ErrorAbort(empty(), "List::front() : list is empty");
		return m_First->data;
	}

	const Type& front() const
	{
		ErrorAbort(empty(), "List::front() : list is empty");
		return m_First->data;
	}

	Type& back()
	{
		ErrorAbort(empty(), "List::back() : list is empty");
		return m_Last->data;
	}

	const Type& back() const
	{
		ErrorAbort(empty(), "List::back() : list is empty");
		return m_Last->data;
	}

	// insert

	Iterator insert(const Type& insertionData, Iterator insertionIterator)
	{
		// inserts before given node (pass nullptr to insert at end), returns iterator at inserted node.

		if(empty()) // empty case
		{
			ErrorAbort(insertionIterator.valid(), "List::insert() : must insert using null iterator in empty list");
			m_First = new Node{insertionData};
			m_Last = m_First;
			m_Size++;
			return Iterator(m_First);
		}

		if(!insertionIterator.valid()) // end case
		{
			m_Last->next = new Node{insertionData, m_Last};
			m_Last = m_Last->next;
			m_Size++;
			return Iterator(m_Last);
		}

		// middle case

		Node* insertionNode = insertionIterator.m_Node;
		Node* newNode = new Node{insertionData, insertionNode->prev, insertionNode};

		if(newNode->prev)
			newNode->prev->next = newNode;
		else
			m_First = newNode; // beginning case

		newNode->next->prev = newNode;
		m_Size++;
		return Iterator(newNode);
	}

	void InsertFront(const Type& data)
	{ insert(data, Iterator(m_First)); }

	void InsertBack(const Type& data)
	{ insert(data, Iterator(nullptr)); }

	// remove

	Iterator remove(Iterator removalIterator)
	{
		// removes given node, returns iterator after removed node.
		// exception: list is empty or null iterator is passed

		ErrorAbort(empty(), "List::remove() : list is empty");
		ErrorAbort(!removalIterator.valid(), "List::remove() : invalid iterator");
		
		Node* removalNode = removalIterator.m_Node;

		if(removalNode->prev)
			removalNode->prev->next = removalNode->next;

		if(removalNode->next)
			removalNode->next->prev = removalNode->prev;

		if(removalNode == m_First)
			m_First = m_First->next;

		if(removalNode == m_Last)
			m_Last = m_Last->prev;

		Node* nextNode = removalNode->next;
		delete removalNode;
		m_Size--;
		return Iterator(nextNode);
	}

	void RemoveFront()
	{ remove(Iterator(m_First)); }

	void RemoveBack()
	{ remove(Iterator(m_Last)); }

	// linear search

	Iterator search(const Type& data)
	{
		for(auto it = first(); it.valid(); ++it)
			if(*it == data)
				return it;

		return Iterator(nullptr);
	}

	ConstIterator search(const Type& data) const
	{
		for(auto it = first(); it.valid(); ++it)
			if(*it == data)
				return it;

		return ConstIterator(nullptr);
	}

	// bubble sort

	void SortAscending()
	{
		auto endIt = last();

		for(int pass = 0; pass < m_Size - 1; pass++)
		{
			bool sorted = true;

			for(auto it = first(); it != endIt; ++it)
			{
				if(!(*it < *it.next()))
				{
					Swap(*it, *it.next());
					sorted = false;
				}
			}

			if(sorted)
				break;

			--endIt;
		}
	}

	void SortDescending()
	{
		auto endIt = last();

		for(int pass = 0; pass < m_Size - 1; pass++)
		{
			bool sorted = true;

			for(auto it = first(); it != endIt; ++it)
			{
				if(!(*it > *it.next()))
				{
					Swap(*it, *it.next());
					sorted = false;
				}
			}

			if(sorted)
				break;

			--endIt;
		}
	}
};

// compare

template<typename Type>
bool operator==(const List<Type>& l1, const List<Type>& l2)
{
	if(&l1 == &l2)
		return true;

	if(l1.size() != l2.size())
		return false;

	auto it1 = l1.first();
	auto it2 = l2.first();

	while(it1.valid())
	{
		if(!(*it1 == *it2))
			return false;

		++it1;
		++it2;
	}

	return true;
}

template<typename Type>
bool operator!=(const List<Type>& l1, const List<Type>& l2)
{ return !(l1 == l2); }
