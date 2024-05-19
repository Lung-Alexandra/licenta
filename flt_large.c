#include "oh.c"
#include "flt_large.h"

void initialize_FLT_LARGE(struct FLT_LARGE *flt) {
    flt->free_list = NULL;
}

void move_to_free_list(struct FLT_LARGE *flt, struct OH *oh) {
    oh->prev_flt = NULL;
    if (flt->free_list == NULL) {
        flt->free_list = oh;
        oh->next_flt = NULL;
    } else {
        struct OH *header = flt->free_list;
        oh->next_flt = header;
        header->prev_flt = oh;
        flt->free_list = oh;
    }
}

void remove_from_free_list(struct FLT_LARGE *flt, struct OH *oh) {
    // if oh is first element
    if (oh->prev_flt == NULL) {
        flt->free_list = oh->next_flt;
        // next prev becomes null because oh prev is null
        if (oh->next_flt != NULL) {
            struct OH *next = oh->next_flt;
            next->prev_flt = NULL;
            oh->next_flt = NULL;
        }
    } else {
        //in the middle or last
        struct OH *prev = oh->prev_flt;
        prev->next_flt = oh->next_flt;
        if (oh->next_flt != NULL) {
            struct OH *next_flt = oh->next_flt;
            next_flt->prev_flt = oh->prev_flt;
            oh->next_flt = NULL;
        }
        oh->prev_flt = NULL;
    }
}

int calculate_obj_to_class(int object_size) {
    int class = 0;
    if (object_size > large_min_size) {
        int rest = (object_size - large_min_size) % gap;
        class = (object_size - large_min_size) / gap ;
        if (rest){
            class++;
        }
    }
    return class;
}

int calculate_class_to_obj(int class) {
    int obj_size = large_min_size + class * gap;
    return obj_size;
}

// returns the class in which object will be allocated
// if class = -1 means that we have no space,
// or we didn't allocate space (20 pages)
int flt_find_class(struct FLT_LARGE *flt, int obj_size) {
    int class = -1;
    // no need to search in flt with size smaller than object size
    int start = calculate_obj_to_class(obj_size);
    for (int i = start; i < NUM_LARGE_CLASSES; i++) {
        struct FLT_LARGE *current_flt = &flt[i];
        if (current_flt->free_list != NULL && calculate_class_to_obj(i) >= obj_size) {
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
    flt[NUM_LARGE_CLASSES - 1].free_list = oh;
//    printf("Add : %p\n", oh);

    //Link with the second one and initialize its OH
    void *ptr = (void *) (new_page_ptr + oh->size + OH_size);
    struct OH *oh1 = init_OH(ptr);
    oh->next_flt = oh1;
    oh->next_in_memory = oh1;

    oh1->size = large_max_size;
    oh1->prev_flt = oh;
    oh1->prev_in_memory = oh;

    ptr = (void *) (ptr + oh1->size + OH_size);
//    printf("Add : %p\n", oh1);

    struct OH *oh2 = init_OH(ptr);
//    printf("Add : %p\n", oh2);
    oh2->prev_in_memory = oh1;
    oh1->next_in_memory = oh2;

    oh2->size = page_size - oh->size - oh1->size - 3 * OH_size;
    int class_last_space = calculate_obj_to_class(oh2->size);
    flt[class_last_space].free_list = oh2;

}

void split(struct FLT_LARGE *flt, struct OH *head, void *to_return, int obj_size) {

    int old_size = head->size;
    int fit_class = calculate_obj_to_class(obj_size);
    int alloc_size = calculate_class_to_obj(fit_class);
    head->size = alloc_size;

    printf("alloc class size %d\n", alloc_size);

//        printf("to_return : %p\n", to_return);


    void *rest_ptr = (void *) (to_return + alloc_size);

    struct OH *rest = init_OH(rest_ptr);
//        printf("rest: %p\n", rest);

    // the space without header is large enough tot keep the minimum object of large obj
    // we need to add it to the corresponding flt class
    int space_empty = old_size - alloc_size - OH_size;
    rest->size = space_empty;

    printf("rest: %d\n", rest->size);
    // calculating the class for remaining space
    int class = calculate_obj_to_class(rest->size);
    // link rest prev pointer with head because it was cut from it
    rest->prev_in_memory = head;
    // make rest next to point to head next
    rest->next_in_memory = head->next_in_memory;
    // link next's prev pointer with rest because its next must need to point to rest
    // not to old head
    if (head->next_in_memory != NULL) {
        struct OH *next_header = head->next_in_memory;
        next_header->prev_in_memory = rest;
    }
    // make head next to point to rest
    head->next_in_memory = rest;

    printf("Free list flt class alloc:%d, Size: %d\n", class, rest->size);

    struct FLT_LARGE *rest_flt = &flt[class];
    move_to_free_list(rest_flt, rest);
}

// FLT_LARGE free_list points always to OH
void *flt_malloc_large(struct FLT_LARGE *flt, int obj_size, int page_size) {
    unsigned int no_space = 0;
    int class = flt_find_class(flt, obj_size);
    if (class != -1) {
        no_space = 1;
    }
    // if no_space is 0 means that we need to allocate a new page
    if (no_space == 0) {
        flt_large_new_page_init(flt, page_size);
        class = flt_find_class(flt, obj_size);
    }

    // the class in which object fit's is known
    struct FLT_LARGE *current_flt = &flt[class];
    void *ptr = current_flt->free_list;
    struct OH *head = ptr;
    //    printf("header: %p\n", head);

    remove_from_free_list(current_flt, head);
    set_slot_occupied(head);

    void *to_return = (void *) (ptr + OH_size);

    printf("Want to alloc %d ", obj_size);

    // if space without header is large enough to keep the minimum object of large obj
    if (head->size - obj_size - OH_size >= large_min_size) {
        split(flt, head, to_return, obj_size);
    }
    else{
        printf("Allocated %d\n", head->size);
    }

    return to_return;
}

// we move from prev to prev adding the size of current oh to prev
void *coalesce_prev(struct FLT_LARGE *flt, struct OH *oh) {
    struct OH *prev = oh->prev_in_memory;

    while (prev != NULL && prev->flag == 0 && oh->size + prev->size + OH_size <= large_max_size) {

        if (oh->size != 0 && prev->size != 0) {
            int class = calculate_obj_to_class(prev->size);

            struct FLT_LARGE *to_remove_flt = &flt[class];
            remove_from_free_list(to_remove_flt, prev);
            printf("Header prev %p class free:%d, Size: %d\n", prev, class, prev->size);

            printf("prev %d uneste cu  %d \n", prev->size, oh->size);
            prev->size += oh->size + OH_size;
            prev->next_in_memory = oh->next_in_memory;
            oh->next_in_memory = NULL;
            oh->prev_in_memory = NULL;
            oh->size = 0;
            oh = prev;
        }
        prev = oh->prev_in_memory;
    }
    return oh;
}

// we move from next to next adding the size of next oh to current oh
void *coalesce_next(struct FLT_LARGE *flt, struct OH *oh) {
    struct OH *next = oh->next_in_memory;
    while (next != NULL && next->size != 0 && next->flag == 0 &&
           oh->size + next->size + OH_size <= large_max_size) {

        int class = calculate_obj_to_class(next->size);
        printf("header next %p class free:%d, Size: %d\n", next, class, next->size);
        struct FLT_LARGE *to_remove_flt = &flt[class];
        remove_from_free_list(to_remove_flt, next);

        printf("%d uneste cu next %d \n", oh->size, next->size);
        oh->size += next->size + OH_size;

        oh->next_in_memory = next->next_in_memory;
        next->prev_in_memory = NULL;
        next->next_in_memory = NULL;
        next->size = 0;
        next = oh->next_in_memory;
    }
    if (next != NULL && next->size != 0 && next->flag == 1) {
        next->prev_in_memory = oh;
    }
    return oh;
}

void flt_free_large(struct FLT_LARGE *flt, void *ptr) {

    void *header_ptr = (void *) (ptr - OH_size);
    printf("FREE : %p\n", header_ptr);

    struct OH *header = header_ptr;
    printf("free %d \n", header->size);

    if (header->size != 0) {
        set_free_slot(header);

//        header = coalesce_prev(flt, header);

        header = coalesce_next(flt, header);

        // verify if  we can discard page
        //   if (flNUM_LARGE_CLASSES) discard_empty_page(header,20 * PAGE_SIZE);

        int class = calculate_obj_to_class(header->size);

//        printf("header class free:%d, Size: %d\n", class, header->size);

        if (header->size >= large_min_size) {
            struct FLT_LARGE *to_add_flt = &flt[class];
            move_to_free_list(to_add_flt, header);
        }
    }
}