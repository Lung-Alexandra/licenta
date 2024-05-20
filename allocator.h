#ifndef UNTITLED_ALLOCATOR_H
#define UNTITLED_ALLOCATOR_H

#include "flt.c"
#include "flt_large.c"
//#include "copy_large.c"
#include "extrem_large.c"

#define small_max_size (1<<7) // 128
#define small_min_size (1<<3) // 8
#define gap (1<<3) // 8
#define NUM_SMALL_CLASSES (((small_max_size-small_min_size)/gap)+1)


#define medium_min_size ((1<<7) + 8) // 136
#define medium_max_size 496
#define NUM_MEDIUM_CLASSES (((medium_max_size-medium_min_size)/gap)+1)


struct FLT small_obj[NUM_SMALL_CLASSES];
struct FLT medium_obj[NUM_MEDIUM_CLASSES];
struct FLT_LARGE large_obj[NUM_LARGE_CLASSES];
void* extreme_large_obj;


void init();

void *alloc(int size);

void ffree(void *ptr);


#endif //UNTITLED_ALLOCATOR_H
