#include "extrem_large.h"

struct Header *init_Header(void *ptr) {
    struct Header *head = (struct Header *) ptr;
    head->size = 0;
    head->prev = NULL;
    head->next = NULL;
    return head;
}

void add_to_extrem_large_obj(void *extreme_large_obj, struct Header *head) {
    if (extreme_large_obj == NULL) {
        extreme_large_obj = head;
    } else {
        struct Header *old_head = extreme_large_obj;
        extreme_large_obj = head;
        head->next = old_head;
        old_head->prev = head;
    }
}

void remove_from_extrem_large_obj(void *extreme_large_obj, struct Header *head) {
    if(head-> prev == NULL){

    }
}

void *allocate_extrem_large_obj(void *extreme_large_obj, int size) {
    // allocate multiple of page
    int new_size = (size + Header_size) + (size + Header_size) % PAGE_SIZE;
    void *new_space_ptr = memory_map(new_size);
    struct Header *header = init_Header(new_space_ptr);
    header->size = new_size - Header_size;
    add_to_extrem_large_obj(extreme_large_obj, header);
    void *to_return = new_space_ptr + Header_size;
    return to_return;
}

void free_extreme_large_obj(void *extreme_large_obj, void *ptr) {
    void *head = ptr - Header_size;
    struct Header *header = init_Header(head);
    remove_from_extrem_large_obj(extreme_large_obj, header);
    int size = header->size + Header_size;
    discard_empty_page(head,size);
}