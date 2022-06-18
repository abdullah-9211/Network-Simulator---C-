
#pragma once
#include "util.h"
#include "Pair.h"

template<typename Key, typename Value>
class SplayTree
{
private:

	// types

	using KeyValue = Pair<Key, Value>;

	struct Node
	{
		KeyValue kv;
		Node* left = nullptr;
		Node* right = nullptr;
	};

	// members

	mutable Node* m_Root = nullptr;

public:

	// constructors and memory management

	SplayTree() = default;

	SplayTree(const SplayTree& other)
	{ CopyImpl(m_Root, other.m_Root); }

	SplayTree& operator=(const SplayTree& other)
	{
		if(this != &other)
		{
			clear();
			CopyImpl(m_Root, other.m_Root);
		}

		return *this;
	}

	~SplayTree()
	{ clear(); }

	// empty state

	bool empty() const
	{ return m_Root == nullptr; }

	void clear()
	{ ClearImpl(m_Root); }

	// search

	Value* search(const Key& key)
	{
		if(empty())
			return nullptr;

		m_Root = splay(key, m_Root);

		if(m_Root->kv.first != key)
			return nullptr;

		return &m_Root->kv.second;
	}

	const Value* search(const Key& key) const
	{
		if(empty())
			return nullptr;

		m_Root = splay(key, m_Root);

		if(m_Root->kv.first != key)
			return nullptr;

		return &m_Root->kv.second;
	}

	// insert

	Value* insert(const KeyValue& kv)
	{
		if(empty())
		{
			m_Root = new Node{kv};
			return &m_Root->kv.second;
		}

		m_Root = splay(kv.first, m_Root);

		if(kv.first < m_Root->kv.first)
		{
			Node* insertionNode = new Node{kv, m_Root->left, m_Root};
			m_Root->left = nullptr;
			m_Root = insertionNode;
			return &m_Root->kv.second;
		}

		else if(kv.first > m_Root->kv.first)
		{
			Node* insertionNode = new Node{kv, m_Root, m_Root->right};
			m_Root->right = nullptr;
			m_Root = insertionNode;
			return &m_Root->kv.second;
		}

		else return nullptr;
	}

	// remove

	bool remove(const Key& key)
	{
		if(!search(key))
			return false;

		Node* removalNode = m_Root;

		if(!m_Root->left)
			m_Root = m_Root->right;
		else
		{
			m_Root = splay(key, m_Root->left);
			m_Root->right = removalNode->right;
		}

		delete removalNode;
		return true;
	}

	// print

	void PrintPreOrder() const
	{ PrintPreOrderImpl(m_Root); }

	void PrintInOrder() const
	{ PrintInOrderImpl(m_Root); }

	void PrintPostOrder() const
	{ PrintPostOrderImpl(m_Root); }

private:

	// copy implementation

	void CopyImpl(Node*& thisNode, Node* otherNode)
	{
		if(otherNode)
		{
			thisNode = new Node{otherNode->kv};
			CopyImpl(thisNode->left, otherNode->left);
			CopyImpl(thisNode->right, otherNode->right);
		}
	}

	// clear implementation

	void ClearImpl(Node*& currentNode)
	{
		if(currentNode)
		{
			ClearImpl(currentNode->left);
			ClearImpl(currentNode->right);
			delete currentNode;
			currentNode = nullptr;
		}
	}

	// print implementation

	void PrintPreOrderImpl(Node* currentNode) const
	{
		if(currentNode)
		{
			std::cout << "\n[" << currentNode->kv.first << ", " << currentNode->kv.second << "]";
			PrintPreOrderImpl(currentNode->left);
			PrintPreOrderImpl(currentNode->right);
		}
	}

	void PrintInOrderImpl(Node* currentNode) const
	{
		if(currentNode)
		{
			PrintInOrderImpl(currentNode->left);
			std::cout << "\n[" << currentNode->kv.first << ", " << currentNode->kv.second << "]";
			PrintInOrderImpl(currentNode->right);
		}
	}

	void PrintPostOrderImpl(Node* currentNode) const
	{
		if(currentNode)
		{
			PrintPostOrderImpl(currentNode->left);
			PrintPostOrderImpl(currentNode->right);
			std::cout << "\n[" << currentNode->kv.first << ", " << currentNode->kv.second << "]";
		}
	}

	// rotation implementation

	Node* RotateRight(Node* root) const
	{
		Node* left = root->left;
		root->left = left->right;
		left->right = root;
		return left;
	}

	Node* RotateLeft(Node* root) const
	{
		Node* right = root->right;
		root->right = right->left;
		right->left = root;
		return right;
	}

	// splay implementation

	Node* splay(const Key& searchedKey, Node* centerTree) const
	{
		if(!centerTree)
			return nullptr;

		Node treeHeader;
		Node* leftTreeMax = &treeHeader;
		Node* rightTreeMin = &treeHeader;

		while(true)
		{
			if(searchedKey < centerTree->kv.first)
			{
				if(!centerTree->left)
					break;

				if(searchedKey < centerTree->left->kv.first)
					centerTree = RotateRight(centerTree);

				if(!centerTree->left)
					break;

				rightTreeMin->left = centerTree;
				rightTreeMin = centerTree;
				centerTree = centerTree->left;
			}

			else if(searchedKey > centerTree->kv.first)
			{
				if(!centerTree->right)
					break;

				if(searchedKey > centerTree->right->kv.first)
					centerTree = RotateLeft(centerTree);

				if(!centerTree->right)
					break;

				leftTreeMax->right = centerTree;
				leftTreeMax = centerTree;
				centerTree = centerTree->right;
			}

			else break;
		}

		leftTreeMax->right = centerTree->left;
		rightTreeMin->left = centerTree->right;
		centerTree->left = treeHeader.right;
		centerTree->right = treeHeader.left;
		return centerTree;
	}
};
