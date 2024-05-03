#include "oh.h"

struct OH *init_OH(void *ptr) {
    struct OH *oh = (struct OH *) ptr;
    oh->size = 0;
    oh->flag = 0;
    oh->slots_occupied = 0;
    oh->prev = NULL;
    oh->prev_cut = NULL;
    oh->next = NULL;
    return oh;
}

void *memory_map(int page_size) {
    void *ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        return NULL; // Handle allocation failure
    }
    return ptr;
}

void set_slot_occupied(struct OH *oh) {
    oh->flag = 1;
    oh->slots_occupied += 1;
}

void set_free_slot(struct OH *oh) {
    oh->flag = 0;
    oh->slots_occupied -= 1;
}

