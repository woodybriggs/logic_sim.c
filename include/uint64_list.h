#include "bucket.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct UInt64ListItem_T {
  uint64_t data;
  struct UInt64ListItem_T * next;
} UInt64ListItem;

typedef struct UInt64List_T {
  BucketAllocator * uInt64ListItemAllocator;
  UInt64ListItem * head;
  UInt64ListItem * tail;
  uint64_t length;
  UInt64ListItem * iterator;
  uint64_t iteratorCount;
} UInt64List;

UInt64List list_uint64_new (BucketAllocator * uInt64ListItemAllocator) {
  UInt64List list = {
    .uInt64ListItemAllocator = uInt64ListItemAllocator,
    .head = 0,
    .tail = 0,
    .iterator = 0,
    .length = 0,
  };

  return list;
}

UInt64ListItem * list_uint64_append(UInt64List * list, uint64_t value) {
  if (list->length == 0) {
    list->head = (UInt64ListItem *) bucket_allocator_allocate(list->uInt64ListItemAllocator);
    list->head->data = value;
    list->tail = list->head;
    list->length++;
  } else {
    list->tail->next = (UInt64ListItem *) bucket_allocator_allocate(list->uInt64ListItemAllocator);
    list->tail->next->data = value;
    list->tail = list->tail->next;
    list->length++;
  }

  return list->tail;
}

void list_uint64_iterator_init(UInt64List *list) {
  list->iterator = list->head;
  list->iteratorCount = 0;
}

UInt64ListItem * list_uint64_iterator_next(UInt64List * list) {
  if (list->iteratorCount == list->length) {
    list_uint64_iterator_init(list);
    return list->iterator;
  }

  UInt64ListItem * result = list->iterator;
  list->iterator = list->iterator->next;
  list->iteratorCount++;

  return result;
}