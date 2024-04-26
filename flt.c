#include "flt.h"

void initialize_FLT(struct FLT *flt) {
    flt->free_page_blocks = NULL;
    flt->full_page_blocks = NULL;
}

void print_free_full_pages(struct FLT *flt) {
    printf("---------------\n");
    printf("Free pages\n");
    struct BMD *bmd = flt->free_page_blocks;
    while (bmd != NULL) {
        printf("%p\n", bmd);
        void *current = (void *) bmd + sizeof(struct BMD);
        for (int i = 0; i < bmd->num_total; ++i) {
            if () {
                printf("[ ]");

            } else {
                printf("[o]");
            }
            current += bmd->object_size;

            if ((i + 1) % 10 == 0) {
                printf("\n");
            }
        }
        bmd = bmd->next_block;
    }
    printf("Full pages\n");
    bmd = flt->full_page_blocks;
    while (bmd != NULL) {
        printf("%p\n", bmd);
        bmd = bmd->next_block;
    }
    printf("---------------\n");
}


/* the free list can look like
           ┌────────┐     next      ┌─────────┐   next    ┌─────────┐   next
           │        ├──────────────►│         ├──────────►│         │ ───────►NULL
           │  bmd1  │               │  bmd2   │           │  bmd3   │
NULL◄──────┤        │◄──────────────┤         │◄──────────┤         │
           └────┬───┘      prev     └─────────┘   prev    └─────────┘
                │
                ▼
           page to move

or like this
           ┌────────┐     next
           │        ├──────────────►NULL
           │  bmd1  │
NULL◄──────┤        │
           └────┬───┘
                │
                ▼
           page to move
*/
void move_to_full(struct FLT *flt, struct BMD *bmd) {
    printf("Moved to full %p\n", bmd);
    /* prev == Null means we want to move the first page from free list
     * prev will always be null because we allocate the fist slot form the recent page
     * (move to free grantees that we add the recent page first)
     * first block can't have a previous block (always points to null)
     * in case block have a pointer to next block
     * we need to make free list to point to next block
     * then we make the current bmd next's pointer and
     * the next block's prev pointer to point to null
   */
    if (bmd->next_block != NULL) {
        struct BMD *next = bmd->next_block;
        flt->free_page_blocks = next;
        next->prev_block = NULL;
        bmd->next_block = NULL;
    } else {
        flt->free_page_blocks = NULL;
    }

    // in case full page is empty just add the block
    if (flt->full_page_blocks == NULL) {
        flt->full_page_blocks = bmd;
    } else {
        // we want to add the block first
        // in order to do that we need to get the head of the full page block
        // make head's prev pointer to point to bmd that we want to insert
        // and make the bmd next pointer to point to the old head
        struct BMD *head = flt->full_page_blocks;
        flt->full_page_blocks = bmd;
        head->prev_block = bmd;
        bmd->next_block = head;
    }
}


/* the full list can look like
 case 1:

           ┌────────┐  next
           │        ├────────►NULL
           │  bmd1  │
NULL◄──────┤        │
           └────┬───┘
                │
                ▼
           page to move

 case 2:
           ┌────────┐     next      ┌─────────┐   next    ┌─────────┐   next
           │        ├──────────────►│         ├──────────►│         │ ───────►NULL
           │  bmd1  │               │  bmd2   │           │  bmd3   │
NULL◄──────┤        │◄──────────────┤         │◄──────────┤         │
           └────┬───┘      prev     └─────────┘   prev    └─────────┘
                │
                ▼
           page to move

case 3:
           ┌────────┐     next      ┌─────────┐   next    ┌─────────┐   next
           │        ├──────────────►│         ├──────────►│         │ ───────►NULL
           │  bmd1  │               │  bmd2   │           │  bmd3   │
NULL◄──────┤        │◄──────────────┤         │◄──────────┤         │
           └────────┘      prev     └────┬────┘   prev    └─────────┘
                                         │
                                         ▼
                                    page to move
case 4:
           ┌────────┐     next      ┌─────────┐   next    ┌─────────┐   next
           │        ├──────────────►│         ├──────────►│         │ ───────►NULL
           │  bmd1  │               │  bmd2   │           │  bmd3   │
NULL◄──────┤        │◄──────────────┤         │◄──────────┤         │
           └────────┘      prev     └─────────┘   prev    └────┬────┘
                                                               │
                                                               ▼
                                                             page to move
 */
void move_to_free(struct FLT *flt, struct BMD *bmd) {
    printf("Moved to free %p\n", bmd);
    struct BMD *prev = bmd->prev_block;
    // case 1 and 2
    if (prev == NULL) {
        // case 2
        if (bmd->next_block != NULL) {
            struct BMD *next = bmd->next_block;
            flt->full_page_blocks = next;
            next->prev_block = NULL;
            bmd->next_block = NULL;
        } else {
            //case 1
            flt->full_page_blocks = NULL;
        }
    } else {
        //case 3
        if (bmd->next_block != NULL) {
            // we are in the middle so block has a prev and a next
            // we need to link the prev next's pointer to next block of the bmd that we want to move
            // and the next block prev pointer to prev block of bmd
            struct BMD *next = bmd->next_block;
            prev->next_block = next;
            next->prev_block = prev;
        } else { // case 4
            // we are at last block that next pointer points to null
            // bmd has a prev pointer that we need to make null
            prev->next_block = NULL;
            bmd->prev_block = NULL;
        }
    }
    // we add the block to be the head of the list
    if (flt->free_page_blocks == NULL) {
        flt->free_page_blocks = bmd;
    } else {
        struct BMD *head = flt->free_page_blocks;
        flt->free_page_blocks = bmd;
        bmd->next_block = head;
    }
}

/* the free list can look like before discard
 case 1:

           ┌────────┐  next
           │        ├────────►NULL
           │  bmd1  │
NULL◄──────┤        │
           └────┬───┘
                │
                ▼
           page to move

 case 2:
           ┌────────┐     next      ┌─────────┐   next    ┌─────────┐   next
           │        ├──────────────►│         ├──────────►│         │ ───────►NULL
           │  bmd1  │               │  bmd2   │           │  bmd3   │
NULL◄──────┤        │◄──────────────┤         │◄──────────┤         │
           └────┬───┘      prev     └─────────┘   prev    └─────────┘
                │
                ▼
           page to move

case 3:
           ┌────────┐     next      ┌─────────┐   next    ┌─────────┐   next
           │        ├──────────────►│         ├──────────►│         │ ───────►NULL
           │  bmd1  │               │  bmd2   │           │  bmd3   │
NULL◄──────┤        │◄──────────────┤         │◄──────────┤         │
           └────────┘      prev     └────┬────┘   prev    └─────────┘
                                         │
                                         ▼
                                    page to move
case 4:
           ┌────────┐     next      ┌─────────┐   next    ┌─────────┐   next
           │        ├──────────────►│         ├──────────►│         │ ───────►NULL
           │  bmd1  │               │  bmd2   │           │  bmd3   │
NULL◄──────┤        │◄──────────────┤         │◄──────────┤         │
           └────────┘      prev     └─────────┘   prev    └────┬────┘
                                                               │
                                                               ▼
                                                             page to move
 */
void remove_bmd_from_free_list(struct FLT *flt, struct BMD *bmd) {
    printf("Discard bmd %p from flt\n", bmd);
    struct BMD *prev = bmd->prev_block;
    // case 1 and 2
    if (prev == NULL) {
        // case 2
        if (bmd->next_block != NULL) {
            struct BMD *next = bmd->next_block;
            flt->free_page_blocks = next;
            next->prev_block = NULL;
        } else {
            //case 1
            flt->free_page_blocks = NULL;
        }
    } else {
        //case 3
        if (bmd->next_block != NULL) {
            // we are in the middle so block has a prev and a next
            // we need to link the prev next's pointer to next block of the bmd that we want to move
            // and the next block prev pointer to prev block of bmd
            struct BMD *next = bmd->next_block;
            prev->next_block = next;
            next->prev_block = prev;
        } else { // case 4
            // we are at last block that next pointer points to null
            // bmd has a prev pointer that we need to make null
            prev->next_block = NULL;
            bmd->prev_block = NULL;
        }
    }
}

void *flt_malloc(struct FLT *flt, int object_size, int page_size) {
    // if all pages are full or pages doesn't exist
    if (flt->free_page_blocks == NULL) {
        flt->free_page_blocks = create_BMD(object_size, page_size);
    }

    // go through the list to find a list with free blocks
    struct BMD *bmd = flt->free_page_blocks;
    while (bmd != NULL) {
        // if bmd structure has free slots (obtained after free operation) or has not been completed yet
        if (bmd->num_free != 0 || bmd->num_bumped < bmd->num_total) {
            void *to_return = block_malloc(bmd);
            if (bmd->num_bumped == bmd->num_total && bmd->num_free == 0) {
                move_to_full(flt, bmd);
            }
            return to_return;
        }
        bmd = bmd->next_block;
    }
    return NULL;
}

int flt_free(struct FLT *flt, void *ptr, int page_size) {
    void *current = flt->free_page_blocks;
    while (current != NULL) {
        struct BMD *bmd = current;
        if (ptr >= current && ptr < current + page_size) {
            block_free(bmd, ptr);
            if (is_page_empty(bmd)) {
                remove_bmd_from_free_list(flt, bmd);
                discard_empty_page(bmd, page_size);
            }
            return 1;
        }
        current = bmd->next_block;
    }
    current = flt->full_page_blocks;
    while (current != NULL) {
        struct BMD *bmd = current;
        if (ptr >= current && ptr < current + page_size) {
            block_free(bmd, ptr);
            if (bmd->num_bumped == bmd->num_total - 1 || bmd->num_free != 0) {
                move_to_free(flt, bmd);
            }
            return 1;
        }
        current = bmd->next_block;
    }
    return 0;
}
