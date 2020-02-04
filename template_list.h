#ifndef TEMPLATE_LIST_H_
#define TEMPLATE_LIST_H_

#include <iostream>
#include "components.h"

template<class T>
class List {
	struct ListNode {
		T data;
		ListNode *next,*prev;
		ListNode(const T& newData) :
				data(newData), next(NULL), prev(NULL) {
		}
	};
	ListNode *head,*cur,*last;
	int size;
public:
	List() :
			head(NULL),cur(NULL), last(NULL), size(0) {
		//std::cout << "List was constructed" << std::endl;
	}
	~List() {
		//std::cout << "List is destructed" << std::endl;
		while (!isEmpty()) {
			PopLast();
		}
		isEmpty();
	}

	void Push(const T& object);
	void PushAfterCheck(const T& object,int);
	T PopHead();
	T PopLast();
	bool IsOut();
	const T& GetCurData();
	const T& GetHeadData();
	bool isEmpty();
	void ResetCur() { cur = head; };
	int GetSize();
	bool IncCur();
	bool IncreaseCur();
	void Print();
	int FindElement(const T&object);
	const T& FindMinimum();
	ListNode*Get_Head(){return head;}
};

template<class T>
void List<T>::Push(const T& obj) {
	if (size == 0) {
		head = new ListNode(obj);
		last = head;
	}
	else {
		ListNode* tmp = new ListNode(obj);
		tmp->prev = last;
		last->next = tmp;
		last = tmp;
	}
	size++;
}

template<class T>
void List<T>::PushAfterCheck(const T& obj,int number) {
	if (size == 0) {
		head = new ListNode(obj);
		last = head;
	}
	else {
		ListNode* p=head;
		while (p!=NULL){
			if (p->data == number)
				return;
			p=p->next;
		}
		ListNode* tmp = new ListNode(obj);
		tmp->prev = last;
		last->next = tmp;
		last = tmp;
	}
	size++;
}

template<class T>
T List<T>::PopHead() {
	if (!isEmpty()) {
		T data;
		if (size == 1) {
			head->next = NULL;
			head->prev = NULL;
			last->prev = NULL;
			last->next = NULL;
			last = NULL;
			data = head->data;
			delete head;
			head = NULL;
		} else if (size > 1) {
			head = head->next;
			data = head->prev->data;
			delete head->prev;
			head->prev = NULL;
		}
		size--;
		return data;
	} else {
		std::cout << "\t Pophead : EMPTY LIST" << std::endl;
		return 0;
	}
}

template<class T>
T List<T>::PopLast() {
	if (!isEmpty()) {
		T data = static_cast<T>(NULL);
		if (size == 1) {
			head->next = NULL;
			head->prev = NULL;
			last->prev = NULL;
			last->next = NULL;
			last = NULL;
			data = head->data;
			delete head;
			head = NULL;
		} else if (size > 1) {
			last = last->prev;
			data = last->next->data;
			delete last->next;
			last->next = NULL;
		}
		size--;
		return data;
	} else {
		std::cout << "\tPopLast : EMPTY LIST" << std::endl;
		return 0;
	}
}

template<class T>
const T& List<T>::GetHeadData() {
	if (!isEmpty()) {
		return head->data;
	}
}

template<class T>
bool List<T>::IsOut() {
	if (!isEmpty()) {
		return (cur == NULL);
	}
	//std::cout << "Return false LIST::ISOUT" << std::endl;
	return true;
}

template<class T>
bool List<T>::isEmpty() {
	return (head == 0);
}

template<class T>
const T& List<T>::GetCurData() {
	return cur->data;
}

template<class T>
bool List<T>::IncCur() {
	if ( (cur->next) != NULL ) {
		cur = cur->next;
		return true;
	} else
		return false;
}

template<class T>
bool List<T>::IncreaseCur() {
	if ( (cur->next) != NULL ) {
		cur = cur->next;
		return true;
	}
	else {
		cur=cur->next;
		return false;
	}
}

template<class T>
void List<T>::Print() {
	std::cout << "Printing List : " << std::endl;
	ListNode *cur = head;
	for (int i = size; i > 1; i--) {
		std::cout << cur->data << " ";
		cur = cur->next;
	}
	if (size)
		std::cout <<cur->data << std::endl;
}

template<class T>
int List<T>::GetSize() {
	return size;
}

template<class T>
int List<T>::FindElement(const T& object){
	ListNode*temp=head;
	while(temp!=NULL){
		if(temp->data==object) return 1;
		temp=temp->next;
	}
	return -1;
}

template<class T>
const T& List<T>::FindMinimum(){
	ListNode*temp=head;
	ListNode*temp2;
	T min=temp->data;
	while(temp!=NULL){
		if(temp->data<min){
			min=temp->data;
			temp2=temp;	
		}
		temp=temp->next;
	}
	return temp2->data;
}


#endif
