
#include <stdlib.h>

#include "common/arraylist.h"
#include "common/util.h"

void initArrayList(ArrayList* list) {
    list->count = 0;
    list->capacity = 0;
    list->data = NULL;
}

void freeArrayList(ArrayList* list) {
    free(list->data);
    list->count = 0;
    list->capacity = 0;
    list->data = NULL;
}

void pushToArrayList(ArrayList* list, void* item) {
    if (list->count == list->capacity) {
        list->capacity = list->capacity == 0 ? 4 : 2 * list->capacity;
        list->data = (void**)realloc(list->data, list->capacity * sizeof(void*));
    }
    list->data[list->count] = item;
    list->count++;
}

void* popFromArrayList(ArrayList* list) {
    list->count--;
    void* ret = list->data[list->count];
    return ret;
}

void setInArrayList(ArrayList* list, int index, void* item) {
    list->data[index] = item;
}

void* getFromArrayList(ArrayList* list, int index) {
    void* ret = list->data[index];
    return ret;
}
