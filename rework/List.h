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
};

template<typename T>
class List
{
private:
    NodeLL<T>* tail;
    NodeLL<T>* head;

public:
    // Конструктор по умолчанию для создания пустого списка
    List() : head(nullptr), tail(nullptr) {}

    // Конструктор для создания списка с одним элементом
    List(T value)
    {
        NodeLL<T>* startNode = new NodeLL<T>{ value, nullptr, nullptr };
        head = startNode;
        tail = startNode;
    }

    // Деструктор для очистки списка
    ~List()
    {
        NodeLL<T>* currentNode = head;
        while (currentNode)
        {
            NodeLL<T>* curNext = currentNode->nextNode;
            delete currentNode;
            currentNode = curNext;
        }
        head = nullptr;
        tail = nullptr;
    }

    // Проверка на пустоту списка
    bool isEmpty() const
    {
        return head == nullptr;
    }

    // Получение размера списка
    int GetSize() const
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

    // Добавление элемента в конец
    void AddToEnd(T value)
    {
        NodeLL<T>* newNode = new NodeLL<T>{ value, tail, nullptr };
        if (isEmpty()) {
            head = newNode;
            tail = newNode;
        } else {
            tail->nextNode = newNode;
            tail = newNode;
        }
    }

    // Добавление элемента в начало
    void AddToStart(T value)
    {
        NodeLL<T>* newNode = new NodeLL<T>{ value, nullptr, head };
        if (isEmpty()) {
            head = newNode;
            tail = newNode;
        } else {
            head->previousNode = newNode;
            head = newNode;
        }
    }

    // Добавление в конец списка (LPUSH)
    void LPUSH(T value)
    {
        AddToEnd(value);
    }

    // Добавление по индексу
    void LPUSH(T value, int index)
    {
        if (index < 0 || index > GetSize()) {
            std::cerr << "Index out of range!" << std::endl;
            return;
        }

        if (index == GetSize()) {
            AddToEnd(value);
        } else if (index == 0) {
            AddToStart(value);
        } else {
            NodeLL<T>* current = head;
            for (int i = 0; i < index - 1; ++i) {
                current = current->nextNode;
            }
            NodeLL<T>* newNode = new NodeLL<T>{ value, current, current->nextNode };
            current->nextNode->previousNode = newNode;
            current->nextNode = newNode;
        }
    }

    // Удаление последнего элемента
    void DeleteLast()
    {
        if (isEmpty()) return;

        if (head == tail) { // Один элемент в списке
            delete head;
            head = nullptr;
            tail = nullptr;
        } else {
            NodeLL<T>* prevNode = tail->previousNode;
            prevNode->nextNode = nullptr;
            delete tail;
            tail = prevNode;
        }
    }

    // Удаление первого элемента
    void DeleteFirst()
    {
        if (isEmpty()) return;

        if (head == tail) { // Один элемент в списке
            delete head;
            head = nullptr;
            tail = nullptr;
        } else {
            NodeLL<T>* newHead = head->nextNode;
            newHead->previousNode = nullptr;
            delete head;
            head = newHead;
        }
    }

    // Удаление по индексу
    void LDEL(int index)
    {
        if (index < 0 || index >= GetSize()) {
            std::cerr << "Index out of range!" << std::endl;
            return;
        }

        if (index == 0) {
            DeleteFirst();
        } else if (index == GetSize() - 1) {
            DeleteLast();
        } else {
            NodeLL<T>* current = head;
            for (int i = 0; i < index; ++i) {
                current = current->nextNode;
            }
            current->previousNode->nextNode = current->nextNode;
            current->nextNode->previousNode = current->previousNode;
            delete current;
        }
    }

    // Поиск элемента по значению
    NodeLL<T>* Find(T value) const
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

    // Получение элемента по индексу
    T getElement(int index) const
    {
        if (isEmpty()) {
            throw std::out_of_range("Empty list, no elements there!");
        }

        if (index < 0 || index >= GetSize()) {
            throw std::out_of_range("Index out of range!");
        }

        NodeLL<T>* currentNode = head;
        for (int i = 0; i < index; i++) {
            currentNode = currentNode->nextNode;
        }

        return currentNode->value;
    }

    // Печать списка в консоль
    void Print() const
    {
        NodeLL<T>* currentNode = head;
        while (currentNode)
        {
            std::cout << currentNode->value << " ";
            currentNode = currentNode->nextNode;
        }
        std::cout << std::endl;
    }

    // Печать списка в файл
    void PrintInFile(std::fstream& file) const
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
