#include "extrem_large.h"


void add_to_extrem_large_obj(void *extreme_large_obj, struct OH *head) {
    if (extreme_large_obj == NULL) {
        extreme_large_obj = head;
    } else {
        struct OH *old_head = extreme_large_obj;
        extreme_large_obj = head;
        head->next_flt = old_head;
        old_head->prev_flt = head;
    }
}

void remove_from_extrem_large_obj(void *extreme_large_obj, struct OH *head) {
    if(head-> prev_flt == NULL){
        if(head -> next_flt != NULL){
            extreme_large_obj = head->next_flt;
        }
        else extreme_large_obj =  NULL;
    }
    else{

    }
}

void *allocate_extrem_large_obj(void *extreme_large_obj, int size) {
    // allocate multiple of page
    int new_size = (size + OH_size) + (size + OH_size) % PAGE_SIZE;
    void *new_space_ptr = memory_map(new_size);
    struct OH *OH = init_OH(new_space_ptr);
    OH->size = new_size - OH_size;
    add_to_extrem_large_obj(extreme_large_obj, OH);
    void *to_return = new_space_ptr + OH_size;
    return to_return;
}

void free_extreme_large_obj(void *extreme_large_obj, void *ptr) {
    void *head = ptr - OH_size;
    struct OH *OH = init_OH(head);
    remove_from_extrem_large_obj(extreme_large_obj, OH);
    int size = OH->size + OH_size;
//    discard_empty_page(head,size);
}