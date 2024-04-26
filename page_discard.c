#include <sys/mman.h>
#include "page_discard.h"
void discard_empty_page(void *page_address, size_t page_size) {
    madvise(page_address, page_size, MADV_DONTNEED);
    printf("Discarded");
}