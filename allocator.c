#include "allocator.h"

void init() {
    for (int i = 0; i < SMALL_CLASS_SIZE; ++i)
        initialize_FLT(&small_obj[i]);
    //medium obj
    //large

}

int get_class(int size, int class_size) {
    for (int i = 0; i < class_size; ++i) {
       int class_max_size=(i+1)*gap;
        if (size <= class_max_size)
            return i;
    }
    return -1;
}


void *alloc(int size) {
    if (size <= small_max_size) {
        int class = get_class(size, SMALL_CLASS_SIZE);
//        printf("%d", class);
        if (class != -1) {
            return flt_malloc(&small_obj[class], (class+1)*gap);
        }
    }
    if (size >= medium_min_size && size <= medium_max_size) {
        int class = get_class(size, MEDIUM_CLASS_SIZE);
//        printf("%d", class);
        if (class != -1) {
            return flt_malloc(&medium_obj[class], size);
        }
    }
    return NULL;
}

void ffree(void *ptr) {
    for (int i = 0; i < SMALL_CLASS_SIZE; ++i) {
        int res = flt_free(&small_obj[i], ptr);
        if (res) {
            break;
        }
    }
}