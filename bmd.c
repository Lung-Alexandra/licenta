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

void *allocate_page( int page_size) {
//    size_t size =sizeof (struct BMD);
//    printf("%zu",size);
    void *ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        return NULL; // Handle allocation failure
    }
    return ptr;
}

void *create_BMD(int object_size, int page_size) {
//    printf("Pagina noua\n");
    void *ptr = allocate_page(page_size);

    // Initialize the BMD structure within the allocated memory:
    struct BMD *bmd = initialize_BMD(ptr);

    bmd->current_ptr = (void *) (ptr + sizeof(struct BMD)); // After BMD structure

    bmd->object_size = object_size;
    bmd->num_total = (page_size - sizeof(*bmd)) / object_size;

//    printf("Allocate page %p\n", ptr);
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

int is_page_empty(struct BMD *bmd) {
    if (bmd->num_free == bmd->num_total && bmd->num_bumped == bmd->num_total)
        return 1;
    return 0;
}

int block_free(struct BMD *bmd, void *ptr) {
    if (bmd->free_list == NULL) {
        bmd->free_list = ptr;
        *(void **)ptr = NULL;
    } else {
        void *old_head = bmd->free_list;
        bmd->free_list = ptr;

        void **old = ptr;
        *old = old_head;
    }
    bmd->num_free += 1;
    return 1;
}
//*(void**)bmd->free_list