#ifndef UNTITLED_OH_H
#define UNTITLED_OH_H

#include<stddef.h>
#include"bmd.h"
#include "sys/mman.h"
#include <stdlib.h>

struct OH {
    void *next_flt;
    void *next_in_memory;
    void *prev_flt;
    void *prev_in_memory;
    void *next_mdata;
    void *prev_mdata;
    int size;
    int flag; // 0 if slot is empty , 1 otherwise
};

extern int OH_size;

struct OH *init_OH(void *ptr);

void reset_OH(struct OH *oh);

void set_slot_occupied(struct OH *oh);

void set_free_slot(struct OH *oh);

void *memory_map(int page_size);

#endif //UNTITLED_OH_H
