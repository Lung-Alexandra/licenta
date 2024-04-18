#include "allocator.h"

void init() {
    for (int i = 0; i < SMALL_CLASS_SIZE; ++i)
        initialize_FLT(&small_obj[i]);
    //medium obj
    //large

}

void *alloc(int size) {
    if (size < medium_min_size) {
        int class = -1;
        for (int i = 0; i < SMALL_CLASS_SIZE; ++i) {
            class = i;
            if (i * gap >= size)
                break;
        }
//        printf("%d", class);
        if (class != -1){
            return flt_malloc(&small_obj[class], size);
        }
    }
  return NULL;
}
void* ffree(void* ptr){
    for(int i=0;i<SMALL_CLASS_SIZE;++i) {
        void * res = flt_free(&small_obj[i], ptr);
        if (res == ptr)
            break;
    }
    return NULL;
}