#ifndef _LINKED_QUEUE_H
#define _LINKED_QUEUE_H
#include <iostream>
using namespace std;

template<class T>
class Node
{
private:
	T mData;
	Node*mNext;
public:
	Node(T data) :mData(data) { mNext = nullptr; }
	~Node() {}
	T GetData() { return mData; }
	Node*GetNext() { return mNext; }
	void SetData(T data) { mData = data; }
	void SetNext(Node*next) { mNext = next; }
};

template<class T>
class Queue
{
private:
	Node<T>*mHead;
	Node<T>*mTail;
public:
	Queue();
	~Queue();
	bool QIsEmpty();
	void Push(T data);
	T Pop();
	T Front();
};
template<class T>
Queue<T>::Queue()
{
	mHead = nullptr;
	mTail = nullptr;
}
template<class T>
Queue<T>::~Queue()
{
	while (mHead != nullptr)
		Pop();
}
template<class T>
void Queue<T>::Push(T data)//push
{
	Node<T>*temp = new Node<T>(data);
	if (mHead == nullptr)
	{
		mHead = temp;
		mTail = temp;
	}
	else
	{
		mTail->SetNext(temp);
		mTail = temp;
	}
}
template<class T>
T Queue<T>::Pop()//pop
{
	T temp;
	Node<T>*node;
	if (mHead == nullptr)
	{
		cout << "Queue에 데이터가 없습니다." << endl;
		return 0;
	}
	else
	{
		temp = mHead->GetData();
		node = mHead;
		mHead = mHead->GetNext();
		delete node;
		return temp;
	}
}

template<class T>
T Queue<T>::Front()
{
	if (mHead == nullptr)
	{
		cout << "Queue에 데이터가 없습니다." << endl;
		return 0;
	}
	else
	{
		return mHead->GetData();
	}
}


template<class T>
bool Queue<T>::QIsEmpty()
{
	if (mHead == nullptr)
		return true;
	else
		return false;
}
#endif