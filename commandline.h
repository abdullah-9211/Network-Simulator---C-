
#pragma once
#include "util.h"
#include "Network.h"
#include "CommandParser.h"

// ======================================================================================================================================================
// Send Message
// ======================================================================================================================================================

Message CreateMessage(const String& msgStr)
{
	auto tokens = msgStr.split(':');

	if(!(tokens.size() == 5 && CommandParser::IsMachineAddress(tokens[2]) && CommandParser::IsMachineAddress(tokens[3])))
	{
		std::cout << "\nFile parsing error, messages could not be sent.\n";
		return {};
	}

	return {StrToInt(tokens[0].data()), StrToInt(tokens[1].data()), tokens[2], tokens[3], tokens[4]};
}

List<Message> ParseMessageFile(const String& filepath)
{
	std::ifstream fin(filepath.data());

	if(!fin)
	{
		std::cout << "\nFile opening error, messages could not be sent.\n";
		return {};
	}

	List<Message> msgList;
	String line;

	while(true)
	{
		char ch = fin.get();

		if(ch == '\n' || !fin)
		{
			msgList.InsertBack(CreateMessage(line));
			line.clear();

			if(!fin)
				break;
		}

		else
			line.InsertBack(ch);
	}

	return msgList;
}

void ExecuteSendMsg(const String& filepath)
{
	auto msgList = ParseMessageFile(filepath);

	if(msgList.empty())
	{
		std::cout << "\nNo Messages Found\n";
		return;
	}

	std::cout << "\nMessages Sent\n";

	for(auto msg = msgList.first(); msg.valid(); ++msg)
		std::cout << "\n[" << msg->ID << ", " << msg->priority << ", " << msg->srcAddress << ", " << msg->dstAddress << ", " << msg->payload << "]";

	std::cout << "\n";
	Network::SendMsg(msgList, "path.txt");
}

// ======================================================================================================================================================
// Change Routing Table
// ======================================================================================================================================================

Network::Router::List Parse_RT_File(const String& filepath)
{
	std::ifstream fin(filepath.data());

	if(!fin)
	{
		std::cout << "\nFile opening error, routing table could not be changed.\n";
		return {};
	}

	Network::Router::List fieldList;
	String line;

	while(true)
	{
		char ch = fin.get();

		if(ch == '\n' || !fin)
		{
			auto tokens = line.split(':');
			bool valid = (tokens.size() == 2) && (CommandParser::IsMachineAddress(tokens[0])) && (CommandParser::IsDeviceAddress(tokens[1]));

			if(!valid)
			{
				std::cout << "\nFile parsing error, routing table could not be changed.\n";
				return {};
			}

			fieldList.InsertBack({tokens[0], tokens[1]});
			line.clear();

			if(!fin)
				break;
		}

		else
			line.InsertBack(ch);
	}

	return fieldList;
}

void ExecuteChangeRT(const String& routerAddress, const String& action, const String& RT_FilePath)
{
	auto fieldList = Parse_RT_File(RT_FilePath);

	if(fieldList.empty())
	{
		std::cout << "\nNo Routing Fields Found\n";
		return;
	}

	Network::PrintRoutingTables();

	if(Network::ChangeRT(routerAddress.upper(), action.lower(), fieldList))
	{
		std::cout << "\nChanged Routing Table Successfully\n";
		EnterToContinue("\nPress enter to see changes...");
		Network::PrintRoutingTables();
	}
	else
		std::cout << "\nFailed to Change Routing Table\n";
}

// ======================================================================================================================================================
// Print Path
// ======================================================================================================================================================

void ExecutePrintPath(const String& srcAddress, const String& dstAddress)
{ Network::PrintPath(srcAddress.upper(), dstAddress.upper(), "path.txt"); }

// ======================================================================================================================================================
// Change Edge
// ======================================================================================================================================================

List<Network::Graph::Edge> ParseChangeEdgeFile(const String& filepath)
{
	// lambdas

	auto openingError = []()
	{
		std::cout << "\nFile opening error, edges could not be changed.\n";
		return List<Network::Graph::Edge>();
	};

	auto parsingError = []()
	{
		std::cout << "\nFile parsing error, edges could not be changed.\n";
		return List<Network::Graph::Edge>();
	};

	auto compareEdges = [](const Network::Graph::Edge& e1, const Network::Graph::Edge& e2)
	{
		return
			e1.indexA == e2.indexA &&
			e1.indexB == e2.indexB &&
			FComp(e1.weight, e2.weight, 0.001);
	};

	// empty graph

	if(Network::Empty())
		return {};

	// open file

	std::ifstream fin(filepath.data());

	if(!fin)
		return openingError();

	// parse file

	List<Network::Graph::Edge> edgeList;
	char ch = '\0';
	String token;
	int rowIndex = -1;
	int colIndex = -1;

	while(true)
	{
		// indices should always be in range

		if(!(InRange(rowIndex, -1, Network::DeviceCount()) && InRange(colIndex, -1, Network::DeviceCount())))
			return parsingError();

		ch = fin.get();

		if(!fin || ch == '\n' || ch == ',')
		{
			if(rowIndex == -1 && token.size() > 1)
			{}

			else if(colIndex != -1 && !token.empty() && token.front() != '?')
			{
				// edge weights should be numbers

				if(!StrIsNum(token.data()))
					return parsingError();

				// if edge is not already present, then change.

				auto edge1Iter = Network::GetEdge(rowIndex, colIndex);

				if(!edge1Iter.valid())
					return parsingError();

				Network::Graph::Edge edge2 = {rowIndex, colIndex, double(int(token.front())) - '0'};

				if(!compareEdges(*edge1Iter, edge2))
					edgeList.InsertBack(edge2);
			}

			token.clear();

			if(ch == ',')
				colIndex++;

			else if(ch == '\n')
			{
				rowIndex++;
				colIndex = -1;
			}

			else if(!fin)
				break;
		}
		else
			token.InsertBack(ch);
	}

	// returns list of edges not already present in the network
	return edgeList;
}

void ExecuteChangeEdge(const String& srcAddress, const String& dstAddress, double edgeWeight)
{
	Network::PrintGraph();
	Network::PrintRoutingTables();

	if(Network::ChangeEdge(srcAddress.upper(), dstAddress.upper(), edgeWeight))
	{
		std::cout << "\nChanged edges successfully, routing tables updated.\n";
		EnterToContinue("\nPress enter to see changes...");
		Network::PrintGraph();
		Network::PrintRoutingTables();
	}
	else
		std::cout << "\nFailed to change edges, could not update routing tables.\n";
}

void ExecuteChangeEdge(const String& filepath)
{
	auto edgeList = ParseChangeEdgeFile(filepath);

	if(edgeList.empty())
	{
		std::cout << "\nNo New Edges Found\n";
		return;
	}

	Network::PrintGraph();
	Network::PrintRoutingTables();

	if(Network::ChangeEdge(edgeList))
	{
		std::cout << "\nChanged edge successfully, routing tables updated.\n";
		EnterToContinue("\nPress enter to see changes...");
		Network::PrintGraph();
		Network::PrintRoutingTables();
	}
	else
		std::cout << "\nFailed to change edge, could not update routing tables.\n";
}

// ======================================================================================================================================================
// User Interface
// ======================================================================================================================================================

int SolveQuery(const String& query)
{
	#if 0
	Network::PrintGraph();
	Network::PrintMap();
	Network::PrintMachineRouters();
	Network::PrintRoutingLists();
	Network::PrintRoutingTrees();
	#endif

	if(simulation::thread)
	{
		if(query.lower() == "q")
		{
			if(simulation::lock_flag)
			{
				simulation::lock.unlock();
				simulation::lock_flag = false;
			}

			simulation::run_flag = false;
			return 0;
		}

		else if(query.lower() == "p")
		{
			if(simulation::lock_flag)
				simulation::lock.unlock();
			else
				simulation::lock.lock();

			simulation::lock_flag = !simulation::lock_flag;
			return 0;
		}
	}

	CommandParser parser(query);

	if(parser.IsExit() && !simulation::thread)
		return 1;

	else if(parser.IsSendMsg())
	{
		// send msg <filename>
		ExecuteSendMsg(parser.GetToken(2));
	}

	else if(parser.IsChangeRT())
	{
		// change rt <device> <action> <filename>
		ExecuteChangeRT(parser.GetToken(2), parser.GetToken(3), parser.GetToken(4));
	}

	else if(parser.IsPrintPath())
	{
		// print path <src> to <dst>
		ExecutePrintPath(parser.GetToken(2), parser.GetToken(4));
	}

	else if(parser.IsChangeEdgeDeviceInput())
	{
		// change edge <src>, <dst>, <weight>

		String src = parser.GetToken(2);
		String dst = parser.GetToken(3);

		src.RemoveBack();
		dst.RemoveBack();

		ExecuteChangeEdge(src, dst, StrToInt(parser.GetToken(4).data()));
	}

	else if(parser.IsChangeEdgeFileInput())
	{
		// change edge <filename>
		ExecuteChangeEdge(parser.GetToken(2));
	}

	else if(!simulation::thread)
		std::cout << "\nCommand Not Found\n";

	return 0;
}

void RunQueries()
{
	while(true)
	{
		if(!simulation::thread)
		{
			std::system("CLS");
			std::cout << "\n---> Enter a query: ";
		}

		String queryInput;
		input(queryInput);

		if(SolveQuery(queryInput) == 1)
			break;

		if(simulation::thread && !simulation::run_flag)
		{
			simulation::thread->join();
			delete simulation::thread;
			simulation::thread = nullptr;
		}

		if(!simulation::thread)
			EnterToContinue("\nPress enter to continue... ");
	}
}

void MainMenu()
{
	std::cout << "\n" << String('=', 100) << "\n";
	std::cout << "\nNetwork Simulator\n";
	std::cout << "\n" << String('=', 100) << "\n";

	std::cout << "\nStructures for Routing Tables\n";
	std::cout << "\n1. Linear Lists\n";
	std::cout << "\n2. Splay Trees\n";

	String inputPrompt = "\n---> Enter your choice: ";
	String errorPrompt = "\n---> Invalid input. Enter again: ";
	int inputChoice = 0;
	bool validChoice = true;

	do
	{
		if(!validChoice)
			inputPrompt = "\n---> Invalid choice. Enter again: ";

		validChoice = true;
		input(inputChoice, inputPrompt.data(), errorPrompt.data());

		if(!InRange(inputChoice, 1, 2))
			validChoice = false;

	} while(!validChoice);

	if(inputChoice == 1)
		Network::Init("Network.csv", Network::Router::TableType::List);
	else if(inputChoice == 2)
		Network::Init("Network.csv", Network::Router::TableType::Tree);

	RunQueries();

	std::cout << "\n" << String('=', 100) << "\n";
	std::cout << "\nThe program has ended\n";
	std::cout << "\n" << String('=', 100) << "\n";
	std::cout << "\n";
}

#define COMMAND_TEST_MODE 0

void RunCommandLine()
{
	#if COMMAND_TEST_MODE

	Network::Init("Network.csv", Network::Router::TableType::Tree);
	// std::ofstream("path.txt");
	// ExecuteSendMsg("message.txt");
	// ExecuteChangeRT("R1", "add", "RT1.csv");
	// ExecutePrintPath("m3", "m5");
	// ExecuteChangeEdge("R1", "R2", 25);
	// ExecuteChangeEdge("Network2.csv");
	;	std::cout << "\n";

	#else

	MainMenu();

	#endif

	Network::Delete();
}

// ======================================================================================================================================================
