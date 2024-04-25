#include <stdio.h>
#include <assert.h>

void test_basic_allocation() {
    int NUM_PAGES_TO_FILL = 10;
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        int *a = alloc(2);
//        printf("Address of allocated memory: %p\n", a);
        ffree(a);
    }

}

void test_basic_allocation_and_fill() {
    size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
    printf("Address of allocated memory: %p\n", (void *) a);

    for (int i = 0; i < 10; i++) {
        a[i] = i;
    }

    for (int i = 0; i < 10; i++) {
        assert(a[i] == i);
    }
    printf("Free memory\n");
    ffree(a);
}

void test_multiple_page_allocation_and_fill() {
    int NUM_PAGES_TO_FILL = 150;
    size_t *vec_addr[NUM_PAGES_TO_FILL];
//    printf("%lu\n", sizeof(size_t));
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
        vec_addr[j] = a;
        printf("%d. Address of allocated memory: %p\n", j, (void *) a);

        for (int i = 0; i < 10; i++) {
            a[i] = i;
        }

        for (int i = 0; i < 10; i++) {
            assert(a[i] == i);
        }

    }
    printf("Free memory\n");
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        ffree(vec_addr[j]);
    }
}

void test_fill_and_moveToFree_moveToFull() {
    int NUM_PAGES_TO_FILL = 150;
    size_t *vec_addr[NUM_PAGES_TO_FILL];
//    printf("%lu\n", sizeof(size_t));
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
        vec_addr[j] = a;
        printf("%d. Address of allocated memory: %p\n", j, (void *) a);

        for (int i = 0; i < 10; i++) {
            a[i] = i;
        }

        for (int i = 0; i < 10; i++) {
            assert(a[i] == i);
        }
        if (j == 60) {
            ffree(vec_addr[5]);
        }

    }
    printf("Free memory\n");
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        ffree(vec_addr[j]);
    }
}

void test_fill_and_moveToFree_more_pages_moveToFul() {
    int NUM_PAGES_TO_FILL = 150;
    size_t *vec_addr[NUM_PAGES_TO_FILL];
//    printf("%lu\n", sizeof(size_t));
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
        vec_addr[j] = a;
//            printf("%d. Address of allocated memory: %p\n", j,(void *)a);

        for (int i = 0; i < 10; i++) {
            a[i] = i;
        }

        for (int i = 0; i < 10; i++) {
            assert(a[i] == i);
        }
        if (j == 140) {
            print_free_full_pages(&small_obj[9]);
            ffree(vec_addr[5]);
            print_free_full_pages(&small_obj[9]);
            ffree(vec_addr[60]);
            print_free_full_pages(&small_obj[9]);
        }
    }
    print_free_full_pages(&small_obj[9]);
    printf("Free memory\n");
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        ffree(vec_addr[j]);
    }
    print_free_full_pages(&small_obj[9]);
}