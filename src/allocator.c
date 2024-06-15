#include "allocator.h"

void init() {
    allocator_size = 0;
    for (int i = 0; i < NUM_SMALL_CLASSES; ++i)
        initialize_FLT(&small_obj[i], (i + 1) * gap);
    for (int i = 0; i < NUM_MEDIUM_CLASSES; ++i)
        initialize_FLT(&medium_obj[i], medium_min_size + i * gap);
//    init_MData(&track_pages);
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
//        printf("%d\n", class);
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
    if (ok == 0) {
        for (int i = 0; i < NUM_MEDIUM_CLASSES; ++i) {
            int res = flt_free(&medium_obj[i], ptr, 4 * PAGE_SIZE);
            if (res) {
                ok = 1;
                break;
            }
        }
    }
    if (ok == 0) {
        struct OH *header = (void *) (ptr - OH_size);
        if (header->size <= large_max_size)
            flt_free_large(large_obj, ptr);
        else free_extreme_large_obj(extreme_large_obj, ptr);
    }
}
