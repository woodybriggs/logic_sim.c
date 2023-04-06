#ifndef BUCKET_ALLOCATOR_H
#define BUCKET_ALLOCATOR_H

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUCKET_ALLOCATOR_DEFAULT_ITEM_COUNT 1024
// Define the size of each bucket
#ifndef BUCKET_ALLOCATOR_ITEM_COUNT
#define BUCKET_ALLOCATOR_ITEM_COUNT BUCKET_ALLOCATOR_DEFAULT_ITEM_COUNT
#endif

typedef unsigned char ba_byte;
typedef unsigned long long ba_uint64;

// Define the structure of each bucket
typedef struct Bucket_T {
    ba_uint64 dataSize;
    bool isFull;
    ba_byte * data;
    ba_byte * tail;
    ba_uint64 length;
    ba_uint64 capacity;
    struct Bucket_T* next;
} Bucket;

typedef struct BucketAllocator_T {
    ba_uint64 dataSize;
    ba_uint64 length;
    Bucket* tail;
    Bucket* head;
} BucketAllocator;

Bucket* bucket_new(ba_uint64 dataSize);
BucketAllocator* bucket_allocator_new(ba_uint64 dataSize);
BucketAllocator * bucket_allocator_init(BucketAllocator * allocator, ba_uint64 dataSize);
ba_byte* bucket_allocate_from_current(BucketAllocator* allocator);
ba_byte* bucket_allocator_allocate(BucketAllocator* allocator);
inline void bucket_allocator_free(BucketAllocator* allocator);
ba_byte * bucket_allocator_get_item_by_index(BucketAllocator * allocator, ba_uint64 itemIndex);
Bucket * bucket_allocator_get_bucket_by_index(BucketAllocator * allocator, ba_uint64 itemIndex);

#ifdef BUCKET_ALLOCATOR_IMPLEMENTATION

Bucket* bucket_new(ba_uint64 dataSize) {
    Bucket* bucket = (Bucket*) malloc(sizeof(Bucket) + (BUCKET_ALLOCATOR_ITEM_COUNT * dataSize));
    bucket->dataSize = dataSize;
    bucket->data = (ba_byte *) bucket + sizeof(Bucket);
    bucket->tail = bucket->data;
    bucket->capacity = (BUCKET_ALLOCATOR_ITEM_COUNT);
    bucket->length = 0;
    bucket->next = 0;
    bucket->isFull = 0;
    return bucket;
}

Bucket * bucket_allocator_get_bucket_by_index(BucketAllocator * allocator, ba_uint64 itemIndex)
{
    Bucket * currentBucket = allocator->head;

    ba_uint64 bucketNumber = itemIndex / currentBucket->capacity;

    while (bucketNumber > 0) {
        currentBucket = currentBucket->next;
        bucketNumber--;
    }

    return currentBucket;
}

ba_byte * bucket_allocator_get_item_by_index(BucketAllocator * allocator, ba_uint64 itemIndex) {
    assert(itemIndex <= allocator->length);
    Bucket * currentBucket = allocator->head;

    ba_uint64 bucketNumber = itemIndex / BUCKET_ALLOCATOR_ITEM_COUNT;

    while (bucketNumber > 0) {
        currentBucket = currentBucket->next;
        bucketNumber--;
    }

    ba_uint64 index = itemIndex % BUCKET_ALLOCATOR_ITEM_COUNT;

    return currentBucket->data + (currentBucket->dataSize * index);
}

BucketAllocator * bucket_allocator_init(BucketAllocator * allocator, ba_uint64 dataSize) {
    Bucket* bucket = bucket_new(dataSize);
    allocator->head = bucket;
    allocator->tail = bucket;
    allocator->dataSize = dataSize;
    allocator->length = 0;
    return allocator;
}

BucketAllocator* bucket_allocator_new(ba_uint64 dataSize) {
    BucketAllocator* allocator = (BucketAllocator*) malloc(sizeof(BucketAllocator));
    return bucket_allocator_init(allocator, dataSize);
}

ba_byte* bucket_allocate_from_current(BucketAllocator* allocator) {
    if (!allocator->tail->isFull) {
        void* ptr = allocator->tail->tail;
        allocator->tail->tail += allocator->dataSize;
        allocator->tail->length += 1;
        allocator->length += 1;
        allocator->tail->isFull = allocator->tail->length == allocator->tail->capacity;
        return ptr;
    }
    else {
        Bucket* new_bucket = bucket_new(allocator->dataSize);
        allocator->tail->next = new_bucket;
        allocator->tail = new_bucket;
        return bucket_allocate_from_current(allocator);
    }
}

ba_byte* bucket_allocator_allocate(BucketAllocator* allocator) {
    return bucket_allocate_from_current(allocator);
}

inline void bucket_allocator_free(BucketAllocator* allocator) {}

#endif
#endif