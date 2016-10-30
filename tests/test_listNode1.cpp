#include "../src/list_node.h"
#include <iostream>

bool testInit();
bool testAddNeighbor();

int main(){
    if(testInit()){
	std::cout << "Initializing test passed" << std::endl;
    }
    else{
	std::cout << "Initializing test failed" << std::endl;
    }

    if(testAddNeighbor()){
	std::cout << "Adding neighbor test passed" << std::endl;
    }
    else{
	std::cout << "Adding neighbor test failed" << std::endl;
    }
}

bool testInit(){
    ListNode ln;
    std::cout << "List node initialized." << std::endl;
    if(ln.getNeighborCount() == 0){
	return true;
    }
    else{
	return false;
    }
}

bool testAddNeighbor(){
    ListNode ln;
    ln.addNeighbor(5);
    ln.addNeighbor(22);
    if(ln.getNeighborCount() == 2){
	return true;
    }
    else{
	return false;
    }
}
