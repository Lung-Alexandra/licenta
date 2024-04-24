#include "flt.h"

void initialize_FLT(struct FLT *flt) {
    flt->free_page_blocks = NULL;
    flt->full_page_blocks = NULL;
}

void *move_to_full(struct FLT *flt, struct BMD *bmd) {
    struct BMD *prev = bmd->prev_block;
    if (prev != NULL) {
        prev->next_block = bmd->next_block;
    } else {
        flt->free_page_blocks = bmd->next_block;
    }
    bmd->next_block = NULL;
    if (flt->full_page_blocks == NULL) {
        flt->full_page_blocks = bmd;
    } else {
        struct BMD *head = flt->full_page_blocks;
        flt->full_page_blocks = bmd;
        bmd->next_block = head;
    }
}

void *move_to_free(struct FLT *flt, struct BMD *bmd) {
    struct BMD *prev = bmd->prev_block;
    if (prev != NULL) {
        prev->next_block = bmd->next_block;
    } else {
        flt->full_page_blocks = bmd->next_block;
    }
    bmd->next_block = NULL;
    if (flt->free_page_blocks == NULL) {
        flt->free_page_blocks = bmd;
    } else {
        struct BMD *head = flt->free_page_blocks;
        flt->free_page_blocks = bmd;
        bmd->next_block = head;
    }
}

void *flt_malloc(struct FLT *flt, int object_size) {
    // if all pages are full or pages doesn't exist
    if (flt->free_page_blocks == NULL) {
        flt->free_page_blocks = create_BMD(object_size);
    }

    // go through the list to find a list with free blocks
    struct BMD *bmd = flt->free_page_blocks;
    while (bmd != NULL) {
        // if bmd structure has free slots (obtained after free operation) or has not been completed yet
        if (bmd->num_free != 0 || bmd->num_bumped < bmd->num_total) {
            if (bmd->num_bumped == bmd->num_total && bmd->num_free == 0) {
                move_to_full(flt, bmd);
            }
            return block_malloc(bmd);
        }
        bmd = bmd->next_block;
    }
//    // go through list of free pages and if bmd structure is full after allocation
//    // it is put in full_page_blocks of flt and deleted from free_page_blocks
//    bmd = flt->free_page_blocks;
//    while (bmd != NULL) {
//        if (bmd->num_bumped == bmd->num_total && bmd -> num_free == 0) {
//            flt ->full_page_blocks = bmd;
//        }
//        bmd = bmd->next_block;
//    }
//    //     TO DO call a method which put the bmd in full list if it doesn't have free blocks anymore.
    return NULL;
}

void *flt_free(struct FLT *flt, void *ptr) {
    void *current = flt->free_page_blocks;
    while (current != NULL) {
        struct BMD *bmd = current;
        if (ptr >= current && ptr < current + PAGE_SIZE) {
            block_free(bmd, ptr);
            return ptr;
        }
        current = bmd->next_block;
    }
    current = flt->full_page_blocks;
    while (current != NULL) {
        struct BMD *bmd = current;
        if (ptr >= current && ptr < current + PAGE_SIZE) {
            block_free(bmd, ptr);
            if (bmd->num_bumped == bmd->num_total - 1) {
                move_to_free(flt, bmd);
            }
            return ptr;
        }
        current = bmd->next_block;
    }
    return NULL;
}
