#define BUCKET_ALLOCATOR_IMPLEMENTATION
#include "bucket.h"
#include "uint64_list.h"
#include <stdio.h>

int main(void) {
  BucketAllocator * allocator = bucket_allocator_new(sizeof(UInt64ListItem));
  UInt64List list = list_uint64_new(allocator);

  list_uint64_append(&list, 0);
  list_uint64_append(&list, 1);
  list_uint64_append(&list, 2);
  list_uint64_append(&list, 3);
  list_uint64_append(&list, 4);
  list_uint64_append(&list, 5);
  list_uint64_append(&list, 6);
  list_uint64_append(&list, 7);


  list_uint64_iterator_init(&list);
  for (int i = 0; i < list.length; ++i) {
    UInt64ListItem * item = list_uint64_iterator_next(&list);
    printf("%llu\n", item->data);
  }



}