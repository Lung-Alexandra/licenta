#ifndef UNTITLED_FLT_H
#define UNTITLED_FLT_H

#include <stdio.h>
#include "bmd.c"

struct FLT {
    void *free_page_blocks;
    void *full_page_blocks;
};

void initialize_FLT(struct FLT *flt);

void* flt_malloc(struct FLT *flt, int object_size);
int flt_free(struct FLT *flt, void*ptr   );

#endif //UNTITLED_FLT_H
