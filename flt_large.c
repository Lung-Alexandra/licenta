#include "oh.c"
#include "flt_large.h"

void initialize_FLT_LARGE(struct FLT_LARGE *flt) {
    flt->free_list = NULL;
}

void move_to_free_list(struct FLT_LARGE *flt, struct OH *oh) {
    if (flt->free_list == NULL) {
        flt->free_list = oh;
    } else {
        struct OH *header = (struct OH *) flt->free_list;
        oh->next = header;
        header->prev = oh;
        flt->free_list = oh;
    }
}

void remove_from_free_list(struct FLT_LARGE *flt, struct OH *oh) {
    // if oh point to a next we need to move next to be the oh of free list
    struct OH *next = (struct OH *) oh->next;
    if (oh->next != NULL) {
        flt->free_list = next;
        struct OH *prev = (struct OH *) oh->prev;
        if (prev != NULL) {
            prev->next = next;
            next->prev = prev;
        } else {
            next->prev = NULL;
        }
        oh->next = NULL;
    } else {
        struct OH *prev = (struct OH *) oh->prev;
        if (prev != NULL) {
            flt->free_list = prev;
            prev->next = NULL;
        } else {
            flt->free_list = NULL;
        }
    }
}

int flt_large_calculate_class(int object_size) {
    int class = 0;
    if (object_size > large_min_size) {
        class = (object_size - large_min_size) / gap + 1;
    }
    return class;
}

// returns the class in which object will be allocated
// if class = -1 means that we have no space
// or we didn't allocate space (20 pages)
unsigned int flt_find_class(struct FLT_LARGE *flt, int obj_size) {
    unsigned int class = -1;
    // no need to search in flt with size smaller than object size
    int start = flt_large_calculate_class(obj_size);
    for (int i = start; i < NUM_LARGE_CLASSES; i++) {
        struct FLT_LARGE *current_flt = &flt[i];
        if (current_flt->free_list != NULL) {
            class = i;
            break;
        }
    }
    return class;
}


void flt_large_new_page_init(struct FLT_LARGE *flt, int page_size) {
    // the page has 80K,so we split it in 2*32k and 16k
    void *new_page_ptr = memory_map(page_size);
    // create the first 32k OH and put in the free list
    struct OH *oh = init_OH(new_page_ptr);
    oh->size = large_max_size;
    flt[NUM_LARGE_CLASSES-1].free_list = new_page_ptr;
    printf("Add  fara header: %p\n", new_page_ptr);
    printf("Add : %p\n", new_page_ptr + struct_size);
    //Link with the second one and initialize its OH
    void *ptr = (void *) (new_page_ptr + oh->size);
    oh->next = ptr;
    printf("Add  fara header: %p\n", ptr);
    printf("Add : %p\n", ptr + struct_size);
    struct OH *oh1 = init_OH(ptr);
    oh1->size = large_max_size;
    oh1->prev = oh;

    ptr = (void *) (ptr + oh1->size);
    printf("Add  fara header: %p\n", ptr);
    printf("Add : %p\n", ptr + struct_size);
    struct OH *oh2 = init_OH(ptr);
    oh2->size = page_size - oh->size - oh1->size - 3 * struct_size;
    unsigned int class_last_space = flt_large_calculate_class(oh2->size);
    flt[class_last_space].free_list = ptr;
//    printf("class size: %d , oh->size : %d\n",class_last_space*8+504,oh2->size);
}

// FLT_LARGE free_list points always to OH
void *flt_malloc_large(struct FLT_LARGE *flt, int obj_size, int page_size) {
    unsigned int no_space = 0;
    unsigned int class = flt_find_class(flt, obj_size);
    if (class != -1) {
        no_space = 1;
    }

    // if no_space is 0 means that we need to allocate a new page
    if (no_space == 0) {
        flt_large_new_page_init(flt, page_size);
        int size_rest = page_size - 2 * large_max_size - 3 * struct_size;
        // need to choose where object fits
        // in 32k or 16k
        if (obj_size > size_rest) {
            class = NUM_LARGE_CLASSES;
        } else {
            class = flt_large_calculate_class(size_rest);
        }
    }
//    printf("class size: %d , oh->size : %d\n",class*8+504,obj_size);

    // the class in which object fit's is known
    struct FLT_LARGE *current_flt = &flt[class];
    void *ptr = current_flt->free_list;
    struct OH *head = (struct OH *) ptr;
    printf("header: %p\n", head);
    printf("Add fara header: %p\n", ptr);
    remove_from_free_list(current_flt, head);
    set_slot_occupied(head);

    int old_size = head->size;
    head->size = obj_size;
    printf("%d\n",obj_size );
    printf("Add fara header: %p\n", ptr);
    printf("Add returned: %p\n", (void *) (ptr + struct_size));
//
    void *to_return = (void *) (ptr + struct_size);
    printf("to_return : %p\n", to_return);

    //calculating the class for remaining space
    int space_empty = old_size - obj_size;
    void * rest_ptr =(void *) (to_return + head->size);
    printf("rest_ptr: %p\n", rest_ptr);
    printf("rest : %p\n", ptr + struct_size+head->size);
    struct OH *rest = init_OH(rest_ptr);
    rest->size = space_empty - struct_size;
    class = flt_large_calculate_class(rest->size);
    rest->prev_cut = head;
    printf("Free list flt class alloc:%d, Size: %d\n", class, rest->size);

    // if space without header is large enough tot keep the minimum object of large obj
    // we need to add it to the corresponding flt class
    if (rest->size >= large_min_size) {
        struct FLT_LARGE *rest_flt = &flt[class];
        move_to_free_list(rest_flt, rest_ptr);
    }
    return to_return;
}

// we move from prev to prev adding the size of current oh to prev
void *coalesce_prev(struct FLT_LARGE *flt, struct OH *oh) {
    unsigned int class;
    struct OH *prev = oh->prev_cut;

    while (prev != NULL && prev->flag == 0 &&
           oh->size + prev->size + struct_size <= large_max_size) {
//        prev->slots_occupied += header->slots_occupied - 1;
        if (oh->size != 0 && prev->size != 0) {
            class = flt_large_calculate_class(prev->size);

            struct FLT_LARGE *to_remove_flt = &flt[class];
            remove_from_free_list(to_remove_flt, prev);
            printf("Header prev class free:%d, Size: %d\n", class, prev->size);

            printf("prev %d uneste cu  %d \n", prev->size, oh->size);
            prev->size += oh->size + struct_size;
            oh->size = 0;
            oh = prev;

        }
        prev = prev->prev_cut;
    }
    return oh;
}

// we move from next to next adding the size of next oh to current oh
void *coalesce_next(struct FLT_LARGE *flt, struct OH *oh, void *ptr) {
    unsigned int class;
    struct OH *next = (struct OH *) (ptr);
    while (next != NULL && next->size != 0 && next->flag == 0 &&
           oh->size + next->size + struct_size <= large_max_size) {
//        header->slots_occupied += next->slots_occupied - 1;

        class = flt_large_calculate_class(next->size);
        printf("header next class free:%d, Size: %d\n", class, next->size);
        struct FLT_LARGE *to_remove_flt = &flt[class];
        remove_from_free_list(to_remove_flt, next);

        printf("%d uneste cu next %d \n", oh->size, next->size);
        oh->size += next->size + struct_size;

        ptr += next->size + struct_size;
        next->size = 0;
        next = (struct OH *) (ptr);
    }
    if (next != NULL && next->flag == 1) {
        next->prev_cut = (void *)oh;
    }
    return oh;
}

void flt_free_large(struct FLT_LARGE *flt, void *ptr) {
    unsigned int class;
    printf("FREE : %p\n", ptr);
    void *header_ptr = (void *) (ptr - struct_size);
    printf("FREE header : %p\n", header_ptr);
    struct OH *header = (struct OH *) header_ptr;

    ptr += header->size;

    set_free_slot(header);

    printf("free %d \n", header->size);

    header = coalesce_prev(flt, header);

    header = coalesce_next(flt, header, ptr);

    // verify if  we can discard page
//   if (flNUM_LARGE_CLASSES) discard_empty_page(header,20 * PAGE_SIZE);

    class = flt_large_calculate_class(header->size);

    printf("header class free:%d, Size: %d\n", class, header->size);

    if (header->size >= large_min_size) {
        struct FLT_LARGE *to_add_flt = &flt[class];
        header_ptr = (void*)header;
        move_to_free_list(to_add_flt, header_ptr);
    }

}