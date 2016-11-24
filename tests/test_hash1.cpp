#include "../src/hashtable.h"
#include <iostream>

void testInsert(){
    Hash<int>* h = new Hash<int>(100000);
    bool test1;
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
    test1 = true;
    if(test1){
        std::cout << "Insert test passed" << std::endl;
    }
    else{
        std::cout << "Inset test failed" << std::endl;
    }
}

void testCursor(){
    Hash<int> hash(3);
    hash.add(1,10);
    hash.add(2,10);
    hash.add(3,10);
    hash.add(4,10);
    hash.add(5,20);
    hash.add(6,20);
    hash.add(7,20);
    hash.add(8,30);
    hash.add(9,30);
    hash.add(10,40);
    hash.initCursor();
    ResultCode rescode = FOUND;
    //for(int i = 0; i < 10; i ++){
    //    std::cout << hash.moveCursorNext(rescode) << std::endl;
    //}
    int x;
    int counter = 0;
    while(rescode != NOT_FOUND){
        x = hash.moveCursorNext(rescode);
        std::cout << x << std::endl;
        counter++;
    }
    if(counter == 11){ // 10 + 1 not found
        std::cout << "Test passed" << std::endl;
    }
    else{
        std::cout << "Test failed" << std::endl;
    }
}

int main(){
    testCursor();
}
