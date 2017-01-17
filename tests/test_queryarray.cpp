#include <iostream>
#include "../src/query_array.h"

bool simpleTest1(){
    QueryArray qarr(10, 10);
    qarr.addQuery(1, 2, 1, 0);
    if(qarr.getCurrentQueryDataSize() != 1){
        return false;
    }
    return true;
}

bool simpleTest2(){
    QueryArray qarr(10, 10);
    qarr.addQuery(1, 2, 3, 4);
    if(qarr.getCurrentQueryDataSize() != 1){
        return false;
    }

    QueryData* qd = qarr.getQuery(0);
    if(qd->queryFrom != 1){
        return false;
    }
    if(qd->queryTo != 2){
        return false;
    }
    if(qd->queryId != 3){
        return false;
    }
    if(qd->version != 4){
        return false;
    }
    return true;
}

bool simpleTest3(){
    QueryArray qarr(10, 10);
    for(uint32_t i = 0; i < 10000; i++){
        qarr.addQuery(i, i+1, i+2, i+3);
    }

    QueryData* qd;
    for(uint32_t i = 0; i < 10000; i++){
        qd = qarr.getQuery(i);
        if(qd->queryFrom != i){
            return false;
        }
        if(qd->queryTo != i+1){
            return false;
        }
        if(qd->queryId != i+2){
            return false;
        }
        if(qd->version != i+3){
            return false;
        }
    }

    for(uint32_t i = 0; i < 10000; i++){
        qd = qarr.getQuery(i);
        if(qd->queryFrom != i){
            return false;
        }
        if(qd->queryTo != i+1){
            return false;
        }
        if(qd->queryId != i+2){
            return false;
        }
        if(qd->version != i+3){
            return false;
        }
    }
    
    return true;
}

bool fullTest(){
    QueryArray qarr(10, 10);
    for(uint32_t i = 0; i < 10000; i++){
        qarr.addQuery(i, i+1, i+2, i+3);
    }

    for(uint32_t j = 0; j < 2000; j++){
        qarr.addBurst(j);
    }

    QueryData* qd;
    for(uint32_t i = 0; i < 10000; i++){
        qd = qarr.getQuery(i);
        if(qd->queryFrom != i){
            return false;
        }
        if(qd->queryTo != i+1){
            return false;
        }
        if(qd->queryId != i+2){
            return false;
        }
        if(qd->version != i+3){
            return false;
        }
    }

    for(uint32_t j = 0; j < 2000; j++){
        if(*(qarr.getBurst(j)) != j){
            return false;
        }
    }
    return true;
}

int main(){
    if(simpleTest1()){
        std::cout << "Test passed" << std::endl;
    }
    else{
        std::cout << "Test failed" << std::endl;
    }

    if(simpleTest2()){
        std::cout << "Test passed" << std::endl;
    }
    else{
        std::cout << "Test failed" << std::endl;
    }

    if(simpleTest3()){
        std::cout << "Test passed" << std::endl;
    }
    else{
        std::cout << "Test failed" << std::endl;
    }

    if(fullTest()){
        std::cout << "Full test passed" << std::endl;
    }
    else{
        std::cout << "Full test failed" << std::endl;
    }


}
