#ifndef UNTITLED_EXTREM_LARGE_H
#define UNTITLED_EXTREM_LARGE_H

#include<stddef.h>
#include"page_discard.h"
#include"oh.h"

extern int total_size;

void add_to_extrem_large_obj(void *extreme_large_obj, struct OH *head);

void remove_from_extrem_large_obj(void *extreme_large_obj, struct OH *head);

void *allocate_extrem_large_obj(void *extreme_large_obj, int size);

void free_extreme_large_obj(void *extreme_large_obj, void *ptr);

#endif //UNTITLED_EXTREM_LARGE_H
