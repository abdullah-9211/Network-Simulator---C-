
#pragma once
#include "util.h"
#include "Array.h"
#include "List.h"

template<typename Type>
class Graph
{
public:

	// types

	struct Edge
	{
		int indexA = -1;
		int indexB = -1;
		double weight = 0.0;
	};

	struct Vertex
	{
		Type data = Type();
		List<Edge> edges;
	};

private:

	// members

	Array<Vertex> m_Vertices;

public:

	// empty state

	bool empty() const
	{ return m_Vertices.empty(); }

	void clear()
	{ m_Vertices.clear(); }

	// access

	int VertexCount() const
	{ return m_Vertices.size(); }

	Type& GetData(int index)
	{
		ErrorAbort(!InRange(index, 0, VertexCount() - 1), "Graph::GetData() : index out of bounds");
		return m_Vertices[index].data;
	}

	const Type& GetData(int index) const
	{
		ErrorAbort(!InRange(index, 0, VertexCount() - 1), "Graph::GetData() : index out of bounds");
		return m_Vertices[index].data;
	}

	Vertex& GetVertex(int index)
	{
		ErrorAbort(!InRange(index, 0, VertexCount() - 1), "Graph::GetVertex() : index out of bounds");
		return m_Vertices[index];
	}

	const Vertex& GetVertex(int index) const
	{
		ErrorAbort(!InRange(index, 0, VertexCount() - 1), "Graph::GetVertex() : index out of bounds");
		return m_Vertices[index];
	}

	typename List<Edge>::Iterator GetEdge(int indexA, int indexB)
	{
		ErrorAbort(!(InRange(indexA, 0, VertexCount() - 1) && InRange(indexB, 0, VertexCount() - 1)), "Graph::GetEdge() : index out of bounds");

		auto& vertex = GetVertex(indexA);

		for(auto edge = vertex.edges.first(); edge.valid(); ++edge)
			if(edge->indexB == indexB)
				return edge;

		return {};
	}

	typename List<Edge>::ConstIterator GetEdge(int indexA, int indexB) const
	{
		ErrorAbort(!(InRange(indexA, 0, VertexCount() - 1) && InRange(indexB, 0, VertexCount() - 1)), "Graph::GetEdge() : index out of bounds");

		const auto& vertex = GetVertex(indexA);

		for(auto edge = vertex.edges.first(); edge.valid(); ++edge)
			if(edge->indexB == indexB)
				return edge;

		return {};
	}

	// insert

	void InsertVertex(const Type& data)
	{ m_Vertices.InsertBack({data}); }

	typename List<Edge>::Iterator InsertEdge(int indexA, int indexB, double weight = 0.0)
	{
		ErrorAbort(!(InRange(indexA, 0, VertexCount() - 1) && InRange(indexB, 0, VertexCount() - 1)), "Graph::InsertEdge() : index out of bounds");
		
		if(GetEdge(indexA, indexB).valid())
			return {};

		auto& vertex = GetVertex(indexA);
		vertex.edges.InsertBack({indexA, indexB, weight});
		return vertex.edges.last();
	}

	// remove

	void RemoveVertex(int indexA)
	{
		ErrorAbort(!InRange(indexA, 0, VertexCount() - 1), "Graph::RemoveVertex() : index out of bounds");

		for(int indexB = 0; indexB < VertexCount() - 1; indexB++)
			RemoveEdge(indexB, indexA);

		m_Vertices.remove(indexA);
	}

	bool RemoveEdge(int indexA, int indexB)
	{
		if(auto edge = GetEdge(indexA, indexB))
		{
			auto& vertex = GetVertex(indexA);
			vertex.edges.remove(edge);
			return true;
		}

		return false;
	}
};
