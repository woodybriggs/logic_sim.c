#include <_types/_uint64_t.h>
#include <assert.h>
#include <malloc/_malloc.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct UInt64Array_T {
  uint64_t * data;
  uint64_t length;
  uint64_t capacity;
} UInt64Array;

UInt64Array * array_uint64_new(uint64_t capacity) {
  UInt64Array * result = (UInt64Array *) malloc(sizeof(UInt64Array));
  result->data = (uint64_t *) malloc(sizeof(uint64_t) * capacity);
  result->length = 0;
  result->capacity = capacity;
  return result;
}

void array_uint64_push(UInt64Array * array, uint64_t item) {
  assert(array->length+1 <= array->capacity);
  array->data[array->length] = item;
  array->length++;
}

uint64_t array_uint64_index(UInt64Array * array, uint64_t index) {
  assert(index < array->length);
  return array->data[index];
}

void array_uint64_reverse(UInt64Array * array) {
  uint64_t * reversed = (uint64_t *) malloc(sizeof(uint64_t) * array->length);
  uint64_t index = array->length;
  uint64_t placement = 0;
  while(index > 0) {
    index--;
    reversed[placement] = array_uint64_index(array, index);
    placement++;
  }

  free(array->data);
  array->data = reversed;
}