#include "../src/hashtable.h"
#include <iostream>

int main(){
    Hash<int>* h = new Hash<int>(100000);
    bool test1 = true;
    for(int i = 0; i < 1000000; i++){
        std::cout << "Adding: " << i << std::endl;
        h->add(i, i);
    }

    for(int i = 0; i < 1000000; i++){
        ResultCode rescode;
        int j = h->get(i, rescode);
        if(rescode == NOT_FOUND){
            test1 = false;
            break;
        }
        if(j != i){
            test1 = false;
            break;
        }
        std::cout << "Retrieved: " << j << std::endl;
    }
    for(int i = 1000000; i < 2000000; i++){
        ResultCode rescode;
        h->get(i, rescode);
        if(rescode == FOUND){
            test1 = false;
            break;
        }
    }

    h->reset();
    ResultCode rescode;
    int j = h->get(0, rescode);
    if(rescode == FOUND){
        test1 = false;
    }
    j = h->get(1, rescode);
    if(rescode == FOUND){
        test1 = false;
    }
    std::cout << "Reseted..." << std::endl;

    if(test1){
        std::cout << "Insert test passed" << std::endl;
    }
    else{
        std::cout << "Inset test failed" << std::endl;
    }
}
