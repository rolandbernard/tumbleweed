#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct {
    int start;
    int end;
    int count;
    int capacity;
    void** data;
} Queue;

void initQueue(Queue* qu);

void freeQueue(Queue* qu);

void pushQueue(Queue* qu, void* value);

void* popQueue(Queue* qu);

#endif
