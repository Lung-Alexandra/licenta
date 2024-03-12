#ifndef UNTITLED_FLT_H
#define UNTITLED_FLT_H

#include <stdio.h>
#include "bmd.h"

struct FLT {
    void *free_page_blocks;
    void *full_page_blocks;
};

void initialize_FLT(struct FLT *flt) {
    flt->free_page_blocks = NULL;
    flt->full_page_blocks = NULL;
}

void add_to_FLT(struct FLT *flt, int object_size) {
    if (flt->free_page_blocks == NULL)
        flt->free_page_blocks = create_BMD(object_size);
}


#endif //UNTITLED_FLT_H
