#define large_min_size 504
#define large_max_size 32016
#define LARGE_CLASS_SIZE ((large_max_size-large_min_size)/gap)
#define gap (1<<3)

struct FLTl {
    void *free_list;
};

struct OH {
    size_t size;
    struct OH *next;
};

struct OH *init_OH(void *ptr) {
    struct OH *oh = (struct OH *) ptr;
    oh->size = 0;
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

void move_to_free_list(struct FLTl *flt, struct OH *oh) {
    if (flt->free_list == NULL) {
        flt->free_list = oh;
    } else {
        oh->next = flt->free_list;
        flt->free_list = oh;
    }
}

void remove_from_free_list(struct FLTl *flt, struct OH *header) {
    // if header point to a next we need to move next to be the header of free list
    if (header->next != NULL) {
        flt->free_list = header->next;
    } else flt->free_list = NULL;
}

// FLT free_list points always to OH
void *flt_malloc_large(struct FLTl *flt, int obj_size, int page_size) {
    unsigned int no_space = 0;
    unsigned int class;
    int start = (obj_size - large_min_size) / gap;
    for (int i = start; i < LARGE_CLASS_SIZE; i++) {
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
        oh->size = large_max_size-sizeof (struct OH);
        flt[LARGE_CLASS_SIZE - 1].free_list = new_page_ptr;

        //Link with the second one and initialize its OH
        void *ptr = (void *) (new_page_ptr + oh->size);
        oh->next = ptr;
        struct OH *oh1 = init_OH(ptr);
        oh1->size = large_max_size-sizeof (struct OH);


        ptr = (void *) (ptr + oh1->size);
        struct OH *oh2 = init_OH(ptr);
        oh2->size = page_size - oh->size - oh1->size - sizeof (struct OH);
        unsigned int class_last_space = (oh2->size - large_min_size) / gap;
        flt[class_last_space].free_list = oh2;
        if (obj_size > oh2->size) {
            class = LARGE_CLASS_SIZE - 1;
        } else {
            class = class_last_space;
        }
    }
    struct FLTl *fltc = &flt[class];
    struct OH *header = fltc->free_list;
    remove_from_free_list(fltc, header);
    size_t old_size = header->size;
    header->size = obj_size;
    void *to_return = (void *) (header + sizeof(struct OH));

    //calculating the class for remaining space
    size_t space_empty = old_size - obj_size ;
    if (space_empty >= large_min_size) {
        class = (space_empty  - large_min_size +sizeof (struct OH)) / gap;
        struct OH *rest = init_OH((void *) (to_return + header->size));
        rest->size = space_empty;
        struct FLTl *fltr = &flt[class];
        move_to_free_list(fltr, rest);
    }
    return to_return;
}

void flt_free_large(void *ptr) {
    struct OH *header = (struct OH *)ptr;


}