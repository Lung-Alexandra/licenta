#include "allocator.h"


void init() {
    for (int i = 0; i <= SMALL_CLASS_SIZE; ++i)
        initialize_FLT(&small_obj[i], (i + 1) * gap);
    for (int i = 0; i <= MEDIUM_CLASS_SIZE; ++i)
        initialize_FLT(&medium_obj[i], medium_min_size + (i + 1) * gap);
    for (int i = 0; i <= LARGE_CLASS_SIZE; ++i)
        initialize_FLT_LARGE(&large_obj[i]);

}

int get_class(int size, int class_size) {
    for (int i = 0; i <= class_size; ++i) {
        int class_max_size = (i + 1) * gap;
        if (size <= class_max_size)
            return i;
    }
    return -1;
}

int get_class_medium (int size, int class_size) {
    for (int i = 0; i <= class_size; ++i) {
        int class_max_size = medium_min_size + (i + 1) * gap;
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
            return flt_malloc(&small_obj[class], PAGE_SIZE);
        }
    }
    if (size > small_max_size && size <= medium_max_size) {
        int class = get_class_medium(size, MEDIUM_CLASS_SIZE);
        if (class != -1) {
            return flt_malloc(&medium_obj[class], 4 * PAGE_SIZE);
        }
    }
    if (size > medium_max_size && size <= large_max_size) {
        return flt_malloc_large(large_obj, size, 20 * PAGE_SIZE);
    }
    return NULL;
}

void ffree(void *ptr) {
    int ok = 0;
    for (int i = 0; i <= SMALL_CLASS_SIZE; ++i) {
        int res = flt_free(&small_obj[i], ptr, PAGE_SIZE);
        if (res) {
            ok = 1;
            break;
        }
    }
    for (int i = 0; i <= MEDIUM_CLASS_SIZE; ++i) {
        int res = flt_free(&medium_obj[i], ptr, 4 * PAGE_SIZE);
        if (res) {
            ok = 1;
            break;
        }
    }
    if (ok == 0) {
        printf("--------I--------\n");
        for (int j = LARGE_CLASS_SIZE; j >= 0; j--) {
            void *current = large_obj[j].free_list;
            if (current != NULL) {
                printf("flt[%d, %d]: ", (large_min_size + j * gap), j);
                while (current != NULL) {
                    struct OH *oh = (struct OH *) (current);
                    printf( "%p (%d)(next:%p), ", current, oh->size, oh->next);
                    current = ((struct OH *) current)->next;
                }
                printf( "\n");
            }
        }
        printf("-----------------\n");

        flt_free_large(large_obj, ptr);
        printf("--------A--------\n");

        for (int j = LARGE_CLASS_SIZE; j >= 0; j--) {
            void *current = large_obj[j].free_list;
            if (current != NULL) {
                printf("flt[%d, %d]: ", (large_min_size + j * gap), j);
                while (current != NULL) {
                    struct OH *oh = (struct OH *) (current);
                    printf( "%p (%d)(next:%p), ", current, oh->size, oh->next);
                    current = ((struct OH *) current)->next;
                }
                printf( "\n");
            }
        }
        printf("-----------------\n");

    }
}