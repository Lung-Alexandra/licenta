#ifndef UNTITLED_OH_H
#define UNTITLED_OH_H
struct OH {
    void *next;
    void *prev;
    void *prev_in_memory;
    int size;
    int flag; // 0 if slot is empty , 1 otherwise
    int slots_occupied;
};

int struct_size = sizeof(struct OH);

struct OH *init_OH(void *ptr);

void set_slot_occupied(struct OH *oh);

void set_free_slot(struct OH *oh);

#endif //UNTITLED_OH_H
