
#pragma once
#include "util.h"
#include "String.h"

struct Message
{
	int ID = 0;
	int priority = 0;
	String srcAddress;
	String dstAddress;
	String payload;
	String trace;
};

bool operator<(const Message& m1, const Message& m2)
{ return m1.priority < m2.priority; }

bool operator<=(const Message& m1, const Message& m2)
{ return m1.priority <= m2.priority; }

bool operator>(const Message& m1, const Message& m2)
{ return m1.priority > m2.priority; }

bool operator>=(const Message& m1, const Message& m2)
{ return m1.priority >= m2.priority; }
