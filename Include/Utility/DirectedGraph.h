// =====================================================================================================================
// Copyright 2024 Medusa Slockbower
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =====================================================================================================================

#ifndef DIRECTEDGRAPH_H
#define DIRECTEDGRAPH_H

#include <deque>
#include <vector>

template<typename T>
class DirectedGraph
{
public:
	// Typedefs ========================================================================================================

	using DataType = T;
	using Node = uint32_t;
	using NodeQueue = std::deque<Node>;

private:
	// Data Structures =================================================================================================

	struct Director
	{
		enum Flags
		{
			VALID = 0b00000000000000000000000000000001
		};

		Node Parent, Child, Sibling;
		uint32_t Flags;

		Director() : Parent(0), Child(0), Sibling(0), Flags(VALID) { }
	};

	using Hierarchy = std::vector<Director>;
	using Storage = std::vector<DataType>;

public:
	// Functions =======================================================================================================

	DirectedGraph() : Graph{ Director() }, Data{ DataType() }, Freed{ } { }

	[[nodiscard]] Node Parent(Node node) const { return Graph[node].Parent; }
	[[nodiscard]] Node FirstChild(Node node) const { return Graph[node].Child; }
	[[nodiscard]] Node NextSibling(Node node) const { return Graph[node].Sibling; }

	[[nodiscard]] Node LeftMost(Node node) const
	{
		Node current = node;
		while(node = FirstChild(current)) current = node;
		return current;
	}

	[[nodiscard]] uint32_t Depth(Node node) const
	{
		uint32_t depth = 0;
		while (node)
		{
			node = Parent(node);
			++depth;
		}
		return depth;
	}

	Node Insert(const DataType& data, Node parent)
	{
		if(Freed.empty())
		{
			Freed.push_back(static_cast<Node>(Graph.size()));
			Graph.push_back(Director()); Data.push_back(DataType());
		}

		Node next = Freed.front(); Freed.pop_front();
		Director& pnode = Graph[parent];
		Director& node = Graph[next];

		// Setup Node
		node.Parent = parent;
		node.Sibling = pnode.Child;
		node.Child = 0;
		node.Flags = Director::VALID;

		// Set parent's child
		pnode.Child = next;

		Data[next] = data;

		return next;
	}

	void Erase(Node node)
	{
		if(node == 0) return;

		Director& erased = Graph[node];
		erased.Flags &= ~Director::VALID;
		Freed.push_back(node);

		Graph[erased.Parent].Child = erased.Sibling;

		NodeQueue stack{ erased.Child };

		while(stack.empty() == false)
		{
			Node next = stack.front(); stack.pop_front();
			Director& child = Graph[next];
			child.Flags &= ~Director::VALID;
			Freed.push_back(next);

			if(child.Sibling) stack.push_front(child.Sibling);
			if(child.Child) stack.push_front(child.Child);
		}
	}

	DataType& operator[](Node node) { return Data[node]; }
	[[nodiscard]] const DataType& operator[](Node node) const { return Data[node]; }

	template<typename V, typename O>
	void Traverse(V& visitor)
	{
		Traverser<V, O> traverser(*this, visitor);
		traverser();
	}

private:
	// Variables =======================================================================================================

	Hierarchy Graph;
	Storage   Data;
	NodeQueue Freed;

public:
	// Navigation ======================================================================================================

	friend class BreadthFirst;
	class BreadthFirst
	{
	public:
		BreadthFirst(DirectedGraph& graph) : Graph(graph), VisitQueue(0) { }

		Node operator()(Node node)
		{
			node = VisitQueue.back(); VisitQueue.pop_back();
			Director& current = Graph.Graph[node];

			if(current.Sibling) VisitQueue.push_back(current.Sibling);
			if(current.Child) VisitQueue.push_front(current.Child);

			if(VisitQueue.empty()) return 0;
			return node;
		}

	private:
		DirectedGraph& Graph;
		NodeQueue      VisitQueue;
	};

	friend class PreOrder;
	class PreOrder
	{
	public:
		PreOrder(DirectedGraph& graph) : Graph(graph) { }

		Node operator()(Node node)
		{
			Director& current = Graph.Graph[node];

			if(current.Sibling) VisitQueue.push_front(current.Sibling);
			if(current.Child) VisitQueue.push_front(current.Child);

			if(VisitQueue.empty()) return 0;
			Node next = VisitQueue.front(); VisitQueue.pop_front();
			return next;
		}

	private:
		DirectedGraph& Graph;
		NodeQueue      VisitQueue;
	};

	friend class InOrder;
	class InOrder
	{
	public:
		InOrder(DirectedGraph& graph) : Graph(graph) { }

		Node operator()(Node node)
		{
			if(node == 0) VisitQueue.push_back(Graph.LeftMost(node));

			node = VisitQueue.front(); VisitQueue.pop_front();
			Director& current = Graph.Graph[node];

			if(current.Sibling)
			{
				if(Graph.NextSibling(current.Sibling)) VisitQueue.push_back(current.Parent);
				VisitQueue.push_back(Graph.LeftMost(current.Sibling));
			}

			return node;
		}

	private:
		DirectedGraph& Graph;
		NodeQueue      VisitQueue;
	};

	friend class PostOrder;
	class PostOrder
	{
	public:
		PostOrder(DirectedGraph& graph) : Graph(graph) { }

		Node operator()(Node node)
		{
			if(VisitQueue.empty()) VisitQueue.push_back(Graph.LeftMost(node));

			node = VisitQueue.front(); VisitQueue.pop_front();
			if(node == 0) return node;
			Director& current = Graph.Graph[node];

			VisitQueue.push_back(current.Sibling ? Graph.LeftMost(current.Sibling) : Graph.Parent(node));

			return node;
		}

	private:
		DirectedGraph& Graph;
		NodeQueue      VisitQueue;
	};

	template<typename V, typename O>
	class Traverser
	{
	public:
		using VisitorType = V;
		using OrderType = O;

		Traverser(DirectedGraph& graph, VisitorType& visitor) : Graph(graph), Visitor(visitor), Order(graph) { }

		void operator()()
		{
			Node node = 0;
			while(node = Order(node))
			{
				if(Visitor(Graph[node], node)) break;
			}
		}

	private:
		DirectedGraph& Graph;
		VisitorType&   Visitor;
		OrderType      Order;
	};
};

#endif //DIRECTEDGRAPH_H
