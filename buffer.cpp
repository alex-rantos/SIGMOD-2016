/*
 * Buffer.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: alex
 */

#include <iostream>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "buffer.h"
#include "components.h"
#include "template_queue.h"

using namespace std;

/**************		Node class	 ***************/

// Constructor & destructor are never used.
Node::Node() :
		endPos(0), nextNode(0) {
	maxCapacity = N;
}

Node::~Node() {
	//cout << "A Node has been deleted" << endl;
}

int Node::AddNeighbor(int i) {
	// mpainei stin arxi o elegxos giati an mpei meta to endPos++
	// mporei na min exoume kapion extra geitona , opote spatali mnimis
	if (endPos >= maxCapacity) { // reached max cap
		//cout << "endPos :" << endPos << " max :" << maxCapacity << endl;
		return -1;
	}
	neighbor[endPos] = i;
	endPos++;
	return 0;
}
int Node::AddNeighbor(int i, int version) {
	if (endPos >= maxCapacity) { // reached max cap
		return -1;
	}
	neighbor[endPos] = i;
	edgeProperty[endPos] = version;
	endPos++;
	return 0;
}

int Node::GetNextNode() {
	return nextNode;
}

int Node::GetNeighbor(int pos) {
	return neighbor[pos];
}

void Node::SetNextNode(int i) {
	nextNode = i;
}

int Node::IsFull() {
	if (endPos >= maxCapacity)
		return 1;
	else
		return 0;
}

int Node::SearchNeighbors(int dest) {
	for (int i = 0; i < endPos; i++) {
		if (neighbor[i] == dest)
			return 0;
	}
	return 1;
}

int Node::SearchDiffComponent(int target, SCC* strongc, Index* index) {
	IndexNode* indArray = index->GetIndexNode();
	for (int i = 0; i < endPos; i++) {
		if (indArray[neighbor[i]].componentID != target) {
			if (strongc->GetPushChecker()[indArray[neighbor[i]].componentID]
					!= target) {
				strongc->GetStrongEdges()[target]->Push(
						indArray[neighbor[i]].componentID);
				strongc->GetPushChecker()[indArray[neighbor[i]].componentID] =
						target;
			}
		}
		//strongc->GetStrongEdges()[target]->PushAfterCheck(indArray[neighbor[i]].componentID,indArray[neighbor[i]].componentID); //
	}
	if ((this->IsFull()) && (nextNode != 0))
		return nextNode;
	else
		return 0;

}

void Node::PrintNeightbors(int src, char c) {
	ofstream bufOutput("graphOUTCOMING.txt", ios::app);
	ofstream bufincoming("graphINCOMING_REVERSED.txt", ios::app);
	if (c == 'o') {
		for (int i = 0; i < endPos; i++) {
			if (neighbor[i] >= 0) {
				bufOutput << setw(8) << std::left << src << neighbor[i] << endl;
			}
		}
	} else if (c == 'i') {
		for (int i = 0; i < endPos; i++) {
			if (neighbor[i] >= 0) {
				bufincoming << setw(8) << std::left << neighbor[i] << src
						<< endl;
			}
		}
	}
}

int Node::DynamicShortestPath(Index* index,char direction ,char firstdir, int level,int repeat,Queue<int>* myqueue,int threadNum,int version) {
	int i = -1;
	IndexNode* indArray = index->GetIndexNode();
	// an exoume dwsei pliroforia component tote comp>=0 diladi gia strongly-connected-alliws dinoume -1
	if (direction == 's') {
		for (i = 0; i < endPos; i++) {
			if (edgeProperty[i] > version)
				return 0;
			if (indArray[neighbor[i]].dest_visited[threadNum] == repeat){
					if (firstdir=='s')
						return (2*level -1);
					else
						return 2*level;
			}
			else
				if (indArray[neighbor[i]].src_visited[threadNum] != repeat){
					myqueue->Enqueue(neighbor[i]);
					indArray[neighbor[i]].src_visited[threadNum] = repeat;
					//indArray[neighbor[i]].src_level[threadNum] = level;
				}
		}
	} else if (direction == 'd') {
		for (i = 0; i < endPos; i++) {
			if (indArray[neighbor[i]].src_visited[threadNum] == repeat){
				if (firstdir=='d')
					return (2*level -1);
				else
					return 2*level;
			}
			else
				if (indArray[neighbor[i]].dest_visited[threadNum] != repeat){
					myqueue->Enqueue(neighbor[i]);
					indArray[neighbor[i]].dest_visited[threadNum] = repeat;
					//indArray[neighbor[i]].dest_level[threadNum] = level;
				}
		}
	}
	if (i == -1) {
		cout
				<< "ERROR @ Node::ShortestPath.Variable i was used uninitialized!Exiting."
				<< endl;
		exit(1);
	}
	if ((i == maxCapacity) && (nextNode != 0))
		return -nextNode;
	else
		return 0;
}


int Node::StaticShortestPath(Index* index,char direction ,char firstdir, int level,int comp,int repeat,Queue<int>* myqueue,int threadNum) {
	int i = -1;
	IndexNode* indArray = index->GetIndexNode();
	// an exoume dwsei pliroforia component tote comp>=0 diladi gia strongly-connected-alliws dinoume -1
	if (direction == 's') {
		for (i = 0; i < endPos; i++) {
			if (indArray[neighbor[i]].componentID!=comp)//an den paizoume sto idio component
			 	continue;
			if (indArray[neighbor[i]].dest_visited[threadNum] == repeat){
					if (firstdir=='s')
						return (2*level -1);
					else
						return 2*level;
			}
			else
				if (indArray[neighbor[i]].src_visited[threadNum] != repeat){
					myqueue->Enqueue(neighbor[i]);
					indArray[neighbor[i]].src_visited[threadNum] = repeat;
					//indArray[neighbor[i]].src_level[threadNum] = level;
				}
		}
	} else if (direction == 'd') {
		for (i = 0; i < endPos; i++) {
			if (indArray[neighbor[i]].componentID != comp) //an den paizoume sto idio component
				continue;
			if (indArray[neighbor[i]].src_visited[threadNum] == repeat){
				if (firstdir=='d')
					return (2*level -1);
				else
					return 2*level;
			}
			else
				if (indArray[neighbor[i]].dest_visited[threadNum] != repeat){
					myqueue->Enqueue(neighbor[i]);
					indArray[neighbor[i]].dest_visited[threadNum] = repeat;
					//indArray[neighbor[i]].dest_level[threadNum] = level;
				}
		}
	}
	if (i == -1) {
		cout
				<< "ERROR @ Node::ShortestPath.Variable i was used uninitialized!Exiting."
				<< endl;
		exit(1);
	}
	if ((i == maxCapacity) && (nextNode != 0))
		return -nextNode;
	else
		return 0;
}
int Node::StaticShortestPath(Index* index,char direction ,char firstdir, int level,int comp,int src_or_dest,int repeat,Queue<int>* myqueue,int threadNum,SCC* strongc) {
	int i = -1;
	int n;
	IndexNode* indArray = index->GetIndexNode();
	// an exoume dwsei pliroforia component tote comp>=0 diladi gia strongly-connected-alliws dinoume -1
	if (direction == 's') {
		for (i = 0; i < endPos; i++) {
			//cout << "check1" << endl;
			if (indArray[neighbor[i]].componentID!=comp){//an den paizoume sto idio component
			 	n=strongc->IsReachableGrail(index,neighbor[i],src_or_dest);
				if (n==0){
					//cout << neighbor[i] << "  " << src_or_dest <<endl;
					continue;
				}
			}
			if (indArray[neighbor[i]].dest_visited[threadNum] == repeat){
					if (firstdir=='s')
						return (2*level -1);
					else
						return 2*level;
			}
			else
				if (indArray[neighbor[i]].src_visited[threadNum] != repeat){
					myqueue->Enqueue(neighbor[i]);
					indArray[neighbor[i]].src_visited[threadNum] = repeat;
					//indArray[neighbor[i]].src_level[threadNum] = level;
				}
		}
	} else if (direction == 'd') {
		for (i = 0; i < endPos; i++) {
			//cout << "check2" <<endl;
			if (indArray[neighbor[i]].componentID != comp){ //an den paizoume sto idio component
				n=strongc->IsReachableGrail(index,src_or_dest,neighbor[i]);
				if (n==0){
					//cout << src_or_dest << "  " << neighbor[i] <<endl;
					continue;
				}
			}
			if (indArray[neighbor[i]].src_visited[threadNum] == repeat){
				if (firstdir=='d')
					return (2*level -1);
				else
					return 2*level;
			}
			else
				if (indArray[neighbor[i]].dest_visited[threadNum] != repeat){
					//cout << "check3" <<endl;
					myqueue->Enqueue(neighbor[i]);
					indArray[neighbor[i]].dest_visited[threadNum] = repeat;
					//indArray[neighbor[i]].dest_level[threadNum] = level;
				}
		}
	}
	if (i == -1) {
		cout
				<< "ERROR @ Node::ShortestPath.Variable i was used uninitialized!Exiting."
				<< endl;
		exit(1);
	}
	if ((i == maxCapacity) && (nextNode != 0))
		return -nextNode;
	else
		return 0;
}
/**************		Index class 	**************/

Index::Index(int maxSize) :
		indSize(maxSize) {
	indexArray = new IndexNode[maxSize];
	cout << "An Index-data_type has been created." << endl;
}

Index::~Index() {
	this->DestroyVisited();
	delete[] indexArray;
	cout << "An Index-data_type has been deleted." << endl;
}

IndexNode* Index::GetIndexNode() {
	return this->indexArray;
}

int Index::GetNeighbor(int target, Buffer* buffer, int pos) {
	int cap, endPos, nextNode;
	Node *out = buffer->GetListNode('o');
	endPos = out[indexArray[target].out].GetEndPos();
	cap = out[indexArray[target].out].GetCapacity();
	if (cap == 0) {
		cout << "ZERO with target : " << target << " @ : "
				<< indexArray[target].out << endl;
		return -1;
	}
	nextNode = out[indexArray[target].out].GetNextNode();
	// Checking where the requested neighbor is located.
	if (pos >= endPos) {
		if (nextNode == 0) {
			cout << "Wrong neighbor pos :: out of reach!" << endl;
			return -1;
		}
		//iterate through Nodes that contain the rest neighbors
		int count = 0, targetNode;
		targetNode = pos / cap; // = node that the neighbor is located
		nextNode = indexArray[target].out;
		do {
			count++;
			nextNode = buffer->GetListNode('o')[nextNode].GetNextNode();
		} while (count != targetNode);
		int newpos = pos % cap;
		return out[nextNode].GetNeighbor(newpos);
	} else
		// it's located at the first node.
		return out[indexArray[target].out].GetNeighbor(pos);
}

void Index::Insert(int src, int dest, Buffer *buf) {
	/*
	 * If it doesn't already exists in Index we insert it in the IndexArray[value] cell
	 * then we "link" it with Buffer class by setting the offset values to
	 * be equal with the corresponding cell of buffer arrays.
	 */
	if (indexArray[src].out == -1) {
		indexArray[src].out = buf->GetOutEnd();
		indexArray[src].outlast = buf->GetOutEnd();
		buf->IncreaseEndPos('o');
	}
	if (indexArray[dest].in == -1) {
		indexArray[dest].in = buf->GetIncEnd();
		indexArray[dest].inlast = buf->GetIncEnd();
		buf->IncreaseEndPos('i');
	}
}

void Index::CheckCap(int src, int dest) {
	if (src >= dest) {
		if (src > this->indSize) {
			cout << "SRC REALLOC" << endl;
			Reallocate(src);
		}
	} else {
		if (dest > this->indSize) {
			cout << "DEST REALLOC" << endl;
			Reallocate(dest);
		}
	}
}

void Index::Reallocate(int newCapacity) {
	IndexNode *tmp = new IndexNode[indSize + newCapacity + 1];
	memcpy(tmp, indexArray, indSize * sizeof(IndexNode));
	delete[] indexArray;
	indexArray = tmp;
	indSize = indSize + newCapacity + 1;
}

int Index::NeighboursNum(int target, char c, Buffer *buf) {
	Node* tmpnode = buf->GetListNode(c);
	int pos = 0;
	if (c == 'o')
		pos = indexArray[target].out;
	else if (c == 'i')
		pos = indexArray[target].in;
	int end;
	int sum = 0;
	if (pos == -1) // no neighbors
		return 0;
	do {
		end = tmpnode[pos].GetEndPos();
		if (end < tmpnode[pos].GetCapacity())
			return sum + end;
		else {
			sum += end;
			pos = tmpnode[pos].GetNextNode();
		}
	} while (pos != 0); // add up the neighbors until there is not NextNode
	return sum;
}

void Index::InitializeVisited(int threads) {
	for (int i = 0; i < indSize; i++) {
		indexArray[i].src_visited = new int[threads];
		for (int j = 0; j < threads; j++)
			indexArray[i].src_visited[j] = -1;
		indexArray[i].dest_visited = new int[threads];
		for (int j = 0; j < threads; j++)
			indexArray[i].dest_visited[j] = -1;
		/*indexArray[i].src_level = new int[threads];
		for (int j = 0; j < threads; j++)
			indexArray[i].src_level[j] = -1;
		indexArray[i].dest_level = new int[threads];
		for (int j = 0; j < threads; j++)
			indexArray[i].dest_level[j] = -1;*/
	}
}

void Index::DestroyVisited() {
	for (int i = 0; i < indSize; i++) {
		delete[] indexArray[i].src_visited;
		delete[] indexArray[i].dest_visited;
		//delete[] indexArray[i].src_level;
		//delete[] indexArray[i].dest_level;
	}
}

void Index::Print() {
	for (int i = 0; i < indSize; i++) {
		cout << i << " in : " << indexArray[i].in << " &  out "
				<< indexArray[i].out << endl;
	}
}

/**************		Buffer class	 **************/

Buffer::Buffer(int maxVal) :
		incSize(maxVal), incEnd(0), outSize(maxVal), outEnd(0) {
	incoming = new Node[maxVal];
	outcoming = new Node[maxVal];
	cout << "A buffer-data_type has been created." << endl;
}

Buffer::~Buffer() {
	cout << "A buffer-data_type has been deleted" << endl;
	delete[] outcoming;
	delete[] incoming;

}

Node* Buffer::GetListNode(char c) {
	if (c == 'i') {
		return incoming;
	} else if (c == 'o') {
		return outcoming;
	}
	cout << "Buffer returning NULL" << endl;
	return NULL;
}

int Buffer::GetIncEnd() {
	return incEnd;
}

int Buffer::GetOutEnd() {
	return outEnd;
}

void Buffer::InsertBuffer(int src, int dest, Index *index) {
	IndexNode *indexA = index->GetIndexNode();
	if (outEnd >= outSize) { // must realloc
		this->Reallocate('o');
	}
	if (incEnd >= incSize) {
		this->Reallocate('i');
	}
	if (outcoming[indexA[src].outlast].AddNeighbor(dest) == -1) {
		outcoming[indexA[src].outlast].SetNextNode(outEnd);
		indexA[src].outlast = outEnd;
		if (outcoming[indexA[src].outlast].AddNeighbor(dest) == -1)
			cout << "Wrong insert @ outcoming" << endl;
		else
			indexA[src].outNeighbors += 1;
		outEnd++;
	} else
		indexA[src].outNeighbors += 1;

	if (incoming[indexA[dest].inlast].AddNeighbor(src) == -1) {
		incoming[indexA[dest].inlast].SetNextNode(incEnd);
		indexA[dest].inlast = incEnd;
		if (incoming[indexA[dest].inlast].AddNeighbor(src) == -1)
			cout << "Wrong insert @ incoming" << endl;
		else
			indexA[dest].inNeighbors += 1;
		incEnd++;
	} else
		indexA[dest].inNeighbors += 1;
}
void Buffer::InsertBuffer(int src, int dest, Index *index, int version) {
	IndexNode *indexA = index->GetIndexNode();
	if (outEnd >= outSize) {
		this->Reallocate('o');
	}
	if (incEnd >= incSize) {
		this->Reallocate('i');
	}
	if (outcoming[indexA[src].outlast].AddNeighbor(dest, version) == -1) {
		outcoming[indexA[src].outlast].SetNextNode(outEnd);
		indexA[src].outlast = outEnd;
		if (outcoming[indexA[src].outlast].AddNeighbor(dest, version) == -1)
			cout << "Wrong insert @ outcoming" << endl;
		else
			indexA[src].outNeighbors += 1;
		outEnd++;
	} else
		indexA[src].outNeighbors += 1;

	if (incoming[indexA[dest].inlast].AddNeighbor(src, version) == -1) {
		incoming[indexA[dest].inlast].SetNextNode(incEnd);
		indexA[dest].inlast = incEnd;
		if (incoming[indexA[dest].inlast].AddNeighbor(src, version) == -1)
			cout << "Wrong insert @ incoming" << endl;
		else
			indexA[dest].inNeighbors += 1;
		incEnd++;
	} else
		indexA[dest].inNeighbors += 1;
}

void Buffer::IncreaseEndPos(char c) {
	if (c == 'i') {
		incEnd++;
	} else if (c == 'o')
		outEnd++;
	else {
		incEnd++;
		outEnd++;
	}
}

void Buffer::AddNeighbor(int src, int dest, Index *index) {
	IndexNode *indArray = index->GetIndexNode();
	int pos = indArray[src].out;
	do {
		if (outcoming[pos].SearchNeighbors(dest) == 0) {
			//cout << "A :: Source node : " << src << " has already " << dest
			//		<< " as neighbor" << endl;
			return;
		}
		pos = outcoming[pos].GetNextNode();
	} while (pos != 0);
	this->InsertBuffer(src, dest, index);
}
void Buffer::AddNeighbor(int src, int dest, Index *index, int version) {
	IndexNode *indArray = index->GetIndexNode();
	int pos = indArray[src].out;
	do {
		if (outcoming[pos].SearchNeighbors(dest) == 0) {
			//	cout << "A :: Source node : " << src << " has already " << dest
			//		<< " as neighbor" << endl;
			return;
		}
		pos = outcoming[pos].GetNextNode();
	} while (pos != 0);
	this->InsertBuffer(src, dest, index, version);
}

int Buffer::Query(int src, int dest, Index *index, int comp, int repeat,
		int threadNum) {
	IndexNode *indArray = index->GetIndexNode();
	int src_pos;			//= indArray[src].out;
	Node* src_node;			//=&(outcoming[src_pos]);
	int dest_pos;			// = indArray[src].in;
	Node* dest_node;			// = &(incoming[dest_pos]);
	Queue<int>* src_queue = new Queue<int>;
	Queue<int>* dest_queue = new Queue<int>;
	src_queue->Enqueue(src);
	dest_queue->Enqueue(dest);
	indArray[src].src_visited[threadNum] = repeat;
	//indArray[src].src_level[threadNum] = 0;
	indArray[dest].dest_visited[threadNum] = repeat;
	//indArray[dest].dest_level[threadNum] = 0;
	int level = 1;
	int k, i, _size, count;
	int counter_s, counter_d;
	if (indArray[src].out == -1 || indArray[dest].in == -1){
		delete src_queue;
		delete dest_queue;
		return -1;
	}
	if (indArray[src].outNeighbors <= indArray[dest].inNeighbors) {
		while (1) {
			counter_s = 0;
			_size = src_queue->GetSize();
			count = 0;
			while (count < _size) {
				i = src_queue->Dequeue();
				count++;
				//if (indArray[i].src_visited[threadNum] == repeat && indArray[i].src_level[threadNum]==level-1) {
				counter_s++;
				src_pos = indArray[i].out;
				if (src_pos == -1){
					counter_s--;
					continue;
				}
				src_node = &(outcoming[src_pos]);
				k = this->SearchNodeNeighbours(src_node,index, 's','s', level,comp,repeat,src_queue,threadNum);
				if (k > 0){
					delete src_queue;
					delete dest_queue;
					return k;
				}
				else
					continue;
			}
			if (counter_s == 0)
				break;
			counter_d = 0;
			_size = dest_queue->GetSize();
			count = 0;
			while (count < _size) {
				i = dest_queue->Dequeue();
				count++;
				//if (indArray[i].dest_visited[threadNum] == repeat && indArray[i].dest_level[threadNum]==level-1) {
				counter_d++;
				dest_pos = indArray[i].in;
				if (dest_pos == -1){
					counter_d--;
					continue;
				}
				dest_node = &(incoming[dest_pos]);
				k = this->SearchNodeNeighbours(dest_node,index, 'd', 's',level,comp,repeat,dest_queue,threadNum);
				if (k > 0){
					delete src_queue;
					delete dest_queue;
					return k;
				}
				else
					continue;
			}
			if (counter_d == 0)
				break;
			level++;
		}
	} else {
		//cout << "pame sto dest" << endl;
		while (1) {
			counter_d = 0;
			_size = dest_queue->GetSize();
			count = 0;
			while (count < _size) {
				i = dest_queue->Dequeue();
				count++;
				//if (indArray[i].dest_visited[threadNum] == repeat && indArray[i].dest_level[threadNum]==level-1) {
				counter_d++;
				dest_pos = indArray[i].in;
				if (dest_pos == -1){
					counter_d--;
					continue;
				}
				dest_node = &(incoming[dest_pos]);
				k = this->SearchNodeNeighbours(dest_node,index, 'd','d', level,comp,repeat,dest_queue,threadNum);
				if (k > 0){
					delete src_queue;
					delete dest_queue;
					return k;
				}
				else
					continue;
			}
			if (counter_d == 0)
				break;
			counter_s = 0;
			_size = src_queue->GetSize();
			count = 0;
			while (count < _size) {
				i = src_queue->Dequeue();
				count++;
				//if (indArray[i].src_visited[threadNum] == repeat && indArray[i].src_level[threadNum]==level-1) {
				counter_s++;
				src_pos = indArray[i].out;
				if (src_pos == -1){
					counter_s--;
					continue;
				}
				src_node = &(outcoming[src_pos]);
				k = this->SearchNodeNeighbours(src_node,index, 's','d', level,comp,repeat,src_queue,threadNum);
				if (k > 0){
					delete src_queue;
					delete dest_queue;
					return k;
				}
				else
					continue;
			}
			if (counter_s == 0)
				break;
			level++;
		}
	}
	delete src_queue;
	delete dest_queue;
	return -1;
}

int Buffer::SearchNodeNeighbours(Node* node,Index* index, char c,char f, int level,int comp,int repeat,Queue<int>* myqueue,int threadNum) {
	int k;
	if (c == 's') {
		do {
			k = node->StaticShortestPath(index, c,f,level,comp,repeat,myqueue,threadNum);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(outcoming[-k]);
			else
				return 0;
		} while (k < 0); //den exei vrei to path akoma
	} else if (c == 'd') {
		do {
			k = node->StaticShortestPath(index, c,f, level,comp,repeat,myqueue,threadNum);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(incoming[-k]);
			else
				return 0;
		} while (k < 0);
	}
	return 0;
}

int Buffer::SearchNodeNeighbours(Node* node,Index* index, char c,char f, int level,int comp,int src_or_dest,int repeat,Queue<int>* myqueue,int threadNum,SCC* strongc) {
	int k;
	if (c == 's') {
		do {
			k = node->StaticShortestPath(index, c,f,level,comp,src_or_dest,repeat,myqueue,threadNum,strongc);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(outcoming[-k]);
			else
				return 0;
		} while (k < 0); //den exei vrei to path akoma
	} else if (c == 'd') {
		do {
			k = node->StaticShortestPath(index, c,f, level,comp,src_or_dest,repeat,myqueue,threadNum,strongc);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(incoming[-k]);
			else
				return 0;
		} while (k < 0);
	}
	return 0;
}

// Overload Bi-directional functions for dynamic graphs

int Buffer::DynamicQuery(int src, int dest, Index *index, int repeat,
		int threadNum, int version) {
	IndexNode *indArray = index->GetIndexNode();
	int src_pos;			//= indArray[src].out;
	Node* src_node;			//=&(outcoming[src_pos]);
	int dest_pos;			// = indArray[src].in;
	Node* dest_node;			// = &(incoming[dest_pos]);
	Queue<int>* src_queue = new Queue<int>;
	Queue<int>* dest_queue = new Queue<int>;
	src_queue->Enqueue(src);
	dest_queue->Enqueue(dest);
	indArray[src].src_visited[threadNum] = repeat;
	indArray[dest].dest_visited[threadNum] = repeat;
	int level = 1;
	int k, i, _size, count;
	int counter_s, counter_d;
	if (indArray[src].out == -1 || indArray[dest].in == -1) {
		delete src_queue;
		delete dest_queue;
		return -1;
	}
	if (indArray[src].outNeighbors <= indArray[dest].inNeighbors) {
		while (1) {
			counter_s = 0;
			_size = src_queue->GetSize();
			count = 0;
			while (count < _size) { // level distinction dequeue current level
				i = src_queue->Dequeue();
				count++;
				counter_s++;
				src_pos = indArray[i].out;
				if (src_pos == -1) {
					counter_s--;
					continue;
				}
				src_node = &(outcoming[src_pos]);
				k = this->SearchNodeNeighboursDynamic(src_node, index, 's', 's',
						level, repeat, src_queue, threadNum, version);
				if (k > 0) {
					delete src_queue;
					delete dest_queue;
					return k;
				} else
					continue;
			}
			if (counter_s == 0) // if no neighbors
				break;
			counter_d = 0;
			_size = dest_queue->GetSize();
			count = 0;
			while (count < _size) {
				i = dest_queue->Dequeue();
				count++;
				counter_d++;
				dest_pos = indArray[i].in;
				if (dest_pos == -1) {
					counter_d--;
					continue;
				}
				dest_node = &(incoming[dest_pos]);
				k = this->SearchNodeNeighboursDynamic(dest_node, index, 'd',
						's', level, repeat, dest_queue, threadNum, version);
				if (k > 0) {
					delete src_queue;
					delete dest_queue;
					return k;
				} else
					continue;
			}
			if (counter_d == 0)
				break;
			level++;
		}
	} else {
		while (1) {
			counter_d = 0;
			_size = dest_queue->GetSize();
			count = 0;
			while (count < _size) {
				i = dest_queue->Dequeue();
				count++;
				//if (indArray[i].dest_visited[threadNum] == repeat && indArray[i].dest_level[threadNum]==level-1) {
				counter_d++;
				dest_pos = indArray[i].in;
				if (dest_pos == -1) {
					counter_d--;
					continue;
				}
				dest_node = &(incoming[dest_pos]);
				k = this->SearchNodeNeighboursDynamic(dest_node, index, 'd',
						'd', level, repeat, dest_queue, threadNum, version);
				if (k > 0) {
					delete src_queue;
					delete dest_queue;
					return k;
				} else
					continue;
			}
			if (counter_d == 0)
				break;
			counter_s = 0;
			_size = src_queue->GetSize();
			count = 0;
			while (count < _size) {
				i = src_queue->Dequeue();
				count++;
				counter_s++;
				src_pos = indArray[i].out;
				if (src_pos == -1) {
					counter_s--;
					continue;
				}
				src_node = &(outcoming[src_pos]);
				k = this->SearchNodeNeighboursDynamic(src_node, index, 's', 'd',
						level, repeat, src_queue, threadNum, version);
				if (k > 0) {
					delete src_queue;
					delete dest_queue;
					return k;
				} else
					continue;
			}
			if (counter_s == 0)
				break;
			level++;
		}
	}
	delete src_queue;
	delete dest_queue;
	return -1;
}

int Buffer::SearchNodeNeighboursDynamic(Node* node, Index* index, char c,
		char f, int level, int repeat, Queue<int>* myqueue, int threadNum,
		int version) {
	int k;
	if (c == 's') {
		do {
			k = node->DynamicShortestPath(index, c, f, level, repeat, myqueue,
					threadNum, version);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(outcoming[-k]);
			else
				return 0;
		} while (k < 0); //den exei vrei to path akoma
	} else if (c == 'd') {
		do {
			k = node->DynamicShortestPath(index, c, f, level, repeat, myqueue,
					threadNum, version);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(incoming[-k]);
			else
				return 0;
		} while (k < 0);
	}
	return 0;
}

void Buffer::Reallocate(char c) {
	if (c == 'i') {
		Node *tmp = new Node[2 * incSize];
		memcpy(tmp, incoming, incSize * sizeof(Node));
		delete[] incoming;
		incoming = tmp;
		incSize = 2 * incSize;
	} else if (c == 'o') {
		Node *tmp = new Node[2 * outSize];
		memcpy(tmp, outcoming, outSize * sizeof(Node));
		delete[] outcoming;
		outcoming = tmp;
		outSize = 2 * outSize;
	}
}

void Buffer::PrintBuffer(Index *index) {
	IndexNode *a = index->GetIndexNode();
	int size = index->GetSize();
	for (int i = 0; i < size; i++) {

		int pos = a[i].out;
		if (pos != -1) {
			do {
				outcoming[pos].PrintNeightbors(i, 'o');
				pos = outcoming[pos].GetNextNode();
			} while (pos != 0);
		}

		pos = a[i].in;
		if (pos != -1) {
			do {
				incoming[pos].PrintNeightbors(i, 'i');
				pos = incoming[pos].GetNextNode();
			} while (pos != 0);
		}
	}
	if (!fork())
		execl("/bin/sh", "sh", "-c", "./insert_unitest.script", (char *) 0);
	wait(NULL);
	cout << "Press a key to proceed" << endl;
	getchar();
	cout << "Continuing..." << endl;
}


CC* Buffer::estimateConnectedComponents(Index *ind) {
    int ccounter = 0;
    CC* compIndex = new CC(ind->GetSize());
    compIndex->num_of_comp = 0;
    IndexNode *indarr = ind->GetIndexNode();
    int indexSize = ind->GetSize();
    cout<<indexSize<<endl;
    for (int i = 0; i < indexSize; i++) {

        if ((indarr[i].visited_out == false)
                && (indarr[i].visited_in == false)) {
           
		   //klisi tou bfs gia  kathe nea component
		   BFS(ind, i, ccounter, compIndex);
        	
			compIndex->num_of_comp++;
          	
			ccounter++; //o arithmos tis cc pou vriskomaste
        }
    }
    return compIndex;
}

/* episkepsi twn komvwn kai twn geitonwn tous kai topothetisi tis stin antistoixi oura
	me vasi ton arithmo tis klisis tou bfs, topotheteitai kai sto ccindex i katallili component*/
int Buffer::BFS(Index*index, int pos, int component, CC*cindex) {
    IndexNode*indarr = index->GetIndexNode();
    int outTemp,inTemp;
    int nodes_count = 0;
    int neighbor_id;
 
    Queue<int> Queue_Out; // oura gia tous ekserxomenous komvous
    Queue_Out.Enqueue(pos);
    Queue<int> Queue_In;   //oura gia tous eiserxomenous komvous
    Queue_In.Enqueue(pos);
    
	cindex->Set_Comp(pos, component);
    int out_position = indarr[pos].out;
    int in_position = indarr[pos].in;
   
    while (Queue_In.isEmpty() == 0 || Queue_Out.isEmpty() == 0) {
        
		if (Queue_Out.isEmpty() == 0) {
            outTemp = Queue_Out.GetfrontData();
            indarr[outTemp].out_queue = false;
            out_position = indarr[outTemp].out;
        	Queue_Out.Dequeue();
            if (out_position != -1) { //o komvos exei geitones
                do {
                    for (int i = 0;i < this->outcoming[out_position].GetEndPos();i++) 
					{
                    	 neighbor_id = outcoming[out_position].GetNeighbor(i);
                       
					    if (indarr[neighbor_id].visited_out == false&& indarr[neighbor_id].out_queue == false) 
						{
 
                            indarr[neighbor_id].out_queue = true;
                            
							Queue_Out.Enqueue(neighbor_id);
                            
							cindex->Set_Comp(neighbor_id, component);
 
                            if (indarr[neighbor_id].visited_in == false && indarr[neighbor_id].in_queue == false) 
							{
                                Queue_In.Enqueue(neighbor_id); //prepei na psaksei kai tous eiserxomenous
                                
								indarr[neighbor_id].in_queue = true;
                            }
                        }
                    }
                    out_position = outcoming[out_position].GetNextNode();
                
				} while (out_position != 0);  //anazitisi gia olous tous geitones
 
            }
            indarr[outTemp].visited_out = 1;
        }
        //antistoixi diadikasia gia tous eiserxomenous komvous
        if (Queue_In.isEmpty() == 0) {
            
			inTemp = Queue_In.GetfrontData();
            
			in_position = indarr[inTemp].in;
            
			indarr[inTemp].in_queue = false;
            
			Queue_In.Dequeue();
            
			if (in_position != -1) {
                do {
                    for (int i = 0; i < this->incoming[in_position].GetEndPos();i++) {
                        neighbor_id = incoming[in_position].GetNeighbor(i);
                        if (indarr[neighbor_id].visited_in == false && indarr[neighbor_id].in_queue == false) 
						{
                            Queue_In.Enqueue(neighbor_id);
                            
							indarr[neighbor_id].in_queue = true; 
                            
							cindex->Set_Comp(neighbor_id, component); 
                            
							if (indarr[neighbor_id].visited_out == false && indarr[neighbor_id].out_queue == false) 
							{
                                Queue_Out.Enqueue(neighbor_id);
                                
								indarr[neighbor_id].out_queue = true;
                            }
                        }
                    }
                    in_position = incoming[in_position].GetNextNode();
                } while (in_position != 0);
 
            }
            indarr[inTemp].visited_in = 1;
        }
        nodes_count++;
        //--------------------------------------------------        cout<<"count: "<<nodes_count<<endl;
    }
    return nodes_count;
}
