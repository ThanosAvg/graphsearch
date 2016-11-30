#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>

#include "types.h"

typedef uint32_t hashkey_t;

// Bucket class contains the actual data the hash
// is supposed to store and the next bucket node.
// (Linked list approach)
template <class T>
class Bucket{
public:
    Bucket(Bucket *next, T data, hashkey_t key);
    void setNext(Bucket *next);
    void setData(T data);
    void setKey(hashkey_t key);
    Bucket* getNext();
    T getData();
    hashkey_t getKey();
private:
    Bucket* next_;
    T data_;
    hashkey_t key_;
};

// Static hash implementation using buckets for collision.
// Buckets use dynamic linked list approach.
template <class T>
class Hash{
public:
    Hash(uint32_t bucket_number);
    ~Hash();
    T get(hashkey_t key, ResultCode& rescode);
    uint32_t getSize();
    void add(T data, hashkey_t key);
    ResultCode update(T data, hashkey_t key);
    void iterate(void (*action)(T));
private:
    uint32_t hash_(hashkey_t key);
    Bucket<T>** buckets_;
    uint32_t bucket_number_;
    uint32_t size_;
};

#include "hashtable.cpp"
#endif
