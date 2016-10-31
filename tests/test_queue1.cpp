#include "../src/queue.h"
#include <iostream>

using namespace std;

int main(void){
	Queue* queue=new Queue();
	for(int i=0;i<500;i++)
		queue->enqueue(i);
	for(int i=0;i<250;i++)
		queue->dequeue();
	if(queue->isEmpty()){
		cout << "Test Error" << endl;
		delete queue;
		return -1;
	}
	for(int i=0;i<100000;i++)
		queue->enqueue(i);
	if(queue->isEmpty()){
		cout << "Test Error" << endl;
		delete queue;
		return -1;
	}
	for(int i=0;i<100250;i++)
		queue->dequeue();
	if(queue->isEmpty()){
		cout << "Test Passed" << endl;
		delete queue;
		return 0;
	}
	else{
		cout << "Test Failed" << endl;
		delete queue;
		return -1;
	}

}
