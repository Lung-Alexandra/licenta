#ifndef UNTITLED_EXTREM_LARGE_H
#define UNTITLED_EXTREM_LARGE_H

struct Header {
    void *next;
    void *prev;
    int size;
};
int Header_size = sizeof (struct Header);
struct Header *init_Header(void *ptr);



#endif //UNTITLED_EXTREM_LARGE_H
