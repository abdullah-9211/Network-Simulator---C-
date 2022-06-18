
#pragma once
#include "util.h"
#include "Array.h"
#include "String.h"

class CommandParser
{
private:

	// members

	String m_Command;
	Array<String> m_Tokens;

public:

	// constructors

	CommandParser() = default;

	explicit CommandParser(const String& command)
		: m_Command(command.lower()), m_Tokens(m_Command.split())
	{}

	// getters

	const String& GetCommand() const
	{ return m_Command; }

	const String& GetToken(int index) const
	{
		ErrorAbort(!InRange(index, 0, m_Tokens.size() - 1), "CommandParser::GetToken() : index out of bounds");
		return m_Tokens[index];
	}

	// setters

	void SetCommand(const String& command)
	{
		m_Command = command.lower();
		m_Tokens = m_Command.split();
	}

	// parsing methods

	bool IsExit() const
	{ return m_Command == "exit"; }

	bool IsSendMsg() const
	{
		return
			m_Tokens.size() == 3 &&
			m_Tokens[0] == "send" &&
			m_Tokens[1] == "msg" &&
			IsFileName(m_Tokens[2]);
	}

	bool IsChangeRT() const
	{
		return
			m_Tokens.size() == 5 &&
			m_Tokens[0] == "change" &&
			m_Tokens[1] == "rt" &&
			IsRouterAddress(m_Tokens[2]) &&
			(m_Tokens[3] == "add" || m_Tokens[3] == "remove") &&
			IsFileName(m_Tokens[4]);
	}

	bool IsPrintPath() const
	{
		return
			m_Tokens.size() == 5 &&
			m_Tokens[0] == "print" &&
			m_Tokens[1] == "path" &&
			(m_Tokens[2] == "*" || IsMachineAddress(m_Tokens[2])) &&
			m_Tokens[3] == "to" &&
			(m_Tokens[4] == "*" || IsMachineAddress(m_Tokens[4]));
	}

	bool IsChangeEdgeDeviceInput() const
	{
		bool valid =
			m_Tokens.size() >= 3 &&
			m_Tokens[0] == "change" &&
			m_Tokens[1] == "edge";

		if(m_Tokens.size() == 5)
		{
			String deviceA = m_Tokens[2];
			String deviceB = m_Tokens[3];

			valid = valid &&
				(!deviceA.empty() && deviceA.back() == ',') &&
				(!deviceB.empty() && deviceB.back() == ',');

			if(valid)
			{
				deviceA.RemoveBack();
				deviceB.RemoveBack();
			}
			else return false;

			String typeA = DeviceType(deviceA);
			String typeB = DeviceType(deviceB);

			valid = valid &&
				(typeA == "machine" && typeB == "router") ||
				(typeA == "router" && typeB == "machine") ||
				(typeA == "router" && typeB == "router");

			valid = valid && StrIsNum(m_Tokens[4].data());
		}

		else return false;
		return valid;
	}

	bool IsChangeEdgeFileInput() const
	{
		bool valid =
			m_Tokens.size() >= 3 &&
			m_Tokens[0] == "change" &&
			m_Tokens[1] == "edge";

		if(m_Tokens.size() == 3)
			valid = valid && IsFileName(m_Tokens[2]);

		return valid;
	}

	bool IsChangeEdge() const
	{ return IsChangeEdgeDeviceInput() || IsChangeEdgeFileInput(); }

	// utility methods

	static bool IsExtension(const String& str)
	{
		return
			str == ".txt" ||
			str == ".csv";
	}

	static bool IsFileName(const String& str)
	{ return str.size() >= 5 && IsExtension(str.substr(str.size() - 4)); }

	static bool IsDeviceAddress(const String& str)
	{ return IsMachineAddress(str) || IsRouterAddress(str); }

	static bool IsMachineAddress(const String& str)
	{
		return
			str.size() == 2 &&
			ToLower(str[0]) == 'm' &&
			IsDigit(str[1]);
	}

	static bool IsRouterAddress(const String& str)
	{
		return
			str.size() == 2 &&
			ToLower(str[0]) == 'r' &&
			IsDigit(str[1]);
	}

	static String DeviceType(const String& str)
	{
		if(IsMachineAddress(str))
			return "machine";

		else if(IsRouterAddress(str))
			return "router";

		else
			return "none";
	}
};
