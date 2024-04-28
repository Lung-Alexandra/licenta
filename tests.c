#include <stdio.h>
#include <assert.h>

void test_basic_allocation() {
    int NUM_PAGES_TO_FILL = 10;
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        int *a = alloc(2);
//        printf("Address of allocated memory: %p\n", a);
        if (j == 9)
            print_free_full_pages(&small_obj[0], NULL);
        ffree(a);
        if (j == 9)
            print_free_full_pages(&small_obj[0], NULL);
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
    print_free_full_pages(&small_obj[9], NULL);
    printf("Free memory\n");
    ffree(a);
    print_free_full_pages(&small_obj[9], NULL);
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
    print_free_full_pages(&small_obj[9], NULL);
    printf("Free memory\n");
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        if (j == 4) {
            print_free_full_pages(&small_obj[9], NULL);
        }
        ffree(vec_addr[j]);
    }
    print_free_full_pages(&small_obj[9], NULL);
}

void test_fill_and_moveToFree_moveToFull() {
    int NUM_PAGES_TO_FILL = 150;
    size_t *vec_addr[NUM_PAGES_TO_FILL];
//    printf("%lu\n", sizeof(size_t));
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
        vec_addr[j] = a;
//        printf("%d. Address of allocated memory: %p\n", j, (void *) a);

        for (int i = 0; i < 10; i++) {
            a[i] = i;
        }

        for (int i = 0; i < 10; i++) {
            assert(a[i] == i);
        }
        if (j == 60) {
            print_free_full_pages(&small_obj[9], NULL);
            ffree(vec_addr[5]);
            print_free_full_pages(&small_obj[9], NULL);
        }

    }
    print_free_full_pages(&small_obj[9], NULL);
    printf("Free memory\n");
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        ffree(vec_addr[j]);
    }
    print_free_full_pages(&small_obj[9], NULL);
}

void test_fill_and_moveToFree_more_pages_moveToFul() {
    FILE *fp = fopen("bmd_info.txt", "w+");
    if (fp == NULL) {
        fprintf(stderr, "Nu s-a putut deschide fișierul.\n");
    }
    size_t *b = (size_t *) alloc(sizeof(size_t) * 11);
    print_free_full_pages(&small_obj[10], fp);
    ffree(b);
    print_free_full_pages(&small_obj[10], fp);
    int NUM_PAGES_TO_FILL = 150;
    size_t *vec_addr[NUM_PAGES_TO_FILL];
//    printf("%lu\n", sizeof(size_t));
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        if (j != 0)
            print_free_full_pages(&small_obj[9], fp);
        size_t *a = (size_t *) alloc(sizeof(size_t) * 10);
        print_free_full_pages(&small_obj[9], fp);
        vec_addr[j] = a;
//            printf("%d. Address of allocated memory: %p\n", j,(void *)a);

        for (int i = 0; i < 10; i++) {
            a[i] = i;
        }

        for (int i = 0; i < 10; i++) {
            assert(a[i] == i);
        }
        if (j == 140) {
            ffree(vec_addr[5]);
            print_free_full_pages(&small_obj[9], fp);
            ffree(vec_addr[60]);
        }
    }
    printf("Free memory\n");
    for (int j = 0; j < NUM_PAGES_TO_FILL; j++) {
        print_free_full_pages(&small_obj[9], fp);
        ffree(vec_addr[j]);
        vec_addr[j] = NULL;
        print_free_full_pages(&small_obj[9], fp);
    }
    fclose(fp);
}

//void test_medium(){
//
//}
void test_large() {
    int vec_size[] = {504, 508, 513, 505};
    int n = sizeof(vec_size) / sizeof(vec_size[0]);
    FILE *fp = fopen("large_info.txt", "w");
    if (fp == NULL) {
        fprintf(stderr, "Nu s-a putut deschide fișierul.\n");
    }
    printf("%d\n",LARGE_CLASS_SIZE-1);
    for (int i = 0; i < n; ++i) {
        size_t *a = (size_t *) alloc(vec_size[i]);

        fprintf(fp, "-------ALLOC--------\n");
        for (int j = LARGE_CLASS_SIZE - 1; j >= 0; j--) {
            void *current = large_obj[j].free_list;
            if (current != NULL) {
                fprintf(fp, "flt[%d]: ", (large_min_size + j * gap));
                while (current != NULL) {
                    struct OH *oh = (struct OH *)(current );
                    fprintf(fp, "%p (%zu)(next:%p), ", current,oh->size,oh-> next);
                    current = ((struct OH *) current)->next;
                }
                fprintf(fp, "\n");
            }
        }
        fprintf(fp, "-------------------\n");
        printf("%d. Address of allocated memory: %p\n", i, (void *) a);

        flt_free_large(a);

        fprintf(fp, "------FREE--------\n");
        for (int j = LARGE_CLASS_SIZE - 1; j >= 0; j--) {
            void *current = large_obj[j].free_list;
            if (current != NULL) {
                fprintf(fp, "flt[%d]: ", (large_min_size + j * gap));
                while (current != NULL) {
                    struct OH *oh = (struct OH *)(current );
                    fprintf(fp, "%p (%zu)(next:%p), ", current,oh->size,oh-> next);
                    current = ((struct OH *) current)->next;
                }
                fprintf(fp, "\n");
            }
        }
        fprintf(fp, "-------------------\n");
    }
    fclose(fp);

}