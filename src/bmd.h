#ifndef UNTITLED_BMD_H
#define UNTITLED_BMD_H

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

#define PAGE_SIZE sysconf(_SC_PAGESIZE)

struct BMD {
    void *prev_block;
    void *next_block;
    void *free_list;
    void *current_ptr;
    size_t num_total;
    size_t num_bumped;
    size_t num_free;
    size_t object_size;
};

struct BMD *initialize_BMD(void *ptr);

void *allocate_page(int page_size);

void *create_BMD(int object_size, int page_size);

void *block_malloc(struct BMD *bmd);

int is_page_empty(struct BMD *bmd);

int block_free(struct BMD *bmd, void *ptr);

#endif //UNTITLED_BMD_H
