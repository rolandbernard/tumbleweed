
#include <stdlib.h>
#include <string.h>

#include "common/queue.h"

void initQueue(Queue* qu) {
    qu->start = 0;
    qu->end = 0;
    qu->count = 0;
    qu->capacity = 0;
    qu->data = NULL;
}

void freeQueue(Queue* qu) {
    free(qu->data);
    initQueue(qu);
}

void pushQueue(Queue* qu, void* value) {
    if(qu->count == qu->capacity) {
        if(qu->capacity == 0) {
            qu->capacity = 4;
            qu->data = (void**)malloc(sizeof(void*) * qu->capacity);
        } else {
            int new_capacity = 2 * qu->capacity;
            qu->data = (void**)realloc(qu->data, sizeof(void*) * new_capacity);
            memmove(qu->data + new_capacity - qu->capacity + qu->start, qu->data + qu->start, (qu->capacity - qu->start) * sizeof(void*));
            qu->start = new_capacity - qu->capacity + qu->start;
            qu->capacity = new_capacity;
        }
    }
    qu->data[qu->end] = value;
    // capacity will always be a power of two, meaning `& (capacity - 1)` is equal to `% capacity`
    qu->end = (qu->end + 1) & (qu->capacity - 1);
    qu->count++;
}

void* popQueue(Queue* qu) {
    if(qu->count > 0) {
        void* ret = qu->data[qu->start];
        qu->start = (qu->start + 1) & (qu->capacity - 1);
        qu->count--;
        return ret;
    } else {
        return NULL;
    }
}
