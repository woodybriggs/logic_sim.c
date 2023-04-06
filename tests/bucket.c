#define BUCKET_ALLOCATOR_IMPLEMENTATION
#define BUCKET_ALLOCATOR_ITEM_COUNT 1024
#include "bucket.h"

int main(void) {
  BucketAllocator * allocator = bucket_allocator_new(sizeof(int));

  int itemCount = (BUCKET_ALLOCATOR_ITEM_COUNT * 2) + 1;

  for (int item = 0; item < itemCount; ++item)
  {
    int * placement = (int *) bucket_allocator_allocate(allocator);
    uint64_t bucketIndex = item % allocator->tail->capacity;
    int * placementCheck = (int* ) (allocator->tail->data + (allocator->dataSize * bucketIndex));

    assert(placement == placementCheck);
    *placement = item;
  }

  assert(allocator->length == itemCount);

  for (int itemIndex = 0; itemIndex < allocator->length; ++itemIndex )
  {
    int * item = (int *) bucket_allocator_get_item_by_index(allocator, itemIndex);
    assert(*item == itemIndex);
  }

  return 0;
}