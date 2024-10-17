#pragma once
#include <iostream>
#include <fstream>
#include <stdexcept>

template<typename T>
struct NodeLL
{
    T value;
    NodeLL* previousNode;
    NodeLL* nextNode;

    NodeLL(T val) : value(val), previousNode(nullptr), nextNode(nullptr) {}
};

template<typename T>
struct List
{
private:
    NodeLL<T>* head;
    NodeLL<T>* tail;

public:
    // Конструктор для создания пустого списка
    List() : head(nullptr), tail(nullptr) {}

    // Конструктор с одним элементом
    List(T value)
    {
        NodeLL<T>* startNode = new NodeLL<T>(value);
        head = startNode;
        tail = startNode;
    }

    // Деструктор
    // ~List()
    // {
    //     while (head != nullptr)
    //     {
    //         NodeLL<T>* currentNode = head;
    //         head = head->nextNode;
    //         delete currentNode;
    //     }
    //     tail = nullptr;
    // }

    // Проверка, пуст ли список
    bool isEmpty()
    {
        return head == nullptr;
    }

    // Получение размера списка
    int GetSize()
    {
        NodeLL<T>* currentNode = head;
        int size = 0;
        while (currentNode != nullptr)
        {
            size++;
            currentNode = currentNode->nextNode;
        }
        return size;
    }

    // Добавление элемента в конец
    void AddToEnd(T value)
    {
        NodeLL<T>* newNode = new NodeLL<T>(value);
        if (isEmpty())
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->nextNode = newNode;
            newNode->previousNode = tail;
            tail = newNode;
        }
    }

    // Добавление элемента в начало
    void AddToStart(T value)
    {
        NodeLL<T>* newNode = new NodeLL<T>(value);
        if (isEmpty())
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            newNode->nextNode = head;
            head->previousNode = newNode;
            head = newNode;
        }
    }

    // Удаление последнего элемента
    void DeleteLast()
    {
        if (isEmpty()) return;

        if (head == tail)  // Если в списке только один элемент
        {
            delete head;
            head = nullptr;
            tail = nullptr;
        }
        else
        {
            NodeLL<T>* prevNode = tail->previousNode;
            delete tail;
            prevNode->nextNode = nullptr;
            tail = prevNode;
        }
    }

    // Удаление первого элемента
    void DeleteFirst()
    {
        if (isEmpty()) return;

        if (head == tail)  // Если в списке только один элемент
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

    // Удаление элемента по значению
    void DeleteByValue(T value)
    {
        if (isEmpty()) return;

        NodeLL<T>* currentNode = head;

        while (currentNode != nullptr && currentNode->value != value)
        {
            currentNode = currentNode->nextNode;
        }

        if (currentNode == nullptr) return;  // Элемент не найден

        if (currentNode == head)
        {
            DeleteFirst();
        }
        else if (currentNode == tail)
        {
            DeleteLast();
        }
        else
        {
            NodeLL<T>* prev = currentNode->previousNode;
            NodeLL<T>* next = currentNode->nextNode;
            prev->nextNode = next;
            next->previousNode = prev;
            delete currentNode;
        }
    }

    // Добавление элемента (LPUSH) в конец
    void LPUSH(T value)
    {
        AddToEnd(value);
    }

	void LDEL(int index)
	{
    if (isEmpty()) {
        std::cerr << "List is empty!" << std::endl;
        return;
    }

    if (index < 0 || index >= GetSize()) {
        std::cerr << "Index out of range!" << std::endl;
        return;
    }

    if (index == 0) {
        DeleteFirst();
    } else if (index == GetSize() - 1) {
        DeleteLast();
    } else {
        NodeLL<T>* currentNode = head;
        for (int i = 0; i < index; ++i) {
            currentNode = currentNode->nextNode;
        }
        // Удаляем текущий узел
        NodeLL<T>* prevNode = currentNode->previousNode;
        NodeLL<T>* nextNode = currentNode->nextNode;

        prevNode->nextNode = nextNode;
        if (nextNode != nullptr) {
            nextNode->previousNode = prevNode;
        }

        delete currentNode;
    }
	}


    // Получение элемента по индексу
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
            for (int i = 0; i < index; i++)
                currentNode = currentNode->nextNode;

            return currentNode->value;
        }
        else
        {
            std::cerr << "Index out of range!" << std::endl;
            exit(-1);
        }
    }

    // Вывод элементов списка
    void Print()
    {
        NodeLL<T>* currentNode = head;
        while (currentNode != nullptr)
        {
            std::cout << currentNode->value << " ";
            currentNode = currentNode->nextNode;
        }
        std::cout << std::endl;
    }
};
