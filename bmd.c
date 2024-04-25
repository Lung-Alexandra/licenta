#include "bmd.h"

struct BMD *initialize_BMD(void *ptr) {
    struct BMD *bmd = (struct BMD *) ptr;
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

void *allocate_page(int object_size) {
//    size_t size =sizeof (struct BMD);
//    printf("%zu",size);
    void *ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        return NULL; // Handle allocation failure
    }
    // Initialize the BMD structure within the allocated memory:
    struct BMD *bmd = initialize_BMD(ptr);

    bmd->current_ptr = (void *) (ptr + sizeof(struct BMD)); // After BMD structure

    bmd->object_size = object_size;
    bmd->num_total = (PAGE_SIZE - sizeof(*bmd)) / object_size;

    return ptr;
}

void *create_BMD(int object_size) {
    printf("Pagina noua\n");
    void *ptr = allocate_page(object_size);
    printf("Allocate page %p\n", ptr);
    return ptr;
}

void *block_malloc(struct BMD *bmd) {
    void *to_return = NULL;
    if (bmd->free_list == NULL) {
        // alocate contiguous memory
        to_return = bmd->current_ptr;
        bmd->current_ptr += bmd->object_size;
        bmd->num_bumped += 1;

    } else {
        to_return = bmd->free_list;
        void *next = *(void **) (bmd->free_list);
        bmd->free_list = next;
        bmd->num_free -= 1;
    }
    return to_return;
}

void *block_free(struct BMD *bmd, void *ptr) {
    if (bmd->free_list == NULL) {
        bmd->free_list = ptr;
    } else {
        void *old_head = bmd->free_list;
        bmd->free_list = ptr;

        void **old = ptr;
        *old = old_head;
    }
    bmd->num_free += 1;
//    if (is_page_empty(bmd, PAGE_SIZE)) {
//        discard_empty_page(bmd, PAGE_SIZE);
//    }
    return ptr;
}
//*(void**)bmd->free_list