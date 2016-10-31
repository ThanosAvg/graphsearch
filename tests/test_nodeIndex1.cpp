#include "../src/node_index.h"
#include <iostream>

bool testInit();
bool testInsert();
bool testGetHead();

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
}

bool testInit(){
    Buffer buffer;
    NodeIndex index(&buffer);
    if(index.getCurrentSize() == 0){
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
    bool success;
    if(index->getCurrentSize() == 500){
        success = true;
    }
    else{
        success = false;
    }
    delete index;
    delete buffer;
    return success;
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
