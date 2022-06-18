
#pragma once
#include "util.h"
#include "String.h"
#include "Queue.h"
#include "Message.h"
#include "NetworkDevice.h"

class Machine : public NetworkDevice
{
private:

	// members

	Queue<Message> m_InQueue;
	String m_RouterAddress;

public:

	// constructors

	explicit Machine(const String& address)
		: NetworkDevice(address)
	{}

	// getters

	const Queue<Message>& GetInQueue() const
	{ return m_InQueue; }

	const String& GetRouterAddress() const
	{ return m_RouterAddress; }

	// setters

	void SetRouterAddress(const String& routerAddress)
	{ m_RouterAddress = routerAddress; }

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
};
