#include "allocator.h"

void init() {
    for (int i = 0; i < NUM_SMALL_CLASSES; ++i)
        initialize_FLT(&small_obj[i], (i + 1) * gap);
    for (int i = 0; i < NUM_MEDIUM_CLASSES; ++i)
        initialize_FLT(&medium_obj[i], medium_min_size + i * gap);
    init_first();
    for (int i = 0; i < NUM_LARGE_CLASSES; ++i)
        initialize_FLT_LARGE(&large_obj[i]);
    extreme_large_obj = NULL;
}

int get_class_small(int size) {
    for (int i = 0; i < NUM_SMALL_CLASSES; ++i) {
        int class_max_size = (i + 1) * gap;
        if (size <= class_max_size)
            return i;
    }
    return -1;
}

int get_class_medium(int size) {
    for (int i = 0; i < NUM_MEDIUM_CLASSES; ++i) {
        int class_max_size = medium_min_size + i * gap;
        if (size <= class_max_size)
            return i;
    }
    return -1;
}

void *alloc(int size) {
    if (size <= small_max_size) {
        int class = get_class_small(size);
//        printf("%d", class);
        if (class != -1) {
            return flt_malloc(&small_obj[class], PAGE_SIZE);
        }
    }
    if (size > small_max_size && size <= medium_max_size) {
        int class = get_class_medium(size);
        if (class != -1) {
            return flt_malloc(&medium_obj[class], 4 * PAGE_SIZE);
        }
    }
    if (size > medium_max_size && size <= large_max_size) {
        return flt_malloc_large(large_obj, size, 20 * PAGE_SIZE);
    }
//    if(size > large_max_size){
//        return allocate_extrem_large_obj(extreme_large_obj,size);
//    }
    return NULL;
}

void ffree(void *ptr) {
    int ok = 0;
    for (int i = 0; i < NUM_SMALL_CLASSES; ++i) {
        int res = flt_free(&small_obj[i], ptr, PAGE_SIZE);
        if (res) {
            ok = 1;
            break;
        }
    }
    for (int i = 0; i < NUM_MEDIUM_CLASSES; ++i) {
        int res = flt_free(&medium_obj[i], ptr, 4 * PAGE_SIZE);
        if (res) {
            ok = 1;
            break;
        }
    }
    if (ok == 0) {
        struct OH *header = (void *) (ptr - OH_size);
        if (header->size <= large_max_size)
            flt_free_large(large_obj, ptr);
        else free_extreme_large_obj(extreme_large_obj, ptr);
    }
}

void destr() {
    for (int i = 0; i < NUM_SMALL_CLASSES; ++i) {
        struct FLT *flt = &small_obj[i];
        if (flt->free_page_blocks != NULL) {
            struct BMD *head = flt->free_page_blocks;
            remove_bmd_from_free_list(flt, head);
            discard_empty_page(head, PAGE_SIZE);
        }
        if (flt->full_page_blocks != NULL) {
            struct BMD *head = flt->full_page_blocks;
            while (head != NULL) {
                discard_empty_page(head, PAGE_SIZE);
                head = head->next_block;
            }
        }
    }
    for (int i = 0; i < NUM_MEDIUM_CLASSES; ++i) {
        struct FLT *flt = &medium_obj[i];
        if (flt->free_page_blocks != NULL) {
            struct BMD *head = flt->free_page_blocks;
            remove_bmd_from_free_list(flt, head);
            discard_empty_page(head, 4*PAGE_SIZE);
        }
        if (flt->full_page_blocks != NULL) {
            struct BMD *head = flt->full_page_blocks;
            while (head != NULL) {
                struct BMD * next = head->next_block;
                discard_empty_page(head, 4*PAGE_SIZE);
                head = next;
            }
        }
    }
    if(k!=0){
        for(int i =0;i<k;++i){
            void * allocated = first[i];
            discard_empty_page(allocated,20*PAGE_SIZE);
        }
    }
//    if(extreme_large_obj != NULL){
//        struct OH *head = extreme_large_obj;
//        while (head != NULL) {
//            discard_empty_page(head, head->size);
//            head = head->next_flt;
//        }
//    }
}