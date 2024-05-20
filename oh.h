#ifndef UNTITLED_OH_H
#define UNTITLED_OH_H
struct OH {
    void *next_flt;
    void *next_in_memory;
    void *prev_flt;
    void *prev_in_memory;
    int size;
    int flag; // 0 if slot is empty , 1 otherwise
};

int OH_size = sizeof(struct OH);

struct OH *init_OH(void *ptr);

void reset_OH(struct OH *oh);

void set_slot_occupied(struct OH *oh);

void set_free_slot(struct OH *oh);

#endif //UNTITLED_OH_H
