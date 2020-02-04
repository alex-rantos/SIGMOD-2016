#ifndef _TEMPLATE__STACK_H
#define _TEMPLATE__STACK_H

#include <iostream>
#include <stdexcept>

template<class T>
struct item_return{ typedef T type; };

template<class T>
class Stack {
	struct StackNode {
		T data;
		StackNode *next;
		StackNode(const T& newData, StackNode *nextNode) :
				data(newData), next(nextNode) {
		}
	};

	StackNode *head;
	int size;
public:
	Stack() :
			head(NULL),size(0) {
		//std::cout << "Stack was constructed" << std::endl;
	}
	~Stack() {
		//std::cout << "Stack was destructed" << std::endl;
		while (!isEmpty()) {
			Pop();
		}
		isEmpty();
	}

	void Push(const T& object);
	T Pop();
	const T GetHeadData();
	bool isEmpty();
	int GetSize();
	void Print();
};

template<class T>
void Stack<T>::Push(const T& obj) {
	head = new StackNode(obj, head);
	size++;
}

template<class T>
T Stack<T>::Pop() {
	if (!isEmpty()) {
		StackNode *headNode = head;
		head = head->next;
		T data = headNode->data;
		delete headNode;
		size--;
		return data;
	}
	else {
		return static_cast<T>(NULL);
	}
}

template<class T>
const T Stack<T>::GetHeadData() {
	if (!isEmpty()) {
		return head->data;
	}
	return static_cast<T>(NULL);
}

template<class T>
bool Stack<T>::isEmpty() {
	return (head == NULL);
}

template<class T>
int Stack<T>::GetSize() {
	return size;
}

#endif
