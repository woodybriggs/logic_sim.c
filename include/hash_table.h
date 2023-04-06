#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#include "bucket.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#define HASH_TABLE_SIZE 1019

typedef struct HashTableItem_T {
  const char * key;
  void * data;
  uint64_t dataSize;

  struct HashTableItem_T * next;
} HashTableItem;

typedef struct HashTable_T {
  HashTableItem * itemsByHash[HASH_TABLE_SIZE];
  BucketAllocator hashTableItemAllocator;
} HashTable;

int hash_table_hash_key (const char * key);
HashTable * hash_table_new ();
HashTableItem * hash_table_get(HashTable * table, const char *key);
HashTableItem * hash_table_set(HashTable * table, const char *key, void * data, uint64_t size);
int hash_table_c_string_length(const char *string);
bool hash_table_c_string_equal(const char *stringA, const char *stringB);

HashTable* hash_table_new () {
  HashTable * result = (HashTable *) malloc(sizeof(HashTable));
  bucket_allocator_init(&result->hashTableItemAllocator, sizeof(HashTableItem));
  return result;
};

int hash_table_c_string_length(const char *string) {
  int length = 0;
  char * at = (char *) string;

  if (*at == '\0') return 0;

  while (*at != '\0') {
    ++length;
    ++at;
  }

  return length;
}

bool hash_table_c_string_equal(const char *stringA, const char *stringB) {
  int stringALen = hash_table_c_string_length(stringA);
  int stringBLen = hash_table_c_string_length(stringB);

  if (stringALen != stringBLen) return false;

  for (int i = 0; i < stringALen; ++i) {
    char a = stringA[i];
    char b = stringB[i];

    if (a != b) return false;
  }

  return true;
}

int hash_table_hash_key(const char * key) {
  int sum = 0;
  char * at = (char *) key;
  while (*at != '\0') {
    sum += (int) *at;
    at++;
  }
  return sum % HASH_TABLE_SIZE;
}

HashTableItem * hash_table_set(HashTable *table, const char * key, void * data, uint64_t size) {
  uint64_t index = hash_table_hash_key(key);
  HashTableItem * item = table->itemsByHash[index];

  if (!item) {
    item = (HashTableItem *) bucket_allocator_allocate(&table->hashTableItemAllocator);
    table->itemsByHash[index] = item;
    item->key = key;
    item->data = malloc(size);
    item->dataSize = size;
    memcpy(item->data, data, size);
    return item;
  }

  while(item) {

    if (hash_table_c_string_equal(key, item->key)) {
      item->key = key;
      item->data = malloc(size);
      item->dataSize = size;
      memcpy(item->data, data, size);
      return item;
    }

    if (!item->next) {
      item->next = (HashTableItem *) bucket_allocator_allocate(&table->hashTableItemAllocator);
      item = item->next;
      item->key = key;
      item->data = malloc(size);
      item->dataSize = size;
      memcpy(item->data, data, size);
      return item;
    }
    
    item = item->next;
  }

  return item;
}

HashTableItem * hash_table_get(HashTable * table, const char *key) {
  uint64_t index = hash_table_hash_key(key);

  HashTableItem * item = table->itemsByHash[index];
  if (!item) return 0;

  uint64_t keyLength = strlen(key);

  while(item) {
    uint64_t itemKeyLength = strlen(item->key);

    if (keyLength != itemKeyLength) {
      item = item->next;
      continue;
    }
    else if (strncmp(key, item->key, keyLength) == 0) {
      break;
    }
    else {
      item = item->next;
      continue;
    }
  }

  return item;
}

#endif