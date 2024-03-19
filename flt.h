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

void* flt_malloc(struct FLT *flt, int object_size) {
    // if all pages are full or pages doesn't exist
    if (flt->free_page_blocks == NULL) {
        flt->free_page_blocks = create_BMD(object_size);
    }

    // go through the list to find a list with free blocks
    struct BMD *bmd = flt->free_page_blocks;
    while (bmd != NULL) {
        if (bmd->num_free != 0) {
            return block_malloc(bmd);
        }
        bmd = bmd->next_block;
    }
    // TO DO call a method which put the bmd in full list if it doesn't have free blocks anymore.
    return NULL;
}


#endif //UNTITLED_FLT_H
