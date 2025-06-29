
#include <string.h>

#include "alloc.h"
#include "array.h"

typedef struct _array_t_ {
    unsigned char* buffer; // array of bytes
    size_t capacity;       // capacity of array in items
    size_t length;         // length of the array in items
    size_t size;           // size of each item
} _array_t_;

static int normalize_idx(_array_t_* ptr, int index) {

    //_array_t_* ptr = (_array_t_*)h;
    if(index < 0) {
        index = ptr->length + index;
        return index;
    }
    else if(index >= 0 && ((size_t)(index) < ptr->length))
        return index;
    else
        return -1;
}

array_t create_array(size_t size) {

    _array_t_* ptr = _ALLOC_TYPE(_array_t_);
    ptr->size = size;
    ptr->capacity = 1 << 3;
    ptr->length = 0;
    ptr->buffer = _ALLOC(ptr->capacity * size);

    return ptr;
}

void destroy_array(array_t h) {

    _array_t_* ptr = (_array_t_*)h;

    if(ptr != NULL) {
        _FREE(ptr->buffer);
        _FREE(ptr);
    }
}

void append_array(array_t h, void* data) {

    _array_t_* ptr = (_array_t_*)h;

    if(ptr->length > ptr->capacity) {
        ptr->capacity <<= 1;
        ptr->buffer = _REALLOC(ptr->buffer, ptr->capacity * ptr->size);
    }

    memcpy(&ptr->buffer[ptr->length * ptr->size], data, ptr->size);
    ptr->length++;
}

void* index_array(array_t h, int index) {

    _array_t_* ptr = (_array_t_*)h;

    index = normalize_idx(ptr, index);
    if(index >= 0) // && ((size_t)(index) < ptr->length))
        return (void*)&ptr->buffer[index * ptr->size];
    else
        return NULL;
}

void* iterate_array(array_t h, int* post) {

    _array_t_* ptr = (_array_t_*)h;

    if((size_t)(*post) < ptr->length) {
        void* retv = (void*)&ptr->buffer[*post * ptr->size];
        *post = *post + 1;
        return retv;
    }
    else
        return NULL;
}

// zero the length but don't change the capacity
void clear_array(array_t h) {

    ((_array_t_*)h)->length = 0;
}


void* raw_array(array_t h) {

    return (void*)((_array_t_*)h)->buffer;
}

size_t len_array(array_t h) {

    return ((_array_t_*)h)->length;
}

#if 0
// build string:
// gcc -g -o t -Wall -Wextra -Wpedantic -pedantic array.c alloc.c

#include <stdio.h>

typedef struct {
    char str[64];
    int size;
} tstruct_t;

char* strlist[] = {
    "baca",
    "abacay",
    "abacas",
    "abacate",
    "abacaxi",
    "abaci",
    "abacinate",
    "abacination",
    "abacisci",
    "abaciscus",
    "abacist",
    "aback",
    "abacli",
    "abacot",
    "abacterial",
    "abactinal",
    "abactinally",
    "abaction",
    "abactor",
    "abaculi",
    "abaculus",
    "abacus",
    "abacuses",
    "abada",
    "abaddon",
    "abadejo",
    "abadengo",
    "abadia",
    "abadite",
    "abaff",
    "abaft",
    "abay",
    "abayah",
    NULL
};

tstruct_t* create_ts(const char* str) {

    tstruct_t* ptr = _ALLOC_TYPE(tstruct_t);
    int len = strlen(str);
    int room = sizeof(ptr->str) - 1;
    int size = (len > room) ? room : len;
    memcpy(ptr->str, str, size);
    ptr->size = size;

    return ptr;
}

void dump_arr(array_t h) {

    int mark = 0;
    tstruct_t* ptr;
    _array_t_* lst = (_array_t_*)h;

    printf("------\n");
    printf("%lu %lu %lu %p\n", lst->capacity, lst->length, lst->size, (void*)lst->buffer);
    while(NULL != (ptr = iterate_array(h, &mark)))
        printf("%3d. \"%s\" %d %p\n", mark, ptr->str, ptr->size, (void*)ptr);
}

int main(void) {

    array_t h = create_array(sizeof(tstruct_t));

    for(int i = 0; i < 10; i++)
        append_array(h, create_ts(strlist[i]));

    dump_arr(h);

    tstruct_t* ptr = index_array(h, 3);
    printf("\"%s\" %d %p\n", ptr->str, ptr->size, (void*)ptr);

    ptr = index_array(h, 0);
    printf("\"%s\" %d %p\n", ptr->str, ptr->size, (void*)ptr);

    ptr = index_array(h, 9);
    printf("\"%s\" %d %p\n", ptr->str, ptr->size, (void*)ptr);

    ptr = index_array(h, -1);
    printf("\"%s\" %d %p\n", ptr->str, ptr->size, (void*)ptr);

    ptr = index_array(h, -3);
    printf("\"%s\" %d %p\n", ptr->str, ptr->size, (void*)ptr);

    ptr = index_array(h, 17);
    printf("null? %p\n", (void*)ptr);

    ptr = index_array(h, -11);
    printf("null? %p\n", (void*)ptr);

    dump_arr(h);
    clear_array(h);
    dump_arr(h);

    return 0;
}

#endif
