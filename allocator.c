#include "allocator.h"


void init() {
    for (int i = 0; i < NUM_SMALL_CLASSES; ++i)
        initialize_FLT(&small_obj[i], (i + 1) * gap);
    for (int i = 0; i < NUM_MEDIUM_CLASSES; ++i)
        initialize_FLT(&medium_obj[i], medium_min_size + i * gap);
    for (int i = 0; i < NUM_LARGE_CLASSES; ++i)
        initialize_FLT_LARGE(&large_obj[i]);

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
//        printf("-----before-----\n");
//        for (int j = NUM_LARGE_CLASSES - 1; j >= 0; j--) {
//            struct OH *current = large_obj[j].free_list;
//            if (current != NULL) {
//                printf("flt[size:%d, class:%d]: \n", (large_min_size + j * gap),j);
//                while (current != NULL) {
//                    printf("%p (prev:%p)(%d)(next:%p), ", current, current->prev_flt, current->size, current->next_flt);
//                    current = current->next_flt;
//                }
//                printf("\n");
//            }
//        }

//        printf("--------mem before-------\n");
//        struct OH *current = large_obj[NUM_LARGE_CLASSES-1].free_list;
//        while(current!= NULL){
//            printf("%p (prev:%p)(%d)(flag:%d)(next:%p), \n", current, current->prev_in_memory, current->size,current->flag, current->next_in_memory);
//            current = current->next_in_memory;
//        }
//        printf("-----------------\n");

        flt_free_large(large_obj, ptr);

//        printf("--------mem after-------\n");
//        current = large_obj[NUM_LARGE_CLASSES-1].free_list;
//        while(current!= NULL){
//            printf("%p (prev:%p)(%d)(flag:%d)(next:%p), \n", current, current->prev_in_memory, current->size,current->flag, current->next_in_memory);
//            current = current->next_in_memory;
//        }
//        printf("--------mem-------\n");

//        printf("-----after------\n");
//
//        for (int j = NUM_LARGE_CLASSES - 1; j >= 0; j--) {
//            struct OH *current = large_obj[j].free_list;
//            if (current != NULL) {
//                printf("flt[size:%d, class:%d]: \n", (large_min_size + j * gap),j);
//                while (current != NULL) {
//                    printf("%p (prev:%p)(%d)(next:%p), ", current, current->prev_flt, current->size, current->next_flt);
//                    current = current->next_flt;
//                }
//                printf("\n");
//            }
//        }
//        printf("-----------------\n");

    }
}