/*
 * template_queue.h
 *
 *  Created on: Dec 13, 2016
 *      Author: alex
 */

#ifndef TEMPLATE_QUEUE_H_
#define TEMPLATE_QUEUE_H_

#include <iostream>

template<class T>
class Queue {
	struct QueueNode {
		T data;
		QueueNode *next;
		QueueNode(const T& newData, QueueNode *nextNode) :
				data(newData), next(nextNode) {
		}
		QueueNode(QueueNode*nextNode) :
				next(nextNode) {
		}
	};

	QueueNode *front, *rear;
	int size;
public:
	Queue() {
		front = NULL;
		rear = NULL;
		size = 0;
		//std::cout << "Queue was constructed" << std::endl;
	}
	~Queue() {
		//std::cout << "Queue was destructed" << std::endl;
		while (!isEmpty()) {
			Dequeue();
		}
		isEmpty();
	}

	void Enqueue(const T& object);
	T Dequeue();
	const T& GetfrontData();
	int GetSize();
	bool isEmpty();
	void Print();

};

template<class T>
void Queue<T>::Enqueue(const T& obj) {
	if (size == 0) {
		front = new QueueNode(obj, rear);
		rear = front;
	} else {

		//std::cout << "queue insert" << std::endl;

		QueueNode* tmp = new QueueNode(obj, NULL);
		rear->next = tmp;
		rear = tmp;
	}
	size++;

	//std::cout << "exiting" << std::endl;

}

template<class T>
T Queue<T>::Dequeue() {
	if (this->isEmpty() == 0) {
		QueueNode *frontNode = front;
		T tdata = frontNode->data;
		front = front->next;
		delete frontNode;
		frontNode = NULL;
		size--;
		//std::cout << "Dequeued succefully! #" << size << std::endl;
		return tdata;
	} else {
		return static_cast<T>(NULL);
	}
}

template<class T>
const T& Queue<T>::GetfrontData() {
	if (this->isEmpty() == 0) {
		return front->data;
	}
	return static_cast<T>(NULL);
}

template<class T>
bool Queue<T>::isEmpty() {
	return (size == 0);
}

template<class T>
int Queue<T>::GetSize() {
	return size;
}

#endif /* TEMPLATE_QUEUE_H_ */
