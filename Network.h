
#pragma once
#include "util.h"
#include "Pair.h"
#include "Array.h"
#include "String.h"
#include "List.h"
#include "Stack.h"
#include "Queue.h"
#include "PriorityQueue.h"
#include "SplayTree.h"
#include "Graph.h"
#include "Message.h"
#include "NetworkDevice.h"
#include "Machine.h"
#include "Router.h"

#define PRINT_SHORTEST_PATH_TABLE 0

class Network
{
public:

	// types

	using Device = NetworkDevice;
	using Machine = Machine;
	using Router = Router;
	using Graph = Graph<Device*>;
	using Map = SplayTree<String, int>;

private:

	// members

	Graph m_Graph;
	Map m_Map;
	Router::TableType m_RoutingTableType = Router::TableType();

	// constructors

	Network() = default;
	Network(const Network&) = delete;
	Network& operator=(const Network&) = delete;

public:

	// empty state

	static bool Empty()
	{ return Instance().m_Graph.empty() && Instance().m_Map.empty(); }

	// init

	static void Init(const String& filepath, Router::TableType routingTableType)
	{
		Create(filepath, routingTableType);
		FindShortestPaths();
	}

	// create and delete

	static void Create(const String& filepath, Router::TableType routingTableType)
	{ Instance().CreateImpl(filepath, routingTableType); }
	
	static void Delete()
	{ Instance().DeleteImpl(); }

	// getters

	static int DeviceCount()
	{ return Instance().m_Graph.VertexCount(); }

	static Device* GetDevice(int deviceIndex)
	{
		ErrorAbort(!InRange(deviceIndex, 0, DeviceCount() - 1), "Network::GetDevice() : index out of bounds");
		return Instance().m_Graph.GetData(deviceIndex);
	}

	static Machine* GetMachine(int machineIndex)
	{
		ErrorAbort(!InRange(machineIndex, 0, DeviceCount() - 1), "Network::GetMachine() : index out of bounds");
		return DeviceToMachine(GetDevice(machineIndex));
	}

	static Router* GetRouter(int routerIndex)
	{
		ErrorAbort(!InRange(routerIndex, 0, DeviceCount() - 1), "Network::GetRouter() : index out of bounds");
		return DeviceToRouter(GetDevice(routerIndex));
	}

	static Device* GetDevice(const String& deviceAddress)
	{
		auto search = Instance().m_Map.search(deviceAddress);
		return search ? GetDevice(*search) : nullptr;
	}

	static Machine* GetMachine(const String& machineAddress)
	{ return DeviceToMachine(GetDevice(machineAddress)); }

	static Router* GetRouter(const String& routerAddress)
	{ return DeviceToRouter(GetDevice(routerAddress)); }

	static List<Graph::Edge>::ConstIterator GetEdge(int indexA, int indexB)
	{
		ErrorAbort(!(InRange(indexA, 0, DeviceCount() - 1) && InRange(indexB, 0, DeviceCount() - 1)), "Network::GetEdge() : index out of bounds");
		return Instance().m_Graph.GetEdge(indexA, indexB);
	}

	// print

	static void PrintGraph()
	{ Instance().PrintGraphImpl(); }

	static void PrintMap()
	{
		std::cout << "\nDevice Map\n";
		Instance().m_Map.PrintInOrder();
		std::cout << "\n";
	}

	static void PrintMachineRouters()
	{ Instance().PrintMachineRoutersImpl(); }

	static void PrintRoutingLists()
	{ Instance().PrintRoutingListsImpl(); }

	static void PrintRoutingTrees()
	{ Instance().PrintRoutingTreesImpl(); }

	static void PrintRoutingTables()
	{
		if(Instance().m_RoutingTableType == Router::TableType::List)
			PrintRoutingLists();

		else if(Instance().m_RoutingTableType == Router::TableType::Tree)
			PrintRoutingTrees();
	}

	// shortest paths

	static void FindShortestPaths()
	{ Instance().FindShortestPathsImpl(); }

	// commandline

	static void SendMsg(const List<Message>& msgList, const String& filepath)
	{ 
		simulation::run_flag = true;
		simulation::thread = new std::thread(&Network::SendMsgImpl, &Instance(), msgList, filepath);
	}

	static bool ChangeRT(const String& routerAddress, const String& action, const Router::List& fieldList)
	{ return Instance().ChangeRT_Impl(routerAddress, action, fieldList); }

	static void PrintPath(const String& srcAddress, const String& dstAddress, const String& filepath)
	{ Instance().PrintPathImpl(srcAddress, dstAddress, filepath); }

	static bool ChangeEdge(const String& srcAddress, const String& dstAddress, double edgeWeight)
	{ 
		if(!Instance().ChangeEdgeImpl(srcAddress, dstAddress, edgeWeight))
			return false;

		FindShortestPaths();
		return true;
	}

	static bool ChangeEdge(const List<Graph::Edge>& edgeList)
	{
		if(!Instance().ChangeEdgeImpl(edgeList))
			return false;

		FindShortestPaths();
		return true;
	}

private:

	// instance

	static Network& Instance()
	{
		static Network network;
		return network;
	}

	// type conversion

	static Machine* DeviceToMachine(Device* device)
	{ return dynamic_cast<Machine*>(device); }

	static const Machine* DeviceToMachine(const Device* device)
	{ return dynamic_cast<const Machine*>(device); }

	static Router* DeviceToRouter(Device* device)
	{ return dynamic_cast<Router*>(device); }

	static const Router* DeviceToRouter(const Device* device)
	{ return dynamic_cast<const Router*>(device); }

	// create and delete implementation

	void CreateImpl(const String& filepath, Router::TableType routingTableType)
	{
		std::ifstream fin(filepath.data());
		ErrorAbort(!fin, "Network::CreateImpl() : failed to open file");

		char ch = '\0';
		String token;
		int rowIndex = -1;
		int colIndex = -1;

		while(true)
		{
			ch = fin.get();

			if(!fin || ch == '\n' || ch == ',')
			{
				if(rowIndex == -1 && token.size() > 1)
				{
					Device* device = nullptr;

					if(ToUpper(token.front()) == 'M')
						device = new Machine(token);

					else if(ToUpper(token.front()) == 'R')
						device = new Router(token);

					else
						ErrorAbort(true, "Network::CreateImpl() : invalid network device type");

					m_Graph.InsertVertex(device);

					if(!m_Map.insert({token, m_Graph.VertexCount() - 1}))
					{
						delete device;
						ErrorAbort(true, "Network::CreateImpl() : duplicate device inserted");
					}
				}

				else if(colIndex != -1 && !token.empty() && token.front() != '?')
				{
					ErrorAbort(!StrIsNum(token.data()), "Network::CreateImpl() : invalid edge weight");
					m_Graph.InsertEdge(rowIndex, colIndex, double(int(token.front())) - '0');
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

		m_RoutingTableType = routingTableType;
	}

	void DeleteImpl()
	{
		for(int index = 0; index < m_Graph.VertexCount(); index++)
		{
			auto& vertex = m_Graph.GetVertex(index);
			delete vertex.data;
			vertex.data = nullptr;
			vertex.edges.clear();
		}

		m_Graph.clear();
		m_Map.clear();
	}

	// print implementation

	void PrintGraphImpl() const
	{
		std::cout << "\nNetwork Adjacency\n";

		for(int index = 0; index < m_Graph.VertexCount(); index++)
		{
			const auto& vertex = m_Graph.GetVertex(index);
			std::cout << "\n[" << vertex.data << ", " << vertex.data->GetAddress() << "]: ";

			for(auto edge = vertex.edges.first(); edge.valid(); ++edge)
			{
				std::cout << "[" << m_Graph.GetData(edge->indexB)->GetAddress() << ", " << edge->weight << "]";

				if(edge != vertex.edges.last())
					std::cout << " -> ";
			}
		}

		if(!Empty())
			std::cout << "\n";
	}

	void PrintMachineRoutersImpl() const
	{
		std::cout << "\nMachine Routers\n";
		bool oneFound = false;

		for(int index = 0; index < DeviceCount(); index++)
		{
			if(Machine* machine = GetMachine(index))
			{
				std::cout << "\n[" << machine->GetAddress() << ", " << machine->GetRouterAddress() << "]";
				oneFound = true;
			}
		}

		if(oneFound)
			std::cout << "\n";
	}

	void PrintRoutingListsImpl() const
	{
		for(int index = 0; index < DeviceCount(); index++)
		{
			if(Router* router = GetRouter(index))
			{
				std::cout << "\n" << router->GetAddress() << " Routing List\n";
				const auto& list = router->GetRoutingList();

				for(auto field = list.first(); field.valid(); ++field)
					std::cout << "\n[" << field->destAddress << ", " << field->nextAddress << "]";

				if(!list.empty())
					std::cout << "\n";
			}
		}
	}

	void PrintRoutingTreesImpl() const
	{
		for(int index = 0; index < DeviceCount(); index++)
		{
			if(Router* router = GetRouter(index))
			{
				std::cout << "\n" << router->GetAddress() << " Routing Tree\n";
				const auto& tree = router->GetRoutingTree();
				tree.PrintInOrder();

				if(!tree.empty())
					std::cout << "\n";
			}
		}
	}

	// shortest paths implementation

	static void MakePathList(const Array<int>& parentArray, int vertexIndex, List<int>& pathList)
	{
		int parentIndex = parentArray[vertexIndex];

		if(parentIndex != -1)
		{
			pathList.InsertFront(parentIndex);
			MakePathList(parentArray, parentIndex, pathList);
		}
	}

	void FindShortestPathsImpl(int startIndex)
	{
		// exceptions
		ErrorAbort(!InRange(startIndex, 0, DeviceCount() - 1), "Network::FindShortestPathsImpl() : index out of bounds");

		Router* startRouter = GetRouter(startIndex);
		ErrorAbort(startRouter == nullptr, "Network::FindShortestPathsImpl() : starting device is non-router");

		// distance of each vertex from source (source initialized to 0, all else to infinity)
		constexpr double infinity = std::numeric_limits<double>::max();
		Array<double> distances(DeviceCount(), infinity);
		distances[startIndex] = 0;

		// parent of each vertex
		Array<int> parents(DeviceCount(), -1);

		// TODO: use build-heap method
		PriorityQueue<LesserEqual<Pair<double, int>>> priorityQueue;

		for(int index = 0; index < DeviceCount(); index++)
			priorityQueue.enqueue({distances[index], index});

		// run until n devices are checked
		for(int count = 0; count < DeviceCount(); count++)
		{
			// extract minimum distance
			double currentDistance = priorityQueue.front().first;
			int currentIndex = priorityQueue.front().second;
			const auto& currentVertex = m_Graph.GetVertex(currentIndex);
			priorityQueue.dequeue();

			for(auto edge = currentVertex.edges.first(); edge.valid(); ++edge)
			{
				// get neighbour data
				double edgeWeight = edge->weight;
				int neighbourIndex = edge->indexB;
				double& neighbourDistance = distances[neighbourIndex];

				// update distance and parent if shorter path found
				if((currentDistance + edgeWeight) < neighbourDistance)
				{
					neighbourDistance = (currentDistance + edgeWeight);
					parents[neighbourIndex] = currentIndex;
					priorityQueue.enqueue({neighbourDistance, neighbourIndex});
					// this creates duplicates in queue
					// but they will not be checked later redundantly
					// because we will stop at n devices
				}
			}
		}

		// fill routing lists and trees
		{
			Router::TableType tableType = m_RoutingTableType;
			Router::List routingList;
			Router::Tree routingTree;

			for(int index = 0; index < DeviceCount(); index++)
			{
				// ignore router to router paths
				Machine* machine = GetMachine(index);

				if(!machine)
					continue;

				// find full path from source router to destination machine
				List<int> path;
				path.InsertFront(index);
				MakePathList(parents, index, path);

				// ignore if only start router itself exists in the path
				if(path.size() == 1)
					continue;

				// insert routing fields
				Device* nextDevice = GetDevice(*path.first().next());
				Router::Field field = {machine->GetAddress(), nextDevice->GetAddress()};
				
				if(tableType == Router::TableType::List)
					routingList.InsertBack(field);

				else if(tableType == Router::TableType::Tree)
					routingTree.insert({field.destAddress, field.nextAddress});
			}

			// insert routing tables
			if(tableType == Router::TableType::List)
				startRouter->SetRoutingList(routingList);

			else if(tableType == Router::TableType::Tree)
				startRouter->SetRoutingTree(routingTree);
		}

		#if PRINT_SHORTEST_PATH_TABLE

		// print results table

		for(int index = 0; index < DeviceCount(); index++)
		{
			// device

			Device* device = GetDevice(index);
			std::cout << device->GetAddress() << "\t\t";

			// distance

			if(distances[index] == infinity)
				std::cout << "inf";
			else
				std::cout << distances[index];

			// parent

			if(parents[index] == -1)
				std::cout << "\t\t" << "None" << "\t\t";
			else
			{
				device = GetDevice(parents[index]);
				std::cout << "\t\t" << device->GetAddress() << "\t\t";
			}

			// path

			List<int> path;
			path.InsertFront(index);
			MakePathList(parents, index, path);

			for(auto deviceIndex = path.first(); deviceIndex.valid(); ++deviceIndex)
			{
				device = GetDevice(*deviceIndex);
				std::cout << device->GetAddress();
				
				if(deviceIndex != path.last())
					std::cout << " -> ";
			}

			std::cout << "\n";
		}

		#endif
	}

	void FindShortestPathsImpl()
	{
		for(int index = 0; index < m_Graph.VertexCount(); index++)
		{
			auto& vertex = m_Graph.GetVertex(index);
			Device* device = vertex.data;

			if(Machine* machine = DeviceToMachine(device))
			{
				ErrorAbort(vertex.edges.size() != 1, "Network::FindShortestPathsImpl() : machines must be connected to one router only");
				int machineRouterIndex = vertex.edges.first()->indexB;
				Router* machineRouter = GetRouter(machineRouterIndex);
				machine->SetRouterAddress(machineRouter->GetAddress());
			}

			else if(Router* router = DeviceToRouter(device))
			{
				#if PRINT_SHORTEST_PATH_TABLE

				std::cout << "\n" << device->GetAddress() << " Paths\n";
				std::cout << "\nAddress\t\tDistance\tParent\t\tPath\n\n";

				#endif

				FindShortestPathsImpl(index);
			}
		}
	}

	// send msg implementation

	void SendMsgImpl(List<Message> msgList, const String& filepath)
	{
		std::cout << "\nMessage Transfer Log\n";
		SendMsgInit(msgList);

		while(simulation::run_flag && !SendMsgFinished())
		{
			SendMsgCycle(filepath);
		}

		simulation::run_flag = false;
	}

	void SendMsgInit(List<Message>& msgList)
	{
		// enqueue messages in incoming queues of source machines

		for(auto msg = msgList.first(); msg.valid(); ++msg)
		{
			if(Machine* machine = GetMachine(msg->srcAddress))
			{
				msg->trace = msg->srcAddress;
				machine->InsertMessage(*msg);
				std::cout << "\nInserted message " << msg->ID << " in " << machine->GetAddress();
			}
			else ErrorAbort(true, "Network::SendMsgInit() : machine not found");
		}

		if(!msgList.empty())
			std::cout << "\n";
	}

	void SendMsgCycle(const String& filepath)
	{
		simulation::lock.lock();
		simulation::lock_flag = true;

		for(int deviceIndex = 0; deviceIndex < DeviceCount(); deviceIndex++)
		{
			if(Machine* machine = GetMachine(deviceIndex))
			{
				if(machine->ReadMessage())
				{
					// machine in-queue to out-queue
					Message msg = machine->GetOutQueue().back();
					std::cout << "\n" << machine->GetAddress() << " picked up message " << msg.ID;
					
					// machine out-queue to router in-queue
					if(msg.srcAddress == machine->GetAddress())
					{
						Router* router = GetRouter(machine->GetRouterAddress());
						machine->RemoveMessage();
						msg.trace += (":" + router->GetAddress());
						router->InsertMessage(msg);
						std::cout << "\n" << machine->GetAddress() << " transferred message " << msg.ID << " to " << router->GetAddress() << "\n";
						
						simulation::lock.unlock();
						simulation::lock_flag = false;
						std::this_thread::sleep_for(1s);
						simulation::lock.lock();
						simulation::lock_flag = true;
					}

					// message reached destination machine
					else if(msg.dstAddress == machine->GetAddress())
					{
						machine->RemoveMessage();
						std::cout << "\n" << machine->GetAddress() << " received message " << msg.ID << " from " << msg.trace << " \"" << msg.payload << "\"\n";
						SendMsgWritePath(filepath, msg);
					}
				}
			}

			else if(Router* router = GetRouter(deviceIndex))
			{
				if(router->ReadMessage())
				{
					// router in-queue to out-queue
					Message msg = router->GetOutQueue().back();
					std::cout << "\n" << router->GetAddress() << " picked up message " << msg.ID;

					// router out-queue to next device in-queue
					Device* nextDevice = GetDevice(router->RoutingDecision(msg.dstAddress, m_RoutingTableType));
					router->RemoveMessage();
					msg.trace += (":" + nextDevice->GetAddress());
					nextDevice->InsertMessage(msg);
					std::cout << "\n" << router->GetAddress() << " transferred message " << msg.ID << " to " << nextDevice->GetAddress() << "\n";

					simulation::lock.unlock();
					simulation::lock_flag = false;
					std::this_thread::sleep_for(1s);
					simulation::lock.lock();
					simulation::lock_flag = true;
				}
			}
		}

		if(simulation::lock_flag)
		{
			simulation::lock.unlock();
			simulation::lock_flag = false;
		}
	}

	void SendMsgWritePath(const String& filepath, const Message& msg)
	{
		std::ofstream fout(filepath.data(), std::ios::app);
		fout << msg.ID << ":" << msg.trace << "\n";
	}

	bool SendMsgFinished()
	{
		for(int deviceIndex = 0; deviceIndex < DeviceCount(); deviceIndex++)
		{
			if(Device* device = GetDevice(deviceIndex))
				if(!device->GetOutQueue().empty())
					return false;

			if(Machine* machine = GetMachine(deviceIndex))
				if(!machine->GetInQueue().empty())
					return false;

			if(Router* router = GetRouter(deviceIndex))
				if(!router->GetInQueue().empty())
					return false;
		}

		return true;
	}

	// change rt implementation

	bool ChangeRT_Impl(const String& routerAddress, const String& action, const Router::List& fieldList)
	{
		Router* router = GetRouter(routerAddress);

		if(router == nullptr)
			return false;

		Router::List savedList = router->GetRoutingList();
		Router::Tree savedTree = router->GetRoutingTree();

		for(auto field = fieldList.first(); field.valid(); ++field)
		{
			if(action == "add")
				router->InsertField(*field, m_RoutingTableType);

			else if(action == "remove")
			{
				if(!router->RemoveField(*field, m_RoutingTableType))
				{
					router->SetRoutingList(savedList);
					router->SetRoutingTree(savedTree);
					return false;
				}
			}
		}

		return true;
	}

	// print path implementation

	void PrintPathImpl(const String& srcAddress, const String& dstAddress, const String& filepath) const
	{
		std::ifstream fin(filepath.data());
		
		if(!fin)
		{
			std::cout << "\nFailed to open " << filepath << "\n";
			return;
		}

		String line;
		bool oneFound = false;

		std::cout << "\nMessages sent from " << srcAddress << " to " << dstAddress << "\n";

		while(true)
		{
			char ch = fin.get();

			if(!fin)
				break;

			if(ch == '\n')
			{
				auto tokens = line.split(':');
				bool validSize = (tokens.size() >= 3);
				bool validSrc = (srcAddress == "*" || tokens[1] == srcAddress);
				bool validDst = (dstAddress == "*" || tokens.back() == dstAddress);

				if(validSize && validSrc && validDst)
				{
					for(int index = 0; index < tokens.size(); index++)
					{
						if(index == 0)
							std::cout << "\n";
						else
							std::cout << ":";

						std::cout << tokens[index];
						oneFound = true;
					}
				}

				line.clear();
			}

			else
				line.InsertBack(ch);
		}

		if(oneFound)
			std::cout << "\n";
	}

	// change edge implementation

	bool ChangeEdgeImpl(const String& srcAddress, const String& dstAddress, double edgeWeight)
	{
		auto indexA = m_Map.search(srcAddress);
		auto indexB = m_Map.search(dstAddress);

		if(!(indexA && indexB))
			return false;

		auto edgeAB = m_Graph.GetEdge(*indexA, *indexB);
		auto edgeBA = m_Graph.GetEdge(*indexB, *indexA);

		if(!(edgeAB.valid() && edgeBA.valid()))
			return false;

		edgeAB->weight = edgeWeight;
		edgeBA->weight = edgeWeight;
		return true;
	}

	bool ChangeEdgeImpl(const List<Graph::Edge>& edgeList)
	{
		for(auto edgeIter = edgeList.first(); edgeIter.valid(); ++edgeIter)
		{
			if(!(InRange(edgeIter->indexA, 0, DeviceCount()) && InRange(edgeIter->indexB, 0, DeviceCount())))
				return false;

			auto edgeAB = m_Graph.GetEdge(edgeIter->indexA, edgeIter->indexB);

			if(!edgeAB.valid())
				return false;

			edgeAB->weight = edgeIter->weight;
		}

		return true;
	}
};
