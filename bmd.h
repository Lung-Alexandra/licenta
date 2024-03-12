#ifndef UNTITLED_BMD_H
#define UNTITLED_BMD_H
#include <sys/mman.h>
#include <stdio.h>
#define PAGE_SIZE 4096

struct BMD {
    void *prev_block;
    void *next_block;
    void *free_list;
    void *current_ptr;
    ssize_t num_total;
    ssize_t num_bumped;
    ssize_t num_free;
    size_t object_size;
};

struct BMD *initialize_BMD(void *ptr) {
    struct BMD* bmd = (struct BMD*)ptr;
    bmd->prev_block = NULL;
    bmd->next_block = NULL;
    bmd->free_list = NULL;
    bmd->current_ptr = NULL;
    bmd->num_total = 0;
    bmd->num_free = 0;
    bmd->num_bumped = 0;
    bmd->object_size = 0;
    return bmd;
}
void* allocate_page(int object_size) {
    size_t size =sizeof (struct BMD);
    printf("%zu",size);
    void* ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        return NULL; // Handle allocation failure
    }
    // Initialize the BMD structure within the allocated memory:
    struct BMD* bmd = initialize_BMD(ptr);

    bmd->free_list = ptr;  // Assuming free_list is at the beginning
    bmd->current_ptr = (void*)((char*)ptr + sizeof(struct BMD)); // After BMD structure

    bmd->object_size = object_size;
    bmd->num_total = (PAGE_SIZE- sizeof(bmd)) / object_size;
    bmd->num_free = (PAGE_SIZE - sizeof(bmd) )/ object_size;


    return ptr;
}

void *create_BMD(int object_size) {

    void *ptr = allocate_page(object_size);

    return ptr;
}

#endif //UNTITLED_BMD_H
