#ifndef UNTITLED_ALLOCATOR_H
#define UNTITLED_ALLOCATOR_H

#include "flt.h"
#include "flt_large.h"
#include "extrem_large.h"

#define small_max_size (1<<7) // 128
#define small_min_size (1<<3) // 8
#define gap (1<<3) // 8
#define NUM_SMALL_CLASSES (((small_max_size-small_min_size)/gap)+1)


#define medium_min_size ((1<<7) + 8) // 136
#define medium_max_size 496
#define NUM_MEDIUM_CLASSES (((medium_max_size-medium_min_size)/gap)+1)


extern struct FLT small_obj[NUM_SMALL_CLASSES];
extern struct FLT medium_obj[NUM_MEDIUM_CLASSES];
extern struct FLT_LARGE large_obj[NUM_LARGE_CLASSES];
extern void* extreme_large_obj;
extern int has_initialized;


void init();


void *alloc(int size);

void ffree(void *ptr);

//void *malloc(size_t size);
//
//void *realloc(void *ptr, size_t size);
//
//void *calloc(size_t nitems, size_t size);
//
//void free(void *ptr);


#endif //UNTITLED_ALLOCATOR_H
