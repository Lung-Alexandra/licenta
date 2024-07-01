#ifndef UNTITLED_PAGE_DISCARD_H
#define UNTITLED_PAGE_DISCARD_H

#include<stdio.h>
#include <sys/mman.h>
#include <stdlib.h>

void discard_empty_page(void *page_address, int page_size);
void discard_empty_extrobj(void *page_address, int page_size);

#endif //UNTITLED_PAGE_DISCARD_H
