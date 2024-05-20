#ifndef UNTITLED_FLT_LARGE_H
#define UNTITLED_FLT_LARGE_H

#define large_min_size 504
#define large_max_size 32000
#define gap (1<<3)
#define NUM_LARGE_CLASSES (((large_max_size-large_min_size)/gap)+1)

struct FLT_LARGE {
    void *free_list;
};

void initialize_FLT_LARGE(struct FLT_LARGE *flt);

void move_to_free_list(struct FLT_LARGE *flt, struct OH *oh);

void remove_from_free_list(struct FLT_LARGE *flt, struct OH *oh);

int calculate_obj_to_class(int object_size);

int calculate_class_to_obj(int class);

int flt_find_best_class(struct FLT_LARGE *flt, int obj_size);

void flt_large_new_page_init(struct FLT_LARGE *flt, int page_size);

void *flt_malloc_large(struct FLT_LARGE *flt, int obj_size, int page_size);

void *coalesce_prev(struct FLT_LARGE *flt, struct OH *oh);

void *coalesce_next(struct FLT_LARGE *flt, struct OH *oh);

void flt_free_large(struct FLT_LARGE *flt, void *ptr);

#endif //UNTITLED_FLT_LARGE_H
