#pragma once
#include <iostream>
#include <fstream>

template<typename T>
struct NodeLL
{
	T value;
	NodeLL* previousNode;
	NodeLL* nextNode;
};

template<typename T>
struct List
{
private:
	NodeLL<T>* tail;
	NodeLL<T>* head;
public:

	List(T value)
	{
		NodeLL<T>* startNode = new NodeLL<T>{ value, nullptr, nullptr };
		head = startNode;
		tail = startNode;
	}

	~List()
	{
		if (GetSize() == 1)
		{
			delete head;
			head = nullptr;
			tail = nullptr;
		}
		else
		{
			NodeLL<T>* currentNode = head;
			while (currentNode)
			{
				NodeLL<T>* curNext = currentNode->nextNode;
				delete[] currentNode;
				currentNode = curNext;
			}
			head = nullptr;
			tail = nullptr;
		}

	}

	bool isEmpty()
	{
		return head == nullptr;
	}

	int GetSize()
	{
		NodeLL<T>* currentNode = head;
		int size = 0;
		while (currentNode)
		{
			size++;
			currentNode = currentNode->nextNode;
		}
		return size;
	}

	void AddToEnd(T value)
	{

		NodeLL<T>* newNode = new NodeLL<T>{ value, tail, nullptr };
		tail->nextNode = newNode;
		tail = newNode;
	}

	void AddToStart(T value)
	{
		NodeLL<T>* newNode = new NodeLL<T>{ value, nullptr, head };
		head->previousNode = newNode;
		head = newNode;
	}


	void LPUSH(T value)
	{
		AddToEnd(value);
	}

	void LPUSH(T value, int index)
	{
		if (index == GetSize() - 1) AddToEnd(value);
		else if (index == 0) AddToStart(value);
		else std::cout << "Wrong index" << std::endl;

	}

	void DeleteLast()
	{
		if (isEmpty()) return;
		if (GetSize() == 1)
		{
			delete head;
			head = nullptr;
			tail = nullptr;
		}
		else
		{
			NodeLL<T>* prevNode = tail->previousNode;
			prevNode->nextNode = nullptr;
			delete tail;
			tail = prevNode;
		}
	}

	void DeleteFirst()
	{
		if (isEmpty()) return;
		if (GetSize() == 1)
		{
			delete head;
			head = nullptr;
			tail = nullptr;
		}
		else
		{
			NodeLL<T>* newHead = head->nextNode;
			newHead->previousNode = nullptr;
			delete head;
			head = newHead;
		}
	}

	void DeleteByValue(T value)
	{
		if (isEmpty()) return;
		if (GetSize() == 1)
		{
			delete head;
			head = nullptr;
			tail = nullptr;
		}

		NodeLL<T>* current = head;
		while (current->value != value)
		{
			current = current->nextNode;
		}
		if (current == head)
		{
			DeleteFirst();
		}
		else if (current == tail)
		{
			DeleteLast();
		}
		else
		{
			NodeLL<T>* previos = current->previousNode;
			NodeLL<T>* next = current->nextNode;
			previos->nextNode = next;
			next->previousNode = previos;
			delete current;
		}

	}

	void LDEL()
	{
		DeleteLast();
	}
	void LDEL(int index)
	{
		if (index == GetSize() - 1) DeleteLast();
		else if (index == 0) DeleteFirst();
		else std::cout << "Wrong index!" << std::endl;
	}

	NodeLL<T>* Find(T value)
	{
		NodeLL<T>* currentNode = head;
		while (currentNode)
		{
			if (currentNode->value == value)
			{
				return currentNode;
			}
			currentNode = currentNode->nextNode;
		}
		return nullptr;
	}

	T getElement(int index)
	{
		if (isEmpty())
		{
			std::cerr << "Empty list, no elements there!" << std::endl;
			exit(-1);
		}
		else if (index >= 0 && index < GetSize())
		{
			NodeLL<T>* currentNode = head;
		for (int i = 0; i < index; i++) currentNode = currentNode->nextNode;

		return currentNode->value;

		}
		else
		{
			std::cerr << "Index out of range!" << std:: endl;
			exit(-1);
		}

		
	}

	void Print()
	{
		NodeLL<T>* currentNode = head;
		while (currentNode)
		{
			std::cout << currentNode->value << " ";
			currentNode = currentNode->nextNode;
		}
		std::cout << std::endl;
	}

	void PrintInFile(std::fstream& file)
	{
		NodeLL<T>* currentNode = head;
		while (currentNode)
		{
			file << currentNode->value << " ";
			currentNode = currentNode->nextNode;
		}
		file << std::endl;
	}

};