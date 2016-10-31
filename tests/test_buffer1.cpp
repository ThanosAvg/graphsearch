#include "../src/buffer.h"
#include <iostream>

bool testInit();

int main(){
    if(testInit()){
        std::cout << "Initialization test passed" << std::endl;
    }
    else{
        std::cout << "Initialization test failed" << std::endl;
    }
}

bool testInit(){
    Buffer buffer;
    ptr myPtr = buffer.allocNewNode();
    myPtr = buffer.allocNewNode();
    if(myPtr == 0){
        return true;
    }
    else{
        return false;
    }
}
