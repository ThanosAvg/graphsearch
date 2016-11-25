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

bool testCursor(){
    Buffer* buffer = new Buffer();
    NodeIndex* index = new NodeIndex(buffer);
    for(int i = 0; i < 20; i++){
        index->insertNode(i);
    }
    int counter = 0;
    int32_t elem = 0;
    index->cursorInit();
    while((elem = index->cursorGetNext()) != -1){
        //std::cout << elem << std::endl;
        counter++;
    }
    if(counter == 20){
        return true;
    }
    else{
        return false;
    }
}
