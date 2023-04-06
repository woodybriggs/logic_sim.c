#define BUCKET_ALLOCATOR_IMPLEMENTATION
#include "bucket.h"
#include "hash_table.h"
#include <stdio.h>

typedef struct Payload_T {
  int value;
} Payload;

int main (void) {
  HashTable * table = hash_table_new();

  const char * itemAKey = "af";
  const char * itemBKey = "be";
  const char * itemCKey = "cd";
  const char * itemDKey = "hello";
  Payload payloadA = { .value = 100 };
  Payload payloadB = { .value = 200 };
  Payload payloadC = { .value = 300 };
  Payload payloadD = { .value = 400 };

  hash_table_set(table, itemAKey, &payloadA, sizeof(payloadA));
  hash_table_set(table, itemBKey, &payloadB, sizeof(payloadB));
  hash_table_set(table, itemCKey, &payloadC, sizeof(payloadC));
  hash_table_set(table, itemDKey, &payloadD, sizeof(payloadD));

  HashTableItem * itemA = hash_table_get(table, itemAKey);
  HashTableItem * itemB = hash_table_get(table, itemBKey);
  HashTableItem * itemC = hash_table_get(table, itemCKey);
  HashTableItem * itemD = hash_table_get(table, itemDKey);

  printf("|\tHash\t|\tKey\t|\tValue\t|\n");
  printf("|\t%d\t|\t%s\t|\t%d\t|\n", hash_table_hash_key(itemAKey), itemA->key, ((Payload *)itemA->data)->value);
  printf("|\t%d\t|\t%s\t|\t%d\t|\n", hash_table_hash_key(itemBKey), itemB->key, ((Payload *)itemB->data)->value);
  printf("|\t%d\t|\t%s\t|\t%d\t|\n", hash_table_hash_key(itemCKey), itemC->key, ((Payload *)itemC->data)->value);
  printf("|\t%d\t|\t%s\t|\t%d\t|\n", hash_table_hash_key(itemDKey), itemD->key, ((Payload *)itemD->data)->value);

}