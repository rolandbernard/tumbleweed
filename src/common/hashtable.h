#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <pthread.h>

#define DELETED (char*)1

typedef struct {
    char* key;
    void* value;
} HashTableEntry;

typedef struct {
    int count;
    int capacity;
    HashTableEntry* data;
} HashTable;

void initHashTable(HashTable* ht);

void freeHashTable(HashTable* ht);

void setToHashTable(HashTable* ht, const char* key, void* value);

void* getFromHashTable(HashTable* ht, const char* key);

void deleteFromHashTable(HashTable* ht, const char* key);

#endif