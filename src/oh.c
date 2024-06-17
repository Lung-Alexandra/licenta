#include "oh.h"

struct OH *init_OH(void *ptr) {
        struct OH *oh = (struct OH *) ptr;
        oh->size = 0;
        oh->flag = 0;
        oh->prev_flt = NULL;
        oh->next_flt = NULL;
        oh->prev_in_memory = NULL;
        oh->next_in_memory = NULL;
        oh-> next_mdata = NULL;
        oh-> prev_mdata = NULL;
        return oh;
}

void reset_OH(struct OH *oh) {
    oh->size = 0;
    oh-> flag = 0;
    oh->next_in_memory = NULL;
    oh->prev_in_memory = NULL;
    oh->next_flt = NULL;
    oh->prev_flt = NULL;
    oh-> next_mdata = NULL;
    oh-> prev_mdata = NULL;
}

void *memory_map(int page_size) {
    void *ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        exit(1); // Handle allocation failure
    }
    return ptr;
}

void set_slot_occupied(struct OH *oh) {
    oh->flag = 1;
}

void set_free_slot(struct OH *oh) {
    oh->flag = 0;
}

