#define large_min_size 504
#define large_max_size 32000
#define gap (1<<3)
#define LARGE_CLASS_SIZE ((large_max_size-large_min_size)/gap)

struct FLTl {
    void *free_list;
};

struct OH {
    void *next;
    void *prev;
    size_t size;
    int flag; // 0 if slot is empty , 1 otherwise
    int slots_occupied;
};

struct OH *init_OH(void *ptr) {
    struct OH *oh = (struct OH *) ptr;
    oh->size = 0;
    oh->flag = 0;
    oh->slots_occupied = 0;
    oh->prev = NULL;
    oh->next = NULL;
    return oh;
}

void initialize_FLTl(struct FLTl *flt) {
    flt->free_list = NULL;
}

void *memory_map(int page_size) {
    void *ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        return NULL; // Handle allocation failure
    }
    return ptr;
}

void set_slot_occupied(struct OH *oh) {
    oh->flag = 1;
    oh->slots_occupied += 1;
}

void set_free_slot(struct OH *oh) {
    oh->flag = 0;
    oh->slots_occupied -= 1;
}

void move_to_free_list(struct FLTl *flt, struct OH *oh) {
    if (flt->free_list == NULL) {
        flt->free_list = oh;
    } else {
        struct OH *header = (struct OH *) flt->free_list;
        oh->next = header;
        header->prev = oh;
        flt->free_list = oh;
    }
}

void remove_from_free_list(struct FLTl *flt, struct OH *oh) {
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
        oh-> next = NULL;
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

// FLT free_list points always to OH
void *flt_malloc_large(struct FLTl *flt, int obj_size, int page_size) {
    unsigned int no_space = 0;
    unsigned int class;
    int start = 0;
    if (obj_size != large_min_size) {
        start = (obj_size - large_min_size) / gap + 1;
    }
    for (int i = start; i <= LARGE_CLASS_SIZE; i++) {
        struct FLTl *current_flt = &flt[i];
        if (current_flt->free_list != NULL) {
            class = i;
            no_space = 1;
            break;
        }
    }
    if (no_space == 0) {
        // the page has 80K,so we split it in 2*32k and 16k
        void *new_page_ptr = memory_map(page_size);
        // create the first 32k OH and put in the free list
        struct OH *oh = init_OH(new_page_ptr);
        oh->size = large_max_size;
        flt[LARGE_CLASS_SIZE].free_list = new_page_ptr;

        //Link with the second one and initialize its OH
        void *ptr = (void *) (new_page_ptr + oh->size);
        oh->next = ptr;
        struct OH *oh1 = init_OH(ptr);
        oh1->size = large_max_size;
        oh1->prev = oh;

        ptr = (void *) (ptr + oh1->size);
        struct OH *oh2 = init_OH(ptr);
        oh2->size = page_size - oh->size - oh1->size - 3*sizeof(struct OH);
        unsigned int class_last_space = (oh2->size - large_min_size) / gap + 1;
        flt[class_last_space].free_list = oh2;
        if (obj_size > oh2->size) {
            class = LARGE_CLASS_SIZE;
        } else {
            class = class_last_space;
        }
    }

    struct FLTl *current_flt = &flt[class];
    void *ptr = current_flt->free_list;
    struct OH *head = (struct OH *) ptr;

    remove_from_free_list(current_flt, head);
    set_slot_occupied(head);

    size_t old_size = head->size;
    head->size = obj_size;

//    printf("Add : %p\n", ptr);
//    printf("Add : %p\n", ptr + 32);
//
    void *to_return = (void *) (ptr + sizeof(struct OH));
//    printf("Add : %ld\n", to_return - ptr);

    //calculating the class for remaining space
    size_t space_empty = old_size - obj_size ;
    if (space_empty >= large_min_size) {
        if (space_empty != large_min_size) {
            class = (space_empty - large_min_size ) / gap + 1;
        } else class = 0;
        struct OH *rest = init_OH((void *) (to_return + head->size));
        rest->size = space_empty- sizeof(struct OH);
        printf("Free list flt class alloc:%d, Size: %zu\n",class, rest->size);
        struct FLTl *rest_flt = &flt[class];
        move_to_free_list(rest_flt, rest);
        set_free_slot(rest);
    }
    return to_return;
}

void flt_free_large(struct FLTl *flt, void *ptr) {
    unsigned int class = 0;
    void *header_ptr = (void *) (ptr - sizeof(struct OH));
//    printf("Add : %p\n", ptr);
    struct OH *header = (struct OH *) header_ptr;
    header->flag = 0;
//    header->slots_occupied -= 1;
    struct OH *header_prev = (struct OH *) header->prev;
    ptr += header->size;
    struct OH *header_next = (struct OH *) (ptr);
    while (header_prev != NULL && header_prev->flag == 0) {
//        header_prev->slots_occupied += header->slots_occupied - 1;
        header_prev->size += header->size + sizeof(struct OH);
        class = (header->size - large_min_size + sizeof (struct OH)) / gap + 1;
        printf("Header prev class free:%d, Size: %zu\n",class, header_prev->size);
        struct FLTl *to_remove_flt = &flt[class];
        remove_from_free_list(to_remove_flt, header);
        header = header_prev;
        header_prev = (struct OH *) header_prev->prev;
    }
    while (header_next != NULL && header_next-> size !=0 && header_next->flag == 0) {
//        header->slots_occupied += header_next->slots_occupied - 1;
        header->size += header_next->size + sizeof(struct OH);
        class = (header_next->size - large_min_size + sizeof (struct OH)) / gap + 1;
        printf("header next class free:%d, Size: %zu\n",class, header_next->size);
        struct FLTl *to_remove_flt = &flt[class];
        remove_from_free_list(to_remove_flt, header_next);
        ptr+=header_next->size;
        header_next = (struct OH *) (ptr);
    }
    if (header->size != large_min_size) {
        class = (header->size - large_min_size) / gap + 1;
    }
    printf("header class free:%d, Size: %zu\n",class, header->size);

    struct FLTl *to_add_flt = &flt[class];
    move_to_free_list(to_add_flt, header);
}