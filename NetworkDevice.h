
#pragma once
#include "util.h"
#include "String.h"
#include "Queue.h"
#include "Message.h"

class NetworkDevice
{
protected:

	// members

	String m_Address;
	Queue<Message> m_OutQueue;

public:

	// constructors

	explicit NetworkDevice(const String& address)
		: m_Address(address)
	{}

	// getters

	const String& GetAddress() const
	{ return m_Address; }

	const Queue<Message>& GetOutQueue() const
	{ return m_OutQueue; }

	// setters

	void SetAddress(const String& address)
	{ m_Address = address; }

	// messages

	virtual void InsertMessage(const Message&) = 0;
	virtual bool ReadMessage() = 0;
	virtual bool RemoveMessage() = 0;

	// destructor

	virtual ~NetworkDevice() = default;
};
