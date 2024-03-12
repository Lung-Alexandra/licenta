#include "flt.h"

#define small_max_size (1<<7)
#define small_min_size (1<<3)
#define gap (1<<3)
#define SMALL_CLASS_SIZE ((small_max_size-small_min_size)/gap)

struct FLT small_obj[SMALL_CLASS_SIZE];

void init() {
    for (int i = 0; i < SMALL_CLASS_SIZE; ++i)
        initialize_FLT(&small_obj[i]);
    add_to_FLT(&small_obj[0],8);
}

int main() {
    init();
    printf("%zu", ((struct BMD*)small_obj[0].free_page_blocks)->object_size);
    return 0;
}