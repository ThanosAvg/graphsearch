//#include "hashtable.h"
#include <stdlib.h>
#include <iostream>

template <class T>
Bucket<T>::Bucket(Bucket* next, T data, hashkey_t key){
    this->setNext(next);
    this->setData(data);
    this->setKey(key);
}

template <class T>
void Bucket<T>::setNext(Bucket* next){
    this->next_ = next;
}

template <class T>
void Bucket<T>::setData(T data){
    this->data_ = data;
}

template <class T>
void Bucket<T>::setKey(hashkey_t key){
    this->key_ = key;
}

template <class T>
Bucket<T>* Bucket<T>::getNext(){
    return this->next_;
}

template <class T>
T Bucket<T>::getData(){
    return this->data_;
}

template <class T>
hashkey_t Bucket<T>::getKey(){
    return this->key_;
}

template <class T>
Hash<T>::Hash(uint32_t bucket_number){
    this->bucket_number_ = bucket_number;
    this->size_ = 0;
    buckets_ = new Bucket<T>*[this->bucket_number_];
    for(long i = 0; i < this->bucket_number_; i++){
        buckets_[i] = 0;
    }
    this->initCursor();
}

template <class T>
Hash<T>::~Hash(){
    Bucket<T>* cursor;
    Bucket<T>* prev;
    for(long i = 0; i < this->bucket_number_; i++){
        cursor = this->buckets_[i];
        while(cursor != 0){
            prev = cursor;
            cursor = cursor->getNext();
            delete prev;
        }
    }
    delete[] this->buckets_;
}

template <class T>
T Hash<T>::get(hashkey_t key, ResultCode& rescode){
    long bucket_index;
    bucket_index = this->hash_(key);
    Bucket<T>* current = this->buckets_[bucket_index];
    while(current != 0){
        if(current->getKey() == key){
            rescode = FOUND;
            return current->getData(); // Exit point; SUCCESS
        }
        current = current->getNext();
    }
    rescode = NOT_FOUND;
    T blank = T();
    return blank; // Exit point; NOT FOUND
}

template <class T>
void Hash<T>::add(T data, hashkey_t key){
    uint32_t bucket_index;
    bucket_index = this->hash_(key);
    Bucket<T>* cursor = this->buckets_[bucket_index];
    Bucket<T>* prev = 0;

    // Find next available space
    while(cursor != 0){
        prev = cursor;
        cursor = cursor->getNext();
    }

    // CASE: first bucket in buckets_.
    if(prev == 0){
        this->buckets_[bucket_index] = new Bucket<T>(0, data, key);
    }
    // CASE: append at already created bucket list
    else{
        prev->setNext(new Bucket<T>(0, data, key));
    }
    this->size_++;
}

template <class T>
ResultCode Hash<T>::update(T data, hashkey_t key){
    long bucket_index;
    bucket_index = this->hash_(key);
    Bucket<T>* current = this->buckets_[bucket_index];
    while(current != 0){
        if(current->getKey() == key){
            current->setData(data);
            return FOUND;
        }
        current = current->getNext();
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
    return x % this->bucket_number_;
}

template <class T>
uint32_t Hash<T>::getSize(){
    return this->size_;
}


template <class T>
void Hash<T>::initCursor(){
    this->cursorBucket_ = 0;
    this->cursorIndex_ = 0;
}

template <class T>
T Hash<T>::moveCursorNext(ResultCode& rescode){
    if(this->cursorBucket_ != 0){
        if(this->cursorBucket_->getNext() != 0){
            this->cursorBucket_ = this->cursorBucket_->getNext();
            rescode = FOUND;
            return this->cursorBucket_->getData();
        }
        this->cursorIndex_++;
    }
    while(this->cursorIndex_ < this->bucket_number_ && this->buckets_[this->cursorIndex_] == 0){
        // Find next element
        this->cursorIndex_++;
    }

    if(this->cursorIndex_ >= this->bucket_number_){
        // Out of bounds
        rescode = NOT_FOUND;
        return 0;
    }

    // Move cursor
    this->cursorBucket_ = this->buckets_[this->cursorIndex_];
    rescode = FOUND;
    return this->cursorBucket_->getData();  
}
