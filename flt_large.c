struct FLTl {
    void *free_page_blocks;
    void *full_page_blocks;
    int mode; // 0 means bumping 1 means reap
};

typedef struct OH {
    size_t size;
    struct OH* next;
};
struct OH *init_OH(struct OH *oh){
    oh->size = 0;
    oh->next= NULL;
    return oh;
}
void initialize_FLTl(struct FLTl *flt) {
    flt->free_page_blocks = NULL;
    flt->full_page_blocks = NULL;
    flt->mode = 0  ;
}
void * memory_map(int obj_size,int page_size){
    void *ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        return NULL; // Handle allocation failure
    }
    struct OH *oh = init_OH(ptr);
    oh->size = obj_size;
    ptr = (void *)(ptr + sizeof (struct OH));
    return ptr;
}
void *flt_malloc_large(struct FLTl *flt, int obj_size, int page_size) {
    if (flt->free_page_blocks == NULL) {
        flt->free_page_blocks = memory_map(obj_size,page_size);
    }
    if (flt-> mode == 0){
        void *to_return = flt-> free_page_blocks;
        if(flt->full_page_blocks == NULL) {
            flt->full_page_blocks = to_return;
        }else{
            void * old_head = flt->full_page_blocks;
            struct OH *header =  (void*)(to_return - sizeof (struct OH));
            header -> next = old_head;
            flt->full_page_blocks = to_return;
        }
        flt -> free_page_blocks = to_return + sizeof (struct OH);
        return to_return;
    }
    if(flt->mode == 1){
        void *current = flt-> free_page_blocks;

    }
}