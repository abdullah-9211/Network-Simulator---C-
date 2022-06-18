
#pragma once
#include "util.h"
#include "String.h"
#include "List.h"
#include "PriorityQueue.h"
#include "SplayTree.h"
#include "Message.h"
#include "NetworkDevice.h"

class Router : public NetworkDevice
{
public:

	// types

	struct Field
	{
		// members

		String destAddress;
		String nextAddress;

		// compare

		friend bool operator==(const Field& f1, const Field& f2)
		{ return (f1.destAddress == f2.destAddress) && (f1.nextAddress == f2.nextAddress); }

		friend bool operator!=(const Field& f1, const Field& f2)
		{ return !(f1 == f2); }
	};

	using List = List<Field>;
	using Tree = SplayTree<String, String>;
	using PriorityQueue = PriorityQueue<GreaterEqual<Message>>;
	enum class TableType {List, Tree};

private:

	// members

	Router::PriorityQueue m_InQueue;
	Router::List m_RoutingList;
	Router::Tree m_RoutingTree;

public:

	// constructors

	explicit Router(const String& address)
		: NetworkDevice(address)
	{}

	// getters

	const Router::PriorityQueue& GetInQueue() const
	{ return m_InQueue; }

	const Router::List& GetRoutingList() const
	{ return m_RoutingList; }

	const Router::Tree& GetRoutingTree() const
	{ return m_RoutingTree; }

	// setters

	void SetRoutingList(const Router::List& routingList)
	{ m_RoutingList = routingList; }

	void SetRoutingTree(const Router::Tree& routingTree)
	{ m_RoutingTree = routingTree; }

	// fields

	void InsertField(const Field& insertionField, TableType tableType)
	{
		if(tableType == TableType::List)
		{
			// if destination address is already present, change its corresponding next address.

			for(auto field = m_RoutingList.first(); field.valid(); ++field)
			{
				if(field->destAddress == insertionField.destAddress)
				{
					field->nextAddress = insertionField.nextAddress;
					return;
				}
			}

			// if not present, add it along with it corresponding next address.

			m_RoutingList.InsertBack(insertionField);
			return;
		}

		else if(tableType == TableType::Tree)
		{
			// same approach as list insertion

			if(auto field = m_RoutingTree.search(insertionField.destAddress))
			{
				*field = insertionField.nextAddress;
				return;
			}

			m_RoutingTree.insert({insertionField.destAddress, insertionField.nextAddress});
			return;
		}
	}

	bool RemoveField(const Field& removalField, TableType tableType)
	{
		if(tableType == TableType::List)
		{
			auto field = m_RoutingList.search(removalField);

			if(!field.valid())
				return false;

			else
			{
				m_RoutingList.remove(field);
				return true;
			}
		}

		else if(tableType == TableType::Tree)
			return m_RoutingTree.remove(removalField.destAddress);

		else 
			return false;
	}

	// messages

	void InsertMessage(const Message& msg) override
	{ m_InQueue.enqueue(msg); }

	bool ReadMessage() override
	{
		if(!m_InQueue.empty())
		{
			m_OutQueue.enqueue(m_InQueue.front());
			m_InQueue.dequeue();
			return true;
		}
		else return false;
	}

	bool RemoveMessage() override
	{
		if(!m_OutQueue.empty())
		{
			m_OutQueue.dequeue();
			return true;
		}
		else return false;
	}

	// routing decision

	String RoutingDecision(const String& destAddress, TableType tableType) const
	{
		if(tableType == TableType::List)
		{
			for(auto field = m_RoutingList.first(); field.valid(); ++field)
				if(field->destAddress == destAddress)
					return field->nextAddress;
		}

		else if(tableType == TableType::Tree)
		{
			if(auto search = m_RoutingTree.search(destAddress))
				return *search;
		}

		return {};
	}
};
