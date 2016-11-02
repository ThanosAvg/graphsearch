#include "buffer.h"
#include "list_node.h"
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;


Buffer :: Buffer(){
	/* Buffer constructor,initialize to inital size */

	bufferSpace_=(ListNode*)malloc(INITIAL_BUFFER_SIZE*sizeof(ListNode));
	bufferSize_=INITIAL_BUFFER_SIZE;
	numberOfNodes_=0;
}

ptr Buffer :: allocNewNode(){
	/* Returns the index of the new block in the buffer */

	if(numberOfNodes_>=bufferSize_){
		/* Double the size of the buffer if needed */
		//cout << "reallocing" <<endl;
		bufferSpace_=(ListNode*)realloc(bufferSpace_,bufferSize_*sizeof(ListNode)*2);
		bufferSize_*=2;
	}
	ListNode* newNode=new ListNode();
	memcpy(&bufferSpace_[numberOfNodes_], newNode, sizeof(ListNode));
	delete newNode;
	ptr nodePtr=numberOfNodes_;
	numberOfNodes_++;
	return nodePtr;
}

ListNode* Buffer :: getListNode(ptr nodeIndex){
	/* Returns a pointer to the requested ListNode
	or Null if it does not exists */

	if(nodeIndex>=bufferSize_ || nodeIndex >= numberOfNodes_)
		return NULL;
	return (ListNode*)&(bufferSpace_[nodeIndex]);

}

Buffer :: ~Buffer(){
	/* Destructor,deletes bufferSpace */

    free(bufferSpace_);
}
