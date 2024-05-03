#ifndef UNTITLED_ALLOCATOR_H
#define UNTITLED_ALLOCATOR_H

#include "flt.c"
#include "flt_large.c"

#define small_max_size (1<<7) // 128
#define small_min_size (1<<3) // 8
#define gap (1<<3) // 8
#define SMALL_CLASS_SIZE ((small_max_size-small_min_size)/gap)


#define medium_min_size ((1<<7) + 8) // 136
#define medium_max_size 496
#define MEDIUM_CLASS_SIZE ((medium_max_size-medium_min_size)/gap)


struct FLT small_obj[SMALL_CLASS_SIZE+1];
struct FLT medium_obj[MEDIUM_CLASS_SIZE+1];


void init();

void *alloc(int size);

void ffree(void *ptr);


#endif //UNTITLED_ALLOCATOR_H
