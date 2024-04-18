#include "allocator.c"

int main() {
    init();
//    void* a1 = (void*)flt_malloc(&small_obj[0], sizeof(size_t) * 2);
//    void* b1 = (void*)flt_malloc(&small_obj[0], sizeof(size_t) * 2);
//    printf("%p \n", a1);
//    printf("%p \n", b1);
//    printf("%ld \n", b1-a1);

    size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
    printf("%p \n", a);
    for (int i = 0; i < 10; i++) {
        a[i] = i;
    }

    size_t *b = (size_t *) alloc(sizeof(size_t) * 10);
//    printf("%p \n", b);
    for (int i = 0; i < 10; i++) {
        b[i] = i  * 10;
    }
//    printf("%p\n", (void*)&a[2]);
//    printf("%zu\n", *((size_t*)&a[2]));
//    printf("%lu\n", sizeof(size_t));


    ffree(a);

    size_t *c = (size_t *) alloc(sizeof(size_t) * 10);
    printf("%p \n", c);

    for (int i = 0; i < 10; i++) {
        c[i] = 3 * i;
    }

    printf("%p \n", a);
    for (int i = 0; i < 10; i++) {
        printf("%ld \n", a[i]);
    }

    ffree(b);
    ffree(c);
    // de ce a ramane cu pointer catre adresa lui c?
    return 0;
}