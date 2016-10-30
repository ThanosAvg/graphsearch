#include "../src/node_index.h"
#include <iostream>

bool testInit();
bool testInsert();

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
    std::cout << index->getMaxSize() << std::endl;
    for(int i = 0; i < 500; i++){
        if(index->insertNode(i) == false){
            return false;
        }
    }
    std::cout << index->getCurrentSize() << std::endl;
    std::cout << index->getMaxSize() << std::endl;

    return true;
}
