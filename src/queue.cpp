#include <stdlib.h>

#include "queue.h"

Queue::Queue() {
	/*  Queue Constructor */
	start=NULL;
	end=NULL;
}

void Queue::enqueue(uint32_t node) {
	/* Add a node in the end of the queue */
	QueueNode* temp=new QueueNode();
	temp->node=node;
	temp->next=NULL;
	if (start==NULL) {
		start=temp;
	}
	else {
		end->next=temp;
	}
	end=temp;
}

uint32_t Queue::dequeue() {
	/* Pop the first node */
	uint32_t targetNode;
	QueueNode* temp;
	if(start!=NULL){
		targetNode=start->node;
		temp=start;
		start=start->next;
		delete temp;
	}
	return targetNode;
}

bool Queue::isEmpty() {
	/* Check if empty */
	return start==NULL;
}

Queue::~Queue() {
	/* Queue destructor */
	QueueNode* temp;
	while(!this->isEmpty()){
		temp=start;
		start=start->next;
		delete temp;
	}
}

