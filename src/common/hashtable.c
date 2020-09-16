
#include <stdlib.h>
#include <string.h>

#include "common/hashtable.h"

void initHashTable(HashTable* ht) {
    ht->count = 0;
    ht->capacity = 0;
    ht->data = NULL;
}

void freeHashTable(HashTable* ht) {
    for(int i = 0; i < ht->capacity; i++) {
        if(ht->data[i].key != NULL && ht->data[i].key != DELETED) {
            free(ht->data[i].key);
        }
    }
    free(ht->data);
    ht->data = NULL;
    ht->capacity = 0;
    ht->count = 0;
}

static unsigned long hashString(const char* str) {
    unsigned long hash = 6151;
    while(*str) {
        hash = hash * 769 + *str;
        str++;
    }
    return hash;
}

static void insertIntoData(HashTableEntry* data, int capacity, char* key, void* value) {
    // capacity will always be a power of two, meaning `& (capacity - 1)` is equal to `% capacity`
    int index = hashString(key) & (capacity - 1);
    while(data[index].key != NULL && data[index].key != DELETED) {
        index = (index + 1) & (capacity - 1);
    }
    data[index].key = key;
    data[index].value = value;
}

static int findEntry(const HashTableEntry* data, int capacity, const char* key) {
    if(capacity != 0) {
        int index = hashString(key) & (capacity - 1);
        while (data[index].key != NULL) {
            if (data[index].key != DELETED && strcmp(data[index].key, key) == 0) {
                return index;
            }
            index = (index + 1) & (capacity - 1);
        }
    }
    return -1;
}

static void rehashHashTable(HashTable* ht, HashTableEntry* new_data, int new_capacity) {
    for(int i = 0; i < ht->capacity; i++) {
        if(ht->data[i].key != NULL && ht->data[i].key != DELETED) {
            insertIntoData(new_data, new_capacity, ht->data[i].key, ht->data[i].value);
        }
    }
}

void setToHashTable(HashTable* ht, const char* key, void* value) {
    if (ht->count * 3 >= ht->capacity * 2) {
        if (ht->capacity == 0) {
            ht->capacity = 32;
            ht->data = (HashTableEntry*)calloc(ht->capacity, sizeof(HashTableEntry));
        } else {
            int new_capacity = ht->capacity * 2;
            HashTableEntry* new_data = (HashTableEntry*)calloc(new_capacity, sizeof(HashTableEntry));
            rehashHashTable(ht, new_data, new_capacity);
            free(ht->data);
            ht->capacity = new_capacity;
            ht->data = new_data;
        }
    }
    int index = findEntry(ht->data, ht->capacity, key);
    if (index == -1) {
        int len = strlen(key);
        char* key_copy = (char*)malloc(len + 1);
        memcpy(key_copy, key, len + 1);
        insertIntoData(ht->data, ht->capacity, key_copy, value);
        ht->count++;
    } else {
        ht->data[index].value = value;
    }
}

void* getFromHashTable(HashTable* ht, const char* key) {
    int index = findEntry(ht->data, ht->capacity, key);
    void* ret = NULL;
    if (index != -1) {
        ret = ht->data[index].value;
    }
    return ret;
}

void deleteFromHashTable(HashTable* ht, const char* key) {
    int index = findEntry(ht->data, ht->capacity, key);
    if (index != -1) {
        free(ht->data[index].key);
        ht->data[index].key = DELETED;
        ht->count--;
        if (ht->count * 20 < ht->capacity && ht->count > 1) {
            int new_capacity = ht->capacity / 2;
            HashTableEntry* new_data = (HashTableEntry*)calloc(new_capacity, sizeof(HashTableEntry));
            rehashHashTable(ht, new_data, new_capacity);
            free(ht->data);
            ht->capacity = new_capacity;
            ht->data = new_data;
        }
    }
}
