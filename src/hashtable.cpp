//#include "hashtable.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>

template <class T>
Hash<T>::Hash(uint32_t bucketNumber){
    this->bucketNumber_ = bucketNumber;
    this->store = new BucketStore<T>();
    this->store->next = 0;
    this->store->bucketArray = (BucketElem<T>*) malloc(this->bucketNumber_ * this->maxCollisions_ * sizeof(BucketElem<T>));
    memset(this->store->bucketArray, 0, this->bucketNumber_ * this->maxCollisions_ * sizeof(BucketElem<T>));
    //this->store->bucketArray = (BucketElem<T>*)calloc(this->bucketNumber_ * this->maxCollisions_, sizeof(BucketElem<T>));
    this->bInfo = (BucketInfo<T>*) malloc(this->bucketNumber_ * sizeof(BucketInfo<T>));
    memset(this->bInfo, 0, this->bucketNumber_ * sizeof(BucketInfo<T>));
    //this->bInfo = (BucketInfo<T>*) calloc(this->bucketNumber_, sizeof(BucketInfo<T>));
    this->size_ = 0;
}

template <class T>
Hash<T>::~Hash(){
    BucketStore<T> *toBeDeleted;
    BucketStore<T> *current = this->store;
    while(current != 0){
        toBeDeleted = current;
        current = current->next;
        free(toBeDeleted->bucketArray);
        delete toBeDeleted;
    }
}

template <class T>
void Hash<T>::reset(){
    BucketStore<T> *toBeCleaned;
    BucketStore<T> *current = this->store;
    while(current != 0){
        toBeCleaned = current;
        current = current->next;
        memset(toBeCleaned->bucketArray, 0, this->bucketNumber_ * this->maxCollisions_ * sizeof(BucketElem<T>));
    }
    memset(this->bInfo, 0, this->bucketNumber_ * sizeof(BucketInfo<T>));
}

template <class T>
T Hash<T>::get(hashkey_t key, ResultCode& rescode){
    long bucketIndex;
    bucketIndex = this->hash_(key);
    uint32_t current = 0;
    BucketStore<T> *currentStore = this->store;
    while(currentStore != 0){
        for(uint32_t i = 0; i < this->maxCollisions_; i++){
            if(currentStore->bucketArray[maxCollisions_ * bucketIndex + i].flag == 0){
                break; // TODO: can get even better!
            }
            if(currentStore->bucketArray[maxCollisions_ * bucketIndex + i].key == key){
                rescode = FOUND;
                return currentStore->bucketArray[maxCollisions_ * bucketIndex + i].data; // Exit point; FOUND
            }
        }
        currentStore = currentStore->next;
    }
    rescode = NOT_FOUND;
    return this->blank_; // Exit point; NOT FOUND
}

template <class T>
void Hash<T>::add(T data, hashkey_t key){
    uint32_t bucketIndex;
    bucketIndex = this->hash_(key);
    uint32_t current;
    BucketStore<T> *currentStore;

    current = this->bInfo[bucketIndex].lastPos;
    currentStore = this->bInfo[bucketIndex].lastStore;
    if(currentStore == 0){
        currentStore = this->store;
        this->bInfo[bucketIndex].lastStore = this->store;
    }

    if(current < this->maxCollisions_){
        // Simply add
        currentStore->bucketArray[maxCollisions_ * bucketIndex + current].flag = 1;
        currentStore->bucketArray[maxCollisions_ * bucketIndex + current].data = data;
        currentStore->bucketArray[maxCollisions_ * bucketIndex + current].key = key;
        this->bInfo[bucketIndex].lastPos++;
    }
    else if(currentStore->next == 0){
        BucketStore<T> *bstore = new BucketStore<T>();
        bstore->next = 0;
        bstore->bucketArray = (BucketElem<T>*)malloc(this->bucketNumber_ * this->maxCollisions_ * sizeof(BucketElem<T>));
        memset(bstore->bucketArray, 0, this->bucketNumber_ * this->maxCollisions_ * sizeof(BucketElem<T>));
        bstore->bucketArray[maxCollisions_ * bucketIndex].flag = 1;
        bstore->bucketArray[maxCollisions_ * bucketIndex].data = data;
        bstore->bucketArray[maxCollisions_ * bucketIndex].key = key;
        currentStore->next = bstore;
        this->bInfo[bucketIndex].lastStore = bstore;
        this->bInfo[bucketIndex].lastPos = 0;
    }
    else{
        currentStore = currentStore->next;
        current = 0;
        currentStore->bucketArray[maxCollisions_ * bucketIndex + current].flag = 1;
        currentStore->bucketArray[maxCollisions_ * bucketIndex + current].data = data;
        currentStore->bucketArray[maxCollisions_ * bucketIndex + current].key = key;
        this->bInfo[bucketIndex].lastStore = currentStore;
        this->bInfo[bucketIndex].lastPos = 0;
    }

    this->size_++;
}

template <class T>
ResultCode Hash<T>::update(T data, hashkey_t key){
    long bucketIndex;
    bucketIndex = this->hash_(key);
    uint32_t current = 0;
    BucketStore<T> *currentStore = this->store;
    while(currentStore != 0){
        for(uint32_t i = 0; i < this->maxCollisions_; i++){
            if(currentStore->bucketArray[maxCollisions_ * bucketIndex + i].flag == 0){
                break;
            }
            if(currentStore->bucketArray[maxCollisions_ * bucketIndex + i].key == key){
                currentStore->bucketArray[maxCollisions_ * bucketIndex + i].data = data; // Exit point; FOUND
                return FOUND;
            }
        }
        currentStore = currentStore->next;
    }
    return NOT_FOUND;
}

template <class T>
uint32_t Hash<T>::hash_(hashkey_t key){
    // Based on http://stackoverflow.com/a/12996028
    uint32_t x = key;
    /*
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    */
    return x % this->bucketNumber_;
}

template <class T>
uint32_t Hash<T>::getSize(){
    return this->size_;
}
