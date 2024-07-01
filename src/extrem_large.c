#include"extrem_large.h"

int total_size = 0;

void add_to_extrem_large_obj(void *extreme_large_obj, struct OH *head) {
    if (extreme_large_obj == NULL) {
        extreme_large_obj = head;
    } else {
        struct OH *old_head = extreme_large_obj;
        extreme_large_obj = head;
        head->next_mdata = old_head;
        old_head->prev_mdata = head;
    }
}

void remove_from_extrem_large_obj(void *extreme_large_obj, struct OH *head) {
    if (head->prev_mdata == NULL) {
        struct OH *next_md = head->next_mdata;
        extreme_large_obj = next_md;
        if (next_md != NULL) {
            next_md->prev_mdata = NULL;
        }
    } else {
        struct OH *prev = head->prev_mdata;
        struct OH *next = head->next_mdata;
        if (next != NULL) {
            prev->next_mdata = next;
            next->prev_mdata = prev;
        }
        else{
            prev->next_mdata = NULL;
        }
    }
    head->next_mdata = NULL;
    head->prev_mdata = NULL;
}

void *allocate_extrem_large_obj(void *extreme_large_obj, int size) {
    // allocate multiple of page
    int new_size = (size + OH_size) + (size + OH_size) % PAGE_SIZE;
    total_size += new_size;
    void *new_space_ptr = memory_map(new_size);
    struct OH *oh = init_OH(new_space_ptr);
    oh->size = new_size - OH_size;
    add_to_extrem_large_obj(extreme_large_obj, oh);
    void *to_return = new_space_ptr + OH_size;
    return to_return;
}

void free_extreme_large_obj(void *extreme_large_obj, void *ptr) {
    void *head = ptr - OH_size;
    struct OH *oh = head;
    remove_from_extrem_large_obj(extreme_large_obj, oh);
    int size = oh->size + OH_size;
    total_size -=size;
    discard_empty_extrobj(head, size);
}