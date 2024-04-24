#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//void check(int condition, const char *message) {
//    if (!condition) {
//        printf("Test failed: %s\n", message);
//        exit(EXIT_FAILURE);
//    }
//}
// Test de bază pentru alocare și umplere a unei pagini
void test_basic_allocation_and_fill() {
    size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
    printf("Address of allocated memory: %p\n", (void *)a);

    for (int i = 0; i < 10; i++) {
        a[i] = i;
    }

    // Verifică că alocarea s-a efectuat cu succes și că obiectele pot fi utilizate corect
    for (int i = 0; i < 10; i++) {
        assert(a[i] == i);
    }

    // Eliberează memoria alocată
    ffree(a);
}

// Test pentru alocarea și umplerea mai multor pagini
void test_multiple_page_allocation_and_fill() {
    int NUM_PAGES_TO_FILL = 150;
    size_t *vec_addr[NUM_PAGES_TO_FILL];
//    printf("%lu\n", sizeof(size_t));
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
        vec_addr[j] = a;
        printf("%d. Address of allocated memory: %p\n", j,(void *)a);

        for (int i = 0; i < 10; i++) {
            a[i] = i;
        }

        for (int i = 0; i < 10; i++) {
            assert(a[i] == i);
        }

    }
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        if (j == 0)
        {
            printf("wtf\n");
        }
        ffree(vec_addr[j]);
    }
}
void test_multiple_page_allocation_fill_and_free() {
    int NUM_PAGES_TO_FILL = 150;
    size_t *vec_addr[NUM_PAGES_TO_FILL];
//    printf("%lu\n", sizeof(size_t));
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
        vec_addr[j] = a;
        printf("%d. Address of allocated memory: %p\n", j,(void *)a);

        for (int i = 0; i < 10; i++) {
            a[i] = i;
        }

        for (int i = 0; i < 10; i++) {
            assert(a[i] == i);
        }
        if(j == 60){
            ffree(vec_addr[5]);
        }

    }
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        ffree(vec_addr[j]);
    }
}
