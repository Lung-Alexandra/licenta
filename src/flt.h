#ifndef UNTITLED_FLT_H
#define UNTITLED_FLT_H

#include <stdio.h>
#include "bmd.c"

struct FLT {
    int size;
    void *free_page_blocks;
    void *full_page_blocks;
};
int allocator_size;
int total_size;
void initialize_FLT(struct FLT *flt, int size);

void *flt_malloc(struct FLT *flt, int page_size);

int flt_free(struct FLT *flt, void *ptr, int page_size);

#endif //UNTITLED_FLT_H
