#include "flt.h"

#define small_max_size (1<<7)
#define small_min_size (1<<3)
#define gap (1<<3)
#define SMALL_CLASS_SIZE ((small_max_size-small_min_size)/gap)

struct FLT small_obj[SMALL_CLASS_SIZE];

void init() {
    for (int i = 0; i < SMALL_CLASS_SIZE; ++i)
        initialize_FLT(&small_obj[i]);

    void* a = (void*)flt_malloc(&small_obj[0], sizeof(size_t) * 2);
    void* b = (void*)flt_malloc(&small_obj[0], sizeof(size_t) * 2);
    printf("%p \n", a);
    printf("%p \n", b);
    printf("%ld \n", b-a);
}

int main() {
    init();

    size_t* a = (size_t*)flt_malloc(&small_obj[12], sizeof(size_t)*100);
    size_t* b = (size_t*)flt_malloc(&small_obj[12], sizeof(size_t)*100);

    for(int i=0;i <100 ;i++){
        a[i] = i;
    }
    for(int i=0;i <100 ;i++){
        b[i] = (i+2)*10;
    }


    for(int i=0;i <100 ;i++){
        printf("%ld \n", a[i]);
    }
    for(int i=0;i <100 ;i++){
        printf("%ld \n", b[i]);
    }
    return 0;
}