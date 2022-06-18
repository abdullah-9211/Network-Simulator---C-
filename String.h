
#pragma once
#include "util.h"
#include "Array.h"

class String
{
private:

	// members

	Array<char> m_Data = Array<char>(1);

public:

	// constructors

	String() = default;

	explicit String(int size)
		: m_Data(Array<char>(size + 1))
	{}

	explicit String(char ch, int size)
		: m_Data(Array<char>(size + 1, ch))
	{ m_Data.back() = '\0'; }

	String(const char* str)
		: m_Data(Array<char>(StrLen(str) + 1))
	{ CopyArray(data(), str, size()); }

	// empty state

	bool empty() const
	{ return size() == 0; }

	void clear()
	{ m_Data = Array<char>(1); }

	// access

	char* data()
	{ return m_Data.data(); }

	const char* data() const
	{ return m_Data.data(); }

	int size() const
	{ return m_Data.size() - 1; }

	int length() const
	{ return size(); }

	char& operator[](int index)
	{
		ErrorAbort(!InRange(index, 0, size() - 1), "String::operator[]() : index out of bounds");
		return m_Data.data()[index];
	}

	char operator[](int index) const
	{
		ErrorAbort(!InRange(index, 0, size() - 1), "String::operator[]() : index out of bounds");
		return m_Data.data()[index];
	}

	char& front()
	{
		ErrorAbort(empty(), "String::front() : string is empty");
		return m_Data.data()[0];
	}

	char front() const
	{
		ErrorAbort(empty(), "String::front() : string is empty");
		return m_Data.data()[0];
	}

	char& back()
	{
		ErrorAbort(empty(), "String::back() : string is empty");
		return m_Data.data()[size() - 1];
	}

	char back() const
	{
		ErrorAbort(empty(), "String::back() : string is empty");
		return m_Data.data()[size() - 1];
	}

	// insert

	void insert(char ch, int index)
	{
		ErrorAbort(!InRange(index, 0, size()), "String::insert() : index out of bounds");
		m_Data.insert(ch, index);
	}

	void InsertFront(char ch)
	{ insert(ch, 0); }

	void InsertBack(char ch)
	{ insert(ch, size()); }

	// remove

	void remove(int index)
	{
		ErrorAbort(!InRange(index, 0, size() - 1), "String::remove() : index out of bounds");
		m_Data.remove(index);
	}

	void RemoveFront()
	{ remove(0); }

	void RemoveBack()
	{ remove(size() - 1); }

	// search

	int search(char ch) const
	{ return SearchArray(data(), size(), ch); }

	// compare

	friend bool operator==(const String& s1, const String& s2)
	{ return s1.m_Data == s2.m_Data; }

	friend bool operator!=(const String& s1, const String& s2)
	{ return !(s1 == s2); }

	friend bool operator<(const String& s1, const String& s2)
	{ return s1.m_Data < s2.m_Data; }

	friend bool operator<=(const String& s1, const String& s2)
	{ return s1.m_Data <= s2.m_Data; }

	friend bool operator>(const String& s1, const String& s2)
	{ return s2 < s1; }

	friend bool operator>=(const String& s1, const String& s2)
	{ return s2 <= s1; }

	// concatenate

	String& operator+=(const String&);

	// sub-string

	String substr(int start, int end = -1) const
	{
		if(end == -1)
			end = size() - 1;

		ErrorAbort(!(InRange(start, 0, size() - 1) && InRange(end, 0, size() - 1) && (start <= end)), "String::substr() : invalid index");
		String result((end - start) + 1);
		CopyArray(result.data(), data() + start, result.size());
		return result;
	}

	// lowercase and uppercase

	String lower() const
	{
		String lowerStr = *this;

		for(int index = 0; index < size(); index++)
			lowerStr[index] = ToLower(lowerStr[index]);

		return lowerStr;
	}

	String upper() const
	{
		String upperStr = *this;

		for(int index = 0; index < size(); index++)
			upperStr[index] = ToUpper(upperStr[index]);

		return upperStr;
	}

	// split

	Array<String> split(char delim = ' ') const
	{
		Array<String> tokenArray;
		String token;

		for(int index = 0; index < size(); index++)
		{
			bool isLast = (index == size() - 1);
			bool isDelim = (m_Data[index] == delim);

			if(!isDelim)
				token.InsertBack(m_Data[index]);

			if(isDelim || isLast)
			{
				tokenArray.InsertBack(token);
				token.clear();
			}
		}

		return tokenArray;
	}
};

// concatenate

String operator+(const String& s1, const String& s2)
{
	String result(s1.size() + s2.size());
	CopyArray(result.data(), s1.data(), s1.size());
	CopyArray(result.data() + s1.size(), s2.data(), s2.size());
	return result;
}

String& String::operator+=(const String& other)
{ return *this = *this + other; }

// input

void input(String& str, const char* msg = nullptr)
{
	if(msg)
		std::cout << msg;

	const int maxInputSize = 1024;
	char tempInput[maxInputSize]{};

	do
	{
		std::cin.getline(tempInput, maxInputSize);
		str = tempInput;
	
	} while(str.empty());
}

// output

std::ostream& operator<<(std::ostream& out, const String& str)
{ return out << str.data(); }
