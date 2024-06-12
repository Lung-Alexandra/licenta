#include <sys/mman.h>
#include "page_discard.h"

void discard_empty_page(void *page_address, int page_size) {
    int result = madvise(page_address, page_size, MADV_DONTNEED);
    if (result == -1) {
        printf("Error\n");
    }
}