#include "../src/node_index.h"
#include <iostream>

bool testInit();
bool testInsert();
bool testGetHead();
bool testCursor();

int main(){
    if(testInit()){
        std::cout << "Initialization test passed" << std::endl;
    }
    else{
        std::cout << "Initialization test failed" << std::endl;
    }
    if(testInsert()){
        std::cout << "Insertion test passed" << std::endl;
    }
    else{
        std::cout << "Insertion test failed" << std::endl;
    }
    if(testGetHead()){
        std::cout << "Getting list head test passed" << std::endl;
    }
    else{
        std::cout << "Getting list head test failed" << std::endl;
    }
    if(testCursor()){
        std::cout << "Cursor test passed" << std::endl;       
    }
    else{
        std::cout << "Cursor test failed" << std::endl;
    }
}

bool testInit(){
    Buffer buffer;
    NodeIndex index(&buffer);
    if(index.getCurrentSize() != 0){
        return true;
    }
    else{
        return false;
    }
}

bool testInsert(){
    Buffer* buffer = new Buffer();
    NodeIndex* index = new NodeIndex(buffer);
    for(int i = 0; i < 500; i++){
        if(index->insertNode(i) == false){
            return false;
        }
    }
    delete index;
    delete buffer;
    return true;
}

bool testGetHead(){
    Buffer* buffer = new Buffer();
    NodeIndex* index = new NodeIndex(buffer);
    for(int i = 0; i < 500; i++){
        index->insertNode(i);
    }
    bool success = true;
    for(int i = 0; i < 500; i++){
        if(index->getListHead(i) != i){
            success = false;
            break;
        }
    }
    delete index;
    delete buffer;
    return success;
}

bool testCursor(){
    Buffer* buffer = new Buffer();
    NodeIndex* index = new NodeIndex(buffer);
    for(int i = 0; i < 1500000; i+=2){
        index->insertNode(i);
    }
    bool error;
    int next;
    for(int i = 0; i < 1500000; i+=2){
        next = index->getNextId(error);
        if(error){
            std::cout << "An error occured while moving cursor" << std::endl;
            return false;
        }
        else if(next != i){
            std::cout << "Expected: " << i << " Received: " << next << std::endl;
            return false;
        }
    }

    for(int i = 6000; i < 6500; i++){
        next = index->getNextId(error);
        if(!error){
            return false;
        }
    }
    return true;
}
