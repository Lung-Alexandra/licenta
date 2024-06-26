#include "flt_large.h"

void *track_pages = NULL;
int total_mem = 0;

void initialize_FLT_LARGE(struct FLT_LARGE *flt) {
    flt->free_list = NULL;
}

void move_to_free_list(struct FLT_LARGE *flt, struct OH *oh) {
    if (flt->free_list == NULL) {
        flt->free_list = oh;
        oh->prev_flt = NULL;
        oh->next_flt = NULL;
    } else {
        struct OH *old_head = flt->free_list;
        flt->free_list = oh;
        oh->prev_flt = NULL;
        oh->next_flt = old_head;
        old_head->prev_flt = oh;
    }
}

void remove_from_free_list(struct FLT_LARGE *flt, struct OH *oh) {
    // if oh is first element
    struct OH *prev_flt = oh->prev_flt;
    if (prev_flt == NULL) {
        struct OH *next_flt = oh->next_flt;
        if (next_flt != NULL) {
            flt->free_list = next_flt;
            next_flt->prev_flt = NULL;
        }
        else flt->free_list = NULL;
    } else {
        //in the middle or last
        struct OH *next_flt = oh->next_flt;
        if (next_flt != NULL)
            next_flt->prev_flt = prev_flt;
        prev_flt->next_flt = next_flt;
    }
    oh->prev_flt = NULL;
    oh->next_flt = NULL;
}

int calculate_obj_to_class(int object_size) {
    int class = 0;
    if (object_size > large_min_size) {
        int rest = (object_size - large_min_size) % gap;
        class = (object_size - large_min_size) / gap;
        if (rest) {
            class++;
        }
    }
    return class;
}

int calculate_class_to_obj(int c) {
    int obj_size = large_min_size + c * gap;
    return obj_size;
}

// returns the class in which object will be allocated
// if class = -1 means that we have no space,
// or we didn't allocate space (20 pages)
int flt_find_best_class(struct FLT_LARGE *flt, int obj_size) {
    int start = calculate_obj_to_class(obj_size);
    for (int i = start; i < NUM_LARGE_CLASSES; ++i) {
        struct OH *current = flt[i].free_list;
        if (current != NULL && calculate_class_to_obj(i) >= obj_size) {
            return i;
        }
    }
    return -1;
}

void track_page_blocks(struct OH *new_track) {
    if (track_pages == NULL) {
        track_pages = new_track;
    } else {
        struct OH *old_head = track_pages;
        track_pages = new_track;
        new_track->next_mdata = old_head;
        old_head->prev_mdata = new_track;
    }
}

void remove_track_page_blocks(struct OH *track) {
    if (track->prev_mdata == NULL) {
        struct OH *next_md = track->next_mdata;
        if (next_md != NULL) {
            track_pages = next_md;
            next_md->prev_mdata = NULL;
        } else track_pages = NULL;
    } else {
        struct OH *prev = track->prev_mdata;
        struct OH *next = track->next_mdata;
        if (next != NULL) {
            prev->next_mdata = next;
            next->prev_mdata = prev;
            track->next_mdata = NULL;
            track->prev_mdata = NULL;
        } else {
            prev->next_mdata = NULL;
            track->prev_mdata = NULL;
        }
    }
}

void flt_large_new_page_init(struct FLT_LARGE *flt, int page_size) {
    // the page has 80K,so we split it in 2*32k and 16k
    void *new_page_ptr = memory_map(page_size);
    // create the first 32k OH and put in the free list
    struct OH *oh = init_OH(new_page_ptr);
//    printf("alloc %p\n", oh);
    track_page_blocks(oh);

    oh->size = large_max_size;
    flt[NUM_LARGE_CLASSES - 1].free_list = oh;

    //Link with the second one and initialize its OH
    void *ptr = (void *) (new_page_ptr + oh->size + OH_size);
    struct OH *oh1 = init_OH(ptr);
    oh->next_flt = oh1;
    oh->next_in_memory = oh1;

    oh1->size = large_max_size;
    oh1->prev_flt = oh;
    oh1->prev_in_memory = oh;

    ptr = (void *) (ptr + oh1->size + OH_size);

    struct OH *oh2 = init_OH(ptr);
    oh2->prev_in_memory = oh1;
    oh1->next_in_memory = oh2;

    oh2->size = page_size - oh->size - oh1->size - 3 * OH_size;
    int class_last_space = calculate_obj_to_class(oh2->size);
    flt[class_last_space].free_list = oh2;

}

void split(struct FLT_LARGE *flt, struct OH *head, void *current_ptr, int obj_size) {
    int old_size = head->size;
    int fit_class = calculate_obj_to_class(obj_size);
    int alloc_size = calculate_class_to_obj(fit_class);
    head->size = alloc_size;

//    printf("alloc class size %d\n", alloc_size);

    void *rest_ptr = (void *) (current_ptr + alloc_size);

    struct OH *rest = init_OH(rest_ptr);

    // the space without header is large enough tot keep the minimum object of large obj
    // we need to add it to the corresponding flt class
    int space_empty = old_size - alloc_size - OH_size;
    rest->size = space_empty;

//    printf("rest: %d\n", rest->size);

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

    struct FLT_LARGE *rest_flt = &flt[class];
    move_to_free_list(rest_flt, rest);
}

// FLT_LARGE free_list points always to OH
void *flt_malloc_large(struct FLT_LARGE *flt, int obj_size, int page_size) {
    unsigned int no_space = 0;
    int class = flt_find_best_class(flt, obj_size);
    if (class != -1) {
        no_space = 1;
    }
    // if no_space is 0 means that we need to allocate a new page
    if (no_space == 0) {
        flt_large_new_page_init(flt, page_size);
        class = flt_find_best_class(flt, obj_size);
    }

    // the class in which object fit's is known
    struct FLT_LARGE *current_flt = &flt[class];
    void *ptr = current_flt->free_list;
    struct OH *head = ptr;

    remove_from_free_list(current_flt, head);
    set_slot_occupied(head);

    void *to_return = (void *) (ptr + OH_size);

//    printf("Want to alloc %d ", obj_size);

    // if space without header is large enough to keep the minimum object of large obj
    if (head->size - obj_size - OH_size >= large_min_size) {
        split(flt, head, to_return, obj_size);
    } else {
//        printf("Allocated %d\n", head->size);
    }
    total_mem += head->size;
    return to_return;
}

// we move from prev to prev adding the size of current oh to prev
void *coalesce_prev(struct FLT_LARGE *flt, struct OH *oh) {
    struct OH *prev = oh->prev_in_memory;

    while (prev != NULL && prev->flag == 0 && oh->size + prev->size + OH_size <= large_max_size) {
        int class = calculate_obj_to_class(prev->size);
        struct FLT_LARGE *flt_from = &flt[class];
        remove_from_free_list(flt_from, prev);
        prev->size += OH_size + oh->size;
        prev->next_in_memory = oh->next_in_memory;
        if (oh->prev_mdata != NULL)
            prev->prev_mdata = oh->prev_mdata;
        if (oh->next_mdata != NULL)
            prev->next_mdata = oh->next_mdata;
        reset_OH(oh);
        oh = prev;
        prev = oh->prev_in_memory;
    }

    return oh;
}

// we move from next to next adding the size of next oh to current oh
void *coalesce_next(struct FLT_LARGE *flt, struct OH *oh) {
    struct OH *next = oh->next_in_memory;
    while (next != NULL && next->flag == 0 && oh->size + next->size + OH_size <= large_max_size) {
        int class = calculate_obj_to_class(next->size);
        struct FLT_LARGE *flt_from = &flt[class];
        remove_from_free_list(flt_from, next);

        oh->size += OH_size + next->size;
        oh->next_in_memory = next->next_in_memory;
        if (next->prev_mdata != NULL)
            oh->prev_mdata = next->prev_mdata;
        if (next->next_mdata != NULL)
            oh->next_mdata = next->next_mdata;
        reset_OH(next);
        next = oh->next_in_memory;
    }
    if (next != NULL) {
        next->prev_in_memory = oh;
    }
    return oh;
}

void remove_f(struct FLT_LARGE *flt, struct OH *current) {

    while (current != NULL && current->flag == 0) {
        struct OH *next = current->next_in_memory;
        int class = calculate_obj_to_class(current->size);
        struct FLT_LARGE *to_rem = &flt[class];
        if(to_rem != NULL)
            remove_from_free_list(to_rem, current);
        current = next;
    }
}

struct OH *get_header_track(struct OH *header) {
    struct OH *oh = header;
    while (oh->prev_in_memory != NULL) {
        oh = oh->prev_in_memory;
    }
    return oh;
}

int discard(struct FLT_LARGE *flt, struct OH *header) {
    if (track_pages != NULL) {
        struct OH *page_bloc_start = get_header_track(header);
        int size = 0;
        if (page_bloc_start->flag == 0) {
            size = page_bloc_start->size + OH_size;
            struct OH *next = page_bloc_start->next_in_memory;
            while (next != NULL && next->flag == 0 && size + next->size <= 20 * PAGE_SIZE) {
                size += next->size + OH_size;
                next = next->next_in_memory;
            }
        }
        if (size == 20 * PAGE_SIZE) {
//            fprintf(stderr,"discard %p\n", page_bloc_start);
            remove_track_page_blocks(page_bloc_start);
            remove_f(flt, page_bloc_start);
            discard_empty_page(page_bloc_start, 20 * PAGE_SIZE);
            return 1;
        }
    }
    return 0;
}

void flt_free_large(struct FLT_LARGE *flt, void *ptr) {
    void *header_ptr = (void *) (ptr - OH_size);
//    printf("FREE : %p\n", header_ptr);

    struct OH *header = header_ptr;
//    printf("free %d \n", header->size);

    if (header->flag) {
        set_free_slot(header);
        total_mem -= header->size;

        header = coalesce_prev(flt, header);

        header = coalesce_next(flt, header);

        // verify if  we can discard 20 pages
        int res = discard(flt, header);


        if (res == 0) {

            int class = calculate_obj_to_class(header->size);

            if (header->size >= large_min_size) {
                struct FLT_LARGE *to_add_flt = &flt[class];
                move_to_free_list(to_add_flt, header);
            }
        }
    }
}
