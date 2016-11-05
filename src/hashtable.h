#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>

typedef uint32_t hashkey_t;

// To be used in return codes
enum ResultCode{FOUND, NOT_FOUND};

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
    T get(hashkey_t key, ResultCode& rescode);
    void add(T data, hashkey_t key);
    void iterate(void (*action)(T));
private:
    uint32_t hash_(hashkey_t key);
    Bucket<T>** buckets_;
    uint32_t bucket_number_;
    uint32_t size_;
};

#include "hashtable.cpp"
#endif
