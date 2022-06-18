
#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
using namespace std::literals::chrono_literals;

// assert-exception

void ErrorAbort(bool condition, const char* message)
{
	if(condition)
	{
		std::cout << message;
		std::abort();
	}
}

// numbers

bool InRange(int num, int min, int max)
{ return num >= min && num <= max; }

// characters

bool IsDigit(char ch)
{ return InRange(ch, '0', '9'); }

bool IsUpper(char ch)
{ return InRange(ch, 'A', 'Z'); }

bool IsLower(char ch)
{ return InRange(ch, 'a', 'z'); }

bool IsAlpha(char ch)
{ return IsUpper(ch) || IsLower(ch); }

char ToLower(char ch)
{ return IsUpper(ch) ? ch + ('a' - 'A') : ch; }

char ToUpper(char ch)
{ return IsLower(ch) ? ch - ('a' - 'A') : ch; }

// strings

int StrLen(const char* str)
{
	int len = 0;

	while(str[len])
		len++;

	return len;
}

bool StrIsNum(const char* str)
{
	for(int index = 0; str[index]; index++)
		if(!(IsDigit(str[index]) || (index == 0 && str[index] == '-')))
			return false;

	return true;
}

int StrToInt(const char* str)
{
	ErrorAbort(!StrIsNum(str), "StrToInt() : string is not a number");

	int num = 0;

	for(int index = 0; str[index]; index++)
		num = (num * 10) + (str[index] - '0');

	return num;
}

// templates

template<typename NumType>
NumType Abs(NumType num)
{ return num < 0 ? -num : num; }

template<typename Type>
Type Min(const Type& a, const Type& b)
{ return a < b ? a : b; }

template<typename Type>
Type Max(const Type& a, const Type& b)
{ return a > b ? a : b; }

template<typename Type>
void Swap(Type& a, Type& b)
{
	Type t = a;
	a = b;
	b = t;
}

template<typename FloatType>
bool FComp(FloatType a, FloatType b, FloatType epsilon)
{ return Abs(a - b) < epsilon; }

// arrays

template<typename Type>
void CopyArray(Type* dst, const Type* src, int size)
{
	for(int index = 0; index < size; index++)
		dst[index] = src[index];
}

template<typename Type>
void FillArray(Type* array, int size, const Type& data)
{
	for(int index = 0; index < size; index++)
		array[index] = data;
}

template<typename Type>
int SearchArray(const Type* array, int size, const Type& data)
{
	for(int index = 0; index < size; index++)
		if(array[index] == data)
			return index;

	return -1;
}

template<typename Type>
int CompareArray(const Type* a1, const Type* a2, int s1, int s2)
{
	// returns -1 if a1 < a2, 0 if a1 == a2, 1 if a1 > a2.

	int i1 = 0, i2 = 0;

	while(true)
	{
		if(i1 == s1 || i2 == s2) // reached end
		{
			if(i1 == s1 && i2 == s2)
				return 0; // equal (both reached end)

			else if(i1 == s1)
				return -1; // less (left reached end first)

			else
				return 1; // greater (right reached end first)
		}

		if(a1[i1] < a2[i2])
			return -1;

		else if(a1[i1] > a2[i2])
			return 1;

		else // check next element if equal
		{
			i1++;
			i2++;
		}
	}
}

template<typename Type>
void SortArrayAscending(Type* array, int size)
{
	for(int pass = 0; pass < size - 1; pass++)
	{
		bool sorted = true;

		for(int index = 0; index < size - 1 - pass; index++)
		{
			if(!(array[index] < array[index + 1]))
			{
				Swap(array[index], array[index + 1]);
				sorted = false;
			}
		}

		if(sorted)
			break;
	}
}

template<typename Type>
void SortArrayDescending(Type* array, int size)
{
	for(int pass = 0; pass < size - 1; pass++)
	{
		bool sorted = true;

		for(int index = 0; index < size - 1 - pass; index++)
		{
			if(!(array[index] > array[index + 1]))
			{
				Swap(array[index], array[index + 1]);
				sorted = false;
			}
		}

		if(sorted)
			break;
	}
}

template<typename Type>
void ShiftArrayLeft(Type* array, int size)
{
	for(int index = 0; index < size - 1; index++)
		array[index] = array[index + 1];

	array[size - 1] = Type();
}

template<typename Type>
void ShiftArrayRight(Type* array, int size)
{
	for(int index = size - 1; index > 0; index--)
		array[index] = array[index - 1];

	array[0] = Type();
}

// input

template<typename Type>
void input(Type& data, const char* inputPrompt = nullptr, const char* errorPrompt = nullptr)
{
	if(inputPrompt)
		std::cout << inputPrompt;

	bool valid = true;

	do
	{
		if(errorPrompt && !valid)
			std::cout << errorPrompt;

		std::cin >> data;
		valid = std::cin.good();
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	} while(!valid);
}

void EnterToContinue(const char* msg = nullptr)
{
	if(msg)
		std::cout << msg;

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// comparators

template<typename Type>
struct Comparator
{
	using ValueType = Type;
	virtual bool compare(const Type&, const Type&) const = 0;
};

template<typename Type>
struct Equal : Comparator<Type>
{
	using typename Comparator<Type>::ValueType;
	bool compare(const Type& a, const Type& b) const override
	{ return a == b; }
};

template<typename Type>
struct NotEqual : Comparator<Type>
{
	using typename Comparator<Type>::ValueType;
	bool compare(const Type& a, const Type& b) const override
	{ return a != b; }
};

template<typename Type>
struct Lesser : Comparator<Type>
{
	using typename Comparator<Type>::ValueType;
	bool compare(const Type& a, const Type& b) const override
	{ return a < b; }
};

template<typename Type>
struct LesserEqual : Comparator<Type>
{
	using typename Comparator<Type>::ValueType;
	bool compare(const Type& a, const Type& b) const override
	{ return a <= b; }
};

template<typename Type>
struct Greater : Comparator<Type>
{
	using typename Comparator<Type>::ValueType;
	bool compare(const Type& a, const Type& b) const override
	{ return a > b; }
};

template<typename Type>
struct GreaterEqual : Comparator<Type>
{
	using typename Comparator<Type>::ValueType;
	bool compare(const Type& a, const Type& b) const override
	{ return a >= b; }
};

// multi-threading

namespace simulation
{
	std::thread* thread = nullptr;
	std::mutex lock;
	bool run_flag = false;
	bool lock_flag = false;
};
