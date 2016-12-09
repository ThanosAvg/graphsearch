#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>

#include "types.h"

typedef uint32_t hashkey_t;

template <class T>
struct BucketElem{
    int flag;
    T data;
    hashkey_t key;
};

template <class T>
struct BucketStore{
    BucketStore *next;
    BucketElem<T> *bucketArray;
};

template <class T>
struct BucketInfo{
    BucketStore<T> *lastStore;
    uint32_t lastPos;
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
    void reset();
private:
    uint32_t hash_(hashkey_t key);
    BucketStore<T> *store;
    BucketInfo<T> *bInfo;
    uint32_t bucketNumber_;
    uint32_t size_;
    static const uint32_t maxCollisions_ = 100;
    T blank_;
};

#include "hashtable.cpp"
#endif
