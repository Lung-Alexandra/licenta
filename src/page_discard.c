#include "page_discard.h"

void discard_empty_page(void *page_address, int page_size) {
//    int result = madvise(page_address, page_size, MADV_FREE);
    int result = munmap(page_address, page_size);

    if (result == -1) {
        printf("Error\n");
        exit(EXIT_FAILURE);
    }
}
void discard_empty_extrobj(void *page_address, int page_size) {
    int result = munmap(page_address, page_size);
    if (result == -1) {
        printf("Error\n");
        exit(EXIT_FAILURE);
    }
}

